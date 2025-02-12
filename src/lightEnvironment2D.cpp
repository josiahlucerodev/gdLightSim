#include "lightEnvironment2D.h"

//godot_cpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/core/math.hpp>

//std
#include <optional>
#include <variant>

//own
#include <spotLight2D.h>

using namespace godot;

void LightEnvironment2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_display_bvh"), &LightEnvironment2D::get_display_bvh);
	ClassDB::bind_method(D_METHOD("set_display_bvh", "display_bvh"), &LightEnvironment2D::set_display_bvh);
    ClassDB::bind_method(D_METHOD("get_display_aabb"), &LightEnvironment2D::get_display_aabb);
	ClassDB::bind_method(D_METHOD("set_display_aabb", "display_aabb"), &LightEnvironment2D::set_display_aabb);
    ClassDB::bind_method(D_METHOD("get_display_points"), &LightEnvironment2D::get_display_points);
	ClassDB::bind_method(D_METHOD("set_display_points", "display_aabb"), &LightEnvironment2D::set_display_points);
    ClassDB::bind_method(D_METHOD("get_display_midpoints"), &LightEnvironment2D::get_display_midpoints);
	ClassDB::bind_method(D_METHOD("set_display_midpoints", "display_aabb"), &LightEnvironment2D::set_display_midpoints);
    ClassDB::bind_method(D_METHOD("get_display_rays"), &LightEnvironment2D::get_display_rays);
	ClassDB::bind_method(D_METHOD("set_display_rays", "display_aabb"), &LightEnvironment2D::set_display_rays);
    ClassDB::bind_method(D_METHOD("get_ray_arc_spread"), &LightEnvironment2D::get_ray_arc_spread);
	ClassDB::bind_method(D_METHOD("set_ray_arc_spread", "ray_arc_spread"), &LightEnvironment2D::set_ray_arc_spread);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayBVH"), "set_display_bvh", "get_display_bvh");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayAABB"), "set_display_aabb", "get_display_aabb");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayPoints"), "set_display_points", "get_display_points");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayMidpoints"), "set_display_midpoints", "get_display_midpoints");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayRays"), "set_display_rays", "get_display_rays");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rayArcSpread", PROPERTY_HINT_RANGE, "0.0001, 0.01, 0.00001"), "set_ray_arc_spread", "get_ray_arc_spread");
}

void LightEnvironment2D::_notification(int pwhat) {
}

bool LightEnvironment2D::get_display_bvh() const {
    return displayBVH;
}
void LightEnvironment2D::set_display_bvh(const bool displayBVH) {
    this->displayBVH = displayBVH;
}
bool LightEnvironment2D::get_display_aabb() const {
    return displayAABB;
}
void LightEnvironment2D::set_display_aabb(const bool displayAABB) {
    this->displayAABB = displayAABB;
}
bool LightEnvironment2D::get_display_points() const {
    return displayPoints;
}
void LightEnvironment2D::set_display_points(const bool displayPoints) {
    this->displayPoints = displayPoints;
}
bool LightEnvironment2D::get_display_midpoints() const {
    return displayMidpoints;
}
void LightEnvironment2D::set_display_midpoints(const bool displayMidpoints) {
    this->displayMidpoints = displayMidpoints;
}
bool LightEnvironment2D::get_display_rays() const {
    return displayRays;
}
void LightEnvironment2D::set_display_rays(const bool displayRays) {
    this->displayRays = displayRays;
}
double LightEnvironment2D::get_ray_arc_spread() const {
    return rayArcSpread;
}
void LightEnvironment2D::set_ray_arc_spread(const double rayArcSpread) {
    this->rayArcSpread = rayArcSpread;
}

LightEnvironment2D::LightEnvironment2D() {

}

LightEnvironment2D::~LightEnvironment2D() {
    
}

void resetBVH(BVH& bvh) {
    if(bvh.root == nullptr) {
        return;
    }
    bvh.root = nullptr;
    bvh.shapes.clear();
    bvh.nodePool.clear();
}

void LightEnvironment2D::_ready() {
    displayAABB = true;
    displayBVH = true;
    displayPoints = true;
    displayMidpoints = true;
    displayRays = true;
    rayArcSpread = 0.005;
    shapes.clear();
    points.clear();
    resetBVH(bvh);
}

template<typename Type>
void getChildrenOfType(std::vector<Type*>& dst, Node& parent, const String& typeName) {
    Array children = parent.get_children();
    for (std::size_t i = 0; i < children.size(); ++i) {
        Node* child = Object::cast_to<Node>(children[i]);
        if (child) {
            if(child->get_class() == typeName) {
                dst.push_back(Object::cast_to<Type>(child));
            }
            getChildrenOfType<Type>(dst, *child, typeName);
        }
    }
}

template<typename Type>
std::vector<Type*> getChildrenOfType(Node& parent, const String& typeName) {
    std::vector<Type*> childrenOfType;
    getChildrenOfType(childrenOfType, parent, typeName);
    return childrenOfType;
}

ShapeAABB updateAABB(const ShapeAABB rhs, const ShapeAABB lhs) {
    return ShapeAABB {
        Point2{std::min(lhs.min.x, rhs.min.x), std::min(lhs.min.y, rhs.min.y)},
        Point2{std::max(lhs.max.x, rhs.max.x), std::max(lhs.max.y, rhs.max.y)}
    };
}

Shape constructShape(Polygon2D& polygon) {
    PackedVector2Array polygonPoints = polygon.get_polygon();
    if(polygonPoints.size() == 0) {
        return Shape{};
    }
    
    Shape shape;
    shape.points.reserve(polygonPoints.size());
    Point2 point = polygon.get_transform().get_origin() + polygonPoints[0];
    shape.midPoint = Point2(0, 0);
    shape.aabb = ShapeAABB{
        Point2{point.x, point.y},
        Point2{point.x, point.y}
    };
    
    for(std::size_t i = 0; i < polygonPoints.size(); i++) {
        Point2 point = polygonPoints[i] + polygon.get_transform().get_origin();
        shape.aabb = updateAABB(shape.aabb, {Point2(point.x, point.y), Point2(point.x, point.y)});
        shape.midPoint += point;
        shape.points.push_back(point);
    }
    shape.midPoint /= polygonPoints.size();

    return shape;
}

BVH::Node* constructBVHRecursiveHelper(BVH& bvh, ArrayView<Shape> shapes) {
    if(shapes.size == 0) {
        return nullptr;
    }

    BVH::Node& node = *bvh.nodePool.newElement();
    node.b1 = nullptr;
    node.b2 = nullptr;
    
    if (shapes.size < 5) {
        node.shapes = shapes;
        node.aabb = shapes.data[0].aabb;
        node.midPoint = Point2(0, 0);

        for(std::size_t i = 0; i < shapes.size; i++) {
            Shape& shape = shapes.data[i];
            node.aabb = updateAABB(node.aabb, shape.aabb);
            node.midPoint += shape.midPoint;
        }
        node.midPoint /= shapes.size;

    } else {
        std::size_t b1Size = shapes.size / 2;
        node.b1 = constructBVHRecursiveHelper(bvh, ArrayView<Shape>{shapes.data, b1Size});
        node.b2 = constructBVHRecursiveHelper(bvh, ArrayView<Shape>{shapes.data + b1Size, shapes.size - b1Size});
        
        node.aabb = node.b1->aabb;
        node.aabb = updateAABB(node.aabb, node.b2->aabb);
    }

    return &node;
}

void constructBVH(BVH& bvh, std::vector<Shape>& shapes) {
    std::sort(shapes.begin(), shapes.end(), 
        [](Shape lhs, Shape rhs) -> bool { return lhs.midPoint < rhs.midPoint; }
    );
    bvh.shapes = shapes;
    bvh.root = constructBVHRecursiveHelper(bvh, ArrayView<Shape>{bvh.shapes.data(), bvh.shapes.size()});
}

bool isRayIntersectsAABB(const Ray2D& ray, const ShapeAABB& aabb) {
    if (ray.origin.x >= aabb.min.x && ray.origin.x <= aabb.max.x &&
        ray.origin.y >= aabb.min.y && ray.origin.y <= aabb.max.y) {
        return true;
    }
    real_t tmin = (aabb.min.x - ray.origin.x) / ray.direction.x;
    real_t tmax = (aabb.max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax)  {
        std::swap(tmin, tmax);
    }

    real_t tymin = (aabb.min.y - ray.origin.y) / ray.direction.y;
    real_t tymax = (aabb.max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) {
        std::swap(tymin, tymax);
    }

    if (tmin > tymax || tymin > tmax) {
        return false; 
    }

    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }
    return tmax >= 0;
}

std::optional<Point2> rayLineIntersection(const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd) {
    Point2 lineDir = lineEnd - lineStart;
    Point2 rayToLine = lineStart - ray.origin;

    real_t denom = ray.direction.cross(lineDir);
    if (std::fabs(denom) < 1e-5) {
        return {};
    }

    real_t t = rayToLine.cross(lineDir) / denom;
    real_t u = rayToLine.cross(ray.direction) / denom;
    if (t >= 0 && u >= 0 && u <= 1) {
        Point2 intersection = ray.origin + ray.direction * t;
        return intersection;
    }

    return {};
}

std::optional<RayHit2D> shotRayBVHNode(const Ray2D& ray, const BVH::Node* node) {
    if(node == nullptr) {
        return {};
    }
    if(!isRayIntersectsAABB(ray, node->aabb)) {
        return{};
    }

    for(std::size_t i = 0; i < node->shapes.size; i++) {
        Shape& shape = node->shapes.data[i];
        if(shape.points.size() < 2) {
            continue;
        }
        if(!isRayIntersectsAABB(ray, shape.aabb)) {
            continue;
        }

        std::optional<Point2> hitLocation;
        for(std::size_t i = 0; i < shape.points.size(); i++) {
            std::optional<Point2> newHitLocation;
            if(i == shape.points.size() - 1) {
                newHitLocation = rayLineIntersection(ray, 
                    shape.points[i], shape.points[0]);
            } else {
                newHitLocation = rayLineIntersection(ray, 
                    shape.points[i], shape.points[i + 1]);
            }

            if(newHitLocation.has_value()) {
                if(hitLocation.has_value()) {
                    if(ray.origin.distance_to(newHitLocation.value()) < 
                        ray.origin.distance_to(hitLocation.value())) {
                        hitLocation = newHitLocation;
                    } 
                } else {
                    hitLocation = newHitLocation;
                }
            }
        }

        if(hitLocation.has_value()) {
            return RayHit2D{ray, hitLocation.value()};
        }
    }
    
    std::optional<RayHit2D> rayHitOption{};
    rayHitOption = shotRayBVHNode(ray, node->b1);
    if(!rayHitOption.has_value()) {
        rayHitOption = shotRayBVHNode(ray, node->b2);
    }
    return rayHitOption;
}  

std::optional<RayHit2D> shotRay(const Ray2D& ray, const BVH& bvh) {
    return shotRayBVHNode(ray, bvh.root);
}  

void LightEnvironment2D::_process(double delta) {
    shapes.clear();
    points.clear();
    rayMisses.clear();
    rayHits.clear();
    resetBVH(bvh);

    std::vector<Polygon2D*> polygons = getChildrenOfType<Polygon2D>(*this, "Polygon2D");
    if(polygons.empty()) {
        queue_redraw();
        return;
    }

    for(Polygon2D* polygon : polygons) {
        Shape shape = constructShape(*polygon);
        for(Point2& point : shape.points) {
            points.push_back(point);
        }
        shapes.push_back(shape);
    }

    constructBVH(bvh, shapes);

    std::vector<SpotLight2D*> spotLights = getChildrenOfType<SpotLight2D>(*this, "SpotLight2D");
    for(SpotLight2D* spotLight : spotLights) {
        real_t spotLightAngle = spotLight->get_rotation();
        Point2 spotLightLocation = spotLight->get_position();
        real_t spotLightArc = Math::deg_to_rad(spotLight->get_arc());
        int64_t spotLightRayCount = spotLight->get_ray_count();

        std::vector<Point2> pointsInSpotlightArc;
        pointsInSpotlightArc.reserve(points.size());
        {
            real_t angle = spotLightAngle - (spotLightArc / 2);
            pointsInSpotlightArc.push_back(
                spotLightLocation + Point2(cos(angle), sin(angle)));
            pointsInSpotlightArc.push_back(
                spotLightLocation + Point2(cos(angle + spotLightArc), sin(angle + spotLightArc)));
        }

        for(Point2 point : points) {
            if(abs(spotLight->get_angle_to(point)) <= spotLightArc / 2) {
                pointsInSpotlightArc.push_back(point);
            }
        }

        using RayVariant = std::variant<Ray2D, RayHit2D>;

        std::vector<RayVariant> rays;

        int64_t numRaysPerPoint = spotLightRayCount;
        for(Point2 point : pointsInSpotlightArc) {
            Vector2 distance = spotLightLocation.direction_to(point);
            real_t arc = (rayArcSpread / distance.length()) / numRaysPerPoint;
            for(std::size_t i = 0; i < numRaysPerPoint; i++) {
                real_t angle = spotLight->get_angle_to(point) - (arc / 2);
                angle += ((arc / numRaysPerPoint) * i);
                Point2 direction = Point2(cos(angle), sin(angle));
                Ray2D ray = Ray2D{spotLightLocation, direction};

                std::optional<RayHit2D> rayHit = shotRay(ray, bvh);
                if(rayHit.has_value()) {
                    rays.push_back(RayVariant(rayHit.value()));
                } else {
                    rays.push_back(RayVariant(ray));
                }

                if(displayRays) {
                    if(rayHit.has_value()) {
                        rayHits.push_back(rayHit.value());
                    } else {
                        rayMisses.push_back(ray);
                    }
                }
            }
        }

        auto getRay = [](const RayVariant& rayVariant) -> const Ray2D& {
            return std::visit([](auto&& arg) -> const Ray2D& {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, Ray2D>) {
                    return arg;
                } else {
                    return arg.ray;
                }
            }, rayVariant);
        } ;

        std::sort(rays.begin(), rays.end(), 
            [&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
                return spotLight->get_angle_to(getRay(lhs).direction) < spotLight->get_angle_to(getRay(rhs).direction);
            }
        );

        /*
        for(std::size_t i = 0; i < spotLight->get_ray_count(); i++) {
            real_t angle = spotLightAngle - (Math::deg_to_rad(spotLight->get_arc()) / 2);
            angle += ((Math::deg_to_rad(spotLight->get_arc()) / spotLight->get_ray_count()) * i);
            Point2 direction = Point2(cos(angle), sin(angle));
            Ray2D ray = Ray2D{spotLightLocation, direction};
            
            std::optional<RayHit2D> rayHit = shotRay(ray, bvh);
            if(rayHit.has_value()) {
                rayHits.push_back(rayHit.value());
            } else {
                rayMisses.push_back(ray);
            }
        }
        */
    }
    
    queue_redraw();
}

Rect2 createRect(ShapeAABB& aabb) {
    return Rect2(aabb.min, aabb.max - aabb.min);
}

void drawOneAABB(LightEnvironment2D& env, ShapeAABB aabb, Color color) {
    env.draw_rect(createRect(aabb), color, false);
    env.draw_circle(aabb.min, 10, color, true);
    env.draw_circle(aabb.max, 10, color, true);
} 

void drawBVHNode(LightEnvironment2D& env, BVH::Node* node) {
    if(node == nullptr) {
        return;
    }
    drawOneAABB(env, node->aabb, Color(1.0, 1.0, 1.0));
    drawBVHNode(env, node->b1);
    drawBVHNode(env, node->b2);
}
void drawBVH(LightEnvironment2D& env) {
    drawBVHNode(env, env.bvh.root);
}

void drawBVHNodeMidpoints(LightEnvironment2D& env, BVH::Node* node) {
    if(node == nullptr) {
        return;
    }
    env.draw_circle(node->midPoint, 10, Color(1.0, 1.0, 0.0), true);
    drawBVHNodeMidpoints(env, node->b1);
    drawBVHNodeMidpoints(env, node->b2);
}
void drawBVHMidpoints(LightEnvironment2D& env) {
    drawBVHNodeMidpoints(env, env.bvh.root);
}


void LightEnvironment2D::_draw() {
    if(displayBVH) {
        drawBVH(*this);
    }
    if(displayAABB) {
        for(Shape& shape : shapes) {
            drawOneAABB(*this, shape.aabb, Color(1.0, 0.0, 0.0));
        }
    }
    if(displayPoints) {
        for(Point2 point : points) {
            draw_circle(point, 10, Color(0.0, 1.0, 1.0), true);
        }
    }
    if(displayMidpoints) {
        for(Shape& shape : shapes) {
            draw_circle(shape.midPoint, 10, Color(1.0, 1.0, 0.0), true);
        }
        drawBVHMidpoints(*this);
    }
    if(displayRays) {
        for(Ray2D ray : rayMisses) {
            draw_line(ray.origin, ray.origin + (ray.direction * 10000), Color(1.0, 0.0, 0.0));
        }
        for(RayHit2D rayhit : rayHits) {
            draw_line(rayhit.ray.origin, rayhit.location, Color(0.0, 1.0, 0.0));
            draw_circle(rayhit.location, 10, Color(0.0, 1.0, 0.0), true);
        }
    }
}