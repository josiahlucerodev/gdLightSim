#include "lightEnvironment2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/core/math.hpp>

//own
#include "spotLight2D.h"
#include "beamLight2D.h"
#include "circleLight2D.h"

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
    ClassDB::bind_method(D_METHOD("get_display_radial_scan_sections"), &LightEnvironment2D::get_display_radial_scan_sections);
	ClassDB::bind_method(D_METHOD("set_display_radial_scan_sections", "display_rss"), &LightEnvironment2D::set_display_radial_scan_sections);
    ClassDB::bind_method(D_METHOD("get_display_linear_scan_sections"), &LightEnvironment2D::get_display_linear_scan_sections);
	ClassDB::bind_method(D_METHOD("set_display_linear_scan_sections", "display_lss"), &LightEnvironment2D::set_display_linear_scan_sections);

    ClassDB::bind_method(D_METHOD("get_radial_ray_spread"), &LightEnvironment2D::get_radial_ray_spread);
	ClassDB::bind_method(D_METHOD("set_radial_ray_spread", "radial_ray_spread"), &LightEnvironment2D::set_radial_ray_spread);
    ClassDB::bind_method(D_METHOD("get_radial_section_tolerance"), &LightEnvironment2D::get_radial_section_tolerance);
	ClassDB::bind_method(D_METHOD("set_radial_section_tolerance", "tolerance"), &LightEnvironment2D::set_radial_section_tolerance);
    ClassDB::bind_method(D_METHOD("get_linear_ray_spread"), &LightEnvironment2D::get_linear_ray_spread);
	ClassDB::bind_method(D_METHOD("set_linear_ray_spread", "linear_ray_spread"), &LightEnvironment2D::set_linear_ray_spread);
    ClassDB::bind_method(D_METHOD("get_linear_section_tolerance"), &LightEnvironment2D::get_linear_section_tolerance);
	ClassDB::bind_method(D_METHOD("set_linear_section_tolerance", "tolerance"), &LightEnvironment2D::set_linear_section_tolerance);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayBVH"), "set_display_bvh", "get_display_bvh");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayAABB"), "set_display_aabb", "get_display_aabb");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayPoints"), "set_display_points", "get_display_points");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayMidpoints"), "set_display_midpoints", "get_display_midpoints");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayRays"), "set_display_rays", "get_display_rays");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayRadialScanSections"), "set_display_radial_scan_sections", "get_display_radial_scan_sections");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayLinearScanSections"), "set_display_linear_scan_sections", "get_display_linear_scan_sections");

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radialRaySpread", PROPERTY_HINT_RANGE, "1, 50, 0.1"), "set_radial_ray_spread", "get_radial_ray_spread");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radialSectionTolerance", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_radial_section_tolerance", "get_radial_section_tolerance");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linearRaySpread", PROPERTY_HINT_RANGE, "1, 50, 0.1"), "set_linear_ray_spread", "get_linear_ray_spread");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linearSectionTolerance", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_linear_section_tolerance", "get_linear_section_tolerance");
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
bool LightEnvironment2D::get_display_radial_scan_sections() const {
    return displayRadialScanSections;
}
void LightEnvironment2D::set_display_radial_scan_sections(const bool displayRadialScanSections) {
    this->displayRadialScanSections = displayRadialScanSections;
}
bool LightEnvironment2D::get_display_linear_scan_sections() const {
    return displayLinearScanSections;
}
void LightEnvironment2D::set_display_linear_scan_sections(const bool displayLinearScanSections) {
    this->displayLinearScanSections = displayLinearScanSections;
}

double LightEnvironment2D::get_radial_ray_spread() const {
    return radialRaySpread;
}
void LightEnvironment2D::set_radial_ray_spread(const double radialRaySpread) {
    this->radialRaySpread = radialRaySpread;
}
double LightEnvironment2D::get_radial_section_tolerance() const {
    return radialSectionTolerance;
}
void LightEnvironment2D::set_radial_section_tolerance(const double radialSectionTolerance) {
    this->radialSectionTolerance = radialSectionTolerance;
}
double LightEnvironment2D::get_linear_ray_spread() const {
    return linearRaySpread;
}
void LightEnvironment2D::set_linear_ray_spread(const double linearRaySpread) {
    this->linearRaySpread = linearRaySpread;
}
double LightEnvironment2D::get_linear_section_tolerance() const {
    return linearSectionTolerance;
}
void LightEnvironment2D::set_linear_section_tolerance(const double linearSectionTolerance) {
    this->linearSectionTolerance = linearSectionTolerance;
}
LightEnvironment2D::LightEnvironment2D() {}
LightEnvironment2D::~LightEnvironment2D() {}

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
    displayRadialScanSections = true;
    displayLinearScanSections = true;
    
    radialSectionTolerance = 0.1;
    linearSectionTolerance = 0.1;
    shapes.clear();
    points.clear();
    radialScanSections.clear();
    resetBVH(bvh);
}

template<typename Type>
void getChildrenOfType(std::vector<Type*>& dst, Node& parent, const String& typeName) {
    Array children = parent.get_children();
    for (std::size_t i = 0; i < children.size(); ++i) {
        Node2D* child = Object::cast_to<Node2D>(children[i]);
        if (child && child->is_visible()) {
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

Shape constructShape(Polygon2D& polygon, std::size_t shapeId) {
    PackedVector2Array polygonPoints = polygon.get_polygon();
    if(polygonPoints.size() == 0) {
        return Shape{};
    }
    
    Shape shape;
    shape.shapeId = shapeId;
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

std::optional<RayHit2D> rayLineHit(std::size_t shapeId, const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd) {
    std::optional<Point2> intersectionOption = rayLineIntersection(ray, lineStart, lineEnd);
    if(!intersectionOption.has_value()) {
        return std::nullopt;
    }
    Point2 intersection = intersectionOption.value();
    Vector2 lineDir = lineEnd - lineStart;
    Vector2 normal = {-lineDir.y, lineDir.x};  
    normal.normalize();
    real_t angle = ray.direction.angle_to(normal);

    return RayHit2D{shapeId, angle, ray, intersection};
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

        std::optional<RayHit2D> rayHit;
        for(std::size_t i = 0; i < shape.points.size(); i++) {
            std::optional<RayHit2D> newRayHit;
            if(i == shape.points.size() - 1) {
                newRayHit = rayLineHit(shape.shapeId, ray,
                    shape.points[i], shape.points[0]);
            } else {
                newRayHit = rayLineHit(shape.shapeId, ray, 
                    shape.points[i], shape.points[i + 1]);
            }

            if(newRayHit.has_value()) {
                if(rayHit.has_value()) {
                    if(ray.origin.distance_to(newRayHit.value().location) < 
                        ray.origin.distance_to(rayHit.value().location)) {
                        rayHit = newRayHit;
                    } 
                } else {
                    rayHit = newRayHit;
                }
            }
        }

        return rayHit;
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

bool isRelativelyEqual(real_t a, real_t b, real_t epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}
const Ray2D& getRay(const RayVariant& rayVariant) {
    return std::visit([](auto&& arg) -> const Ray2D& {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Ray2D>) {
            return arg;
        } else {
            return arg.ray;
        }
    }, rayVariant);
} ;

template<typename Section, typename Predicate>
std::vector<Section> generateSections(const std::vector<RayVariant>& rays, Predicate&& simplificationPredicate) {
    std::vector<Section> sections;
    size_t i = 0;
    while(i < rays.size()) {

        std::vector<RayHit2D> hitGroup;
        while(i < rays.size()) {
            if (std::holds_alternative<RayHit2D>(rays[i])) {
                hitGroup.push_back(std::get<1>(rays[i])); 
                i++;
            } else {
                break;
            }
        }

        if(hitGroup.size() == 2) {
            sections.push_back(
                Section {
                    SectionType::hit,
                    hitGroup.front(),
                    hitGroup.back()
                }
            );
        } else if(hitGroup.size() > 2) {
            std::vector<RayHit2D> simplifiedHitGroup;
            simplifiedHitGroup.push_back(hitGroup.front());
            for (size_t i = 1; i < hitGroup.size() - 1; i++) {
                RayHit2D& r1 = simplifiedHitGroup.back();
                RayHit2D& r2 = hitGroup[i];
                RayHit2D& r3 = hitGroup[i + 1];
            
                if (simplificationPredicate(r1, r2, r3)) {
                    simplifiedHitGroup.push_back(r2);
                }
            }

            simplifiedHitGroup.push_back(hitGroup.back());

            for(size_t i = 0; i < simplifiedHitGroup.size() - 1; i++) {
                RayHit2D& r1 = simplifiedHitGroup[i];
                RayHit2D& r2 = simplifiedHitGroup[i+1];
                sections.push_back(
                    Section {
                        SectionType::hit,
                        r1, r2
                    }
                );
            }
        }

        std::vector<Ray2D> missGroup;
        while(i < rays.size()) {
            if (std::holds_alternative<Ray2D>(rays[i])) {
                missGroup.push_back(std::get<0>(rays[i])); 
                i++;
            } else {
                break;
            }
        }

        if(missGroup.size() >= 2) {
            sections.push_back(
                Section {
                    SectionType::miss,
                    missGroup.front(),
                    missGroup.back()
                }
            );
        }
    } 
    return sections;
};

struct BeamRayToPoint {
    real_t beamOriginDistance;
    Vector2 rayOrigin;
};

std::optional<BeamRayToPoint> getBeamRayToPoint(Point2 beamLocation, real_t beamRotiation, 
    Point2 unscaledRightBeamPoint, Point2 point) {
        
    real_t theta = beamLocation.angle_to_point(point) - (beamRotiation + Math::deg_to_rad(real_t(90)));
    if(abs(theta + Math::deg_to_rad(real_t(90))) > (Math_PI / 2)) {
        return std::nullopt;
    }
    
    real_t h = beamLocation.distance_to(point);
    real_t a = cos(theta) * h;
    Vector2 origin = (unscaledRightBeamPoint * a) + beamLocation;

    BeamRayToPoint beamRayToPoint{};
    beamRayToPoint.beamOriginDistance = a;
    beamRayToPoint.rayOrigin = origin;
    return beamRayToPoint;
}

void LightEnvironment2D::_process(double delta) {
    shapes.clear();
    points.clear();
    rayMisses.clear();
    rayHits.clear();
    radialScanSections.clear();
    linearScanSections.clear();
    resetBVH(bvh);

    std::vector<Polygon2D*> polygons = getChildrenOfType<Polygon2D>(*this, "Polygon2D");
    if(polygons.empty()) {
        queue_redraw();
        return;
    }

    for(std::size_t i = 0; i < polygons.size(); i++) {
        Polygon2D* polygon = polygons[i];
        Shape shape = constructShape(*polygon, i);
        for(Point2& point : shape.points) {
            points.push_back(point);
        }
        shapes.push_back(shape);
    }

    constructBVH(bvh, shapes);

    auto shotRayAndAdd = [&](std::vector<RayVariant>& rays, 
        const Ray2D& ray) {
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
    };

    std::vector<SpotLight2D*> spotLights = getChildrenOfType<SpotLight2D>(*this, "SpotLight2D");
    for(SpotLight2D* spotLight : spotLights) {
        real_t spotLightAngle = spotLight->get_rotation();
        Point2 spotLightLocation = spotLight->get_position();
        real_t spotLightArc = Math::deg_to_rad(spotLight->get_arc());
        int64_t spotLightRayCount = spotLight->get_ray_count();

        std::vector<Point2> pointsInSpotlightArc;
        pointsInSpotlightArc.reserve(points.size());
        std::vector<RayVariant> rays;
        
        auto testRay = [&](Point2 direction) {
            Ray2D ray = Ray2D{spotLightLocation, direction};
            shotRayAndAdd(rays, ray);
        };
        
        {
            real_t angle = spotLightAngle - (spotLightArc / 2);
            Point2 startRay = Point2(cos(angle), sin(angle));
            testRay(startRay);
            Point2 endRay = Point2(cos(angle + spotLightArc), sin(angle + spotLightArc));
            testRay(endRay);
        }
        
        
        for(Point2 point : points) {
            if(abs(spotLightLocation.angle_to_point(point)) <= spotLightArc / 2) {
                pointsInSpotlightArc.push_back(point);
            }
        }

        for(size_t i = 0; i < pointsInSpotlightArc.size(); i++) {
            Point2& point = pointsInSpotlightArc[i];
            real_t distance = spotLightLocation.distance_to(point);
            real_t arc = (radialRaySpread / distance) / spotLightRayCount;

            for(std::size_t i = 0; i < spotLightRayCount; i++) {
                real_t angle = spotLightLocation.angle_to_point(point) - (arc / 2);
                angle += ((arc / spotLightRayCount) * i);
                Point2 direction = Point2(cos(angle), sin(angle));
                testRay(direction);
            }
        }
        
        std::sort(rays.begin(), rays.end(), 
            [&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
                return spotLightLocation.angle_to_point(getRay(lhs).direction) < spotLightLocation.angle_to_point(getRay(rhs).direction);
            }
        );

        
        auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
            auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
                return (p2.y - p1.y) / (p2.x - p1.x);
            };            

            double slope1 = calculateSlope(r1.location, r2.location);
            double slope2 = calculateSlope(r2.location, r3.location);
            return std::abs(slope1 - slope2) > radialSectionTolerance;
        };

       std::vector<RadialScanSection> sections = generateSections<RadialScanSection>(rays, predicate);
       radialScanSections.insert(radialScanSections.begin(), sections.begin(), sections.end());
    }

    std::vector<BeamLight2D*> beamLights = getChildrenOfType<BeamLight2D>(*this, "BeamLight2D");
    for(BeamLight2D* beamLight : beamLights) {
        Point2 beamLightLocation = beamLight->get_position();
        real_t beamLightWidth = beamLight->get_light_width();
        int64_t beamLightRayCount = beamLight->get_ray_count();
        real_t beamLightRotation = beamLight->get_rotation();

        Vector2 beamDirection = Vector2(cos(beamLightRotation), sin(beamLightRotation));
        real_t piOver2 = Math_PI / 2;
        Vector2 unscaledRightBeamPoint = Vector2(cos(beamLightRotation + piOver2), sin(beamLightRotation + piOver2));
        Vector2 rightBeamPoint = unscaledRightBeamPoint * beamLightWidth;
        rightBeamPoint += beamLightLocation;
        Vector2 unscaledLeftBeamPoint = Vector2(cos(beamLightRotation - piOver2), sin(beamLightRotation - piOver2));
        Vector2 leftBeamPoint = unscaledLeftBeamPoint * beamLightWidth;
        leftBeamPoint += beamLightLocation;

        std::vector<BeamRayToPoint> beamRays;
        beamRays.reserve(points.size());

        std::vector<RayVariant> rays;
        auto testRay = [&](Ray2D ray) {
            shotRayAndAdd(rays, ray);
        };

        for(Point2 point : points) {
            std::optional<BeamRayToPoint> rayToPointOption = getBeamRayToPoint(beamLightLocation, beamLightRotation
                , unscaledRightBeamPoint, point);
            if(rayToPointOption.has_value()) {
                BeamRayToPoint rayToPoint = rayToPointOption.value();
                if(abs(rayToPoint.beamOriginDistance) < (beamLightWidth)) {
                    beamRays.push_back(rayToPoint);
                }
            }
        }
        
        {
            testRay(Ray2D{leftBeamPoint, beamDirection});
            testRay(Ray2D{rightBeamPoint, beamDirection});
        }

        for(size_t i = 0; i < beamRays.size(); i++) {
            BeamRayToPoint& rayToPoint = beamRays[i];

            for(std::size_t i = 0; i < beamLightRayCount; i++) {
                real_t offset = (linearRaySpread / -2) + ((linearRaySpread / beamLightRayCount) * i);
                Vector2 spreadOrigin = rayToPoint.rayOrigin + Vector2{offset, offset};
                testRay(Ray2D{spreadOrigin, beamDirection});
            }
        }

        std::sort(rays.begin(), rays.end(), 
            [&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
                Point2 lhsO = getRay(lhs).origin;
                Point2 rhsO = getRay(rhs).origin;

                return Math::sign(beamLightLocation.angle_to(lhsO)) * beamLightLocation.distance_to(lhsO)
                    < Math::sign(beamLightLocation.angle_to(rhsO)) * beamLightLocation.distance_to(rhsO);
            }
        );

        auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
            auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
                return (p2.y - p1.y) / (p2.x - p1.x);
            };            

            double slope1 = calculateSlope(r1.location, r2.location);
            double slope2 = calculateSlope(r2.location, r3.location);
            return std::abs(slope1 - slope2) > linearSectionTolerance;
        };

        std::vector<LinearScanSection> sections = generateSections<LinearScanSection>(rays, predicate);
        linearScanSections.insert(linearScanSections.begin(), sections.begin(), sections.end());
    }

    std::vector<CircleLight2D*> pointLights = getChildrenOfType<CircleLight2D>(*this, "CircleLight2D");
    for(CircleLight2D* pointLight : pointLights) {
        Point2 pointLightLocation = pointLight->get_position();
        int64_t pointLightRayCount = pointLight->get_ray_count();


        std::vector<RayVariant> rays;
        auto testRay = [&](Ray2D ray) {
            shotRayAndAdd(rays, ray);
        };

        for(Point2 point : points) {
            real_t distance = pointLightLocation.distance_to(point);
            real_t arc = (radialRaySpread / distance) / pointLightRayCount;

            for(std::size_t i = 0; i < pointLightRayCount; i++) {
                real_t angle = pointLightLocation.angle_to_point(point) - (arc / 2);
                angle += ((arc / pointLightRayCount) * i);
                Point2 direction = Point2(cos(angle), sin(angle));
                testRay(Ray2D{pointLightLocation, direction});
            }
        }
        
        std::sort(rays.begin(), rays.end(), 
            [&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
                return pointLightLocation.angle_to_point(getRay(lhs).direction) < pointLightLocation.angle_to_point(getRay(rhs).direction);
            }
        );

        auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
            auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
                return (p2.y - p1.y) / (p2.x - p1.x);
            };            

            double slope1 = calculateSlope(r1.location, r2.location);
            double slope2 = calculateSlope(r2.location, r3.location);
            return std::abs(slope1 - slope2) > radialSectionTolerance;
        };

       std::vector<RadialScanSection> sections = generateSections<RadialScanSection>(rays, predicate);
       radialScanSections.insert(radialScanSections.begin(), sections.begin(), sections.end());
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

void drawRayMiss(LightEnvironment2D& env, const Ray2D& ray) {
    env.draw_line(ray.origin, ray.origin + (ray.direction * 10000), Color(1.0, 0.0, 0.0));
}

void drawRayHit(LightEnvironment2D& env, const RayHit2D& rayHit) {
    env.draw_line(rayHit.ray.origin, rayHit.location, Color(0.0, 1.0, 0.0));
    env.draw_circle(rayHit.location, 10, Color(0.0, 1.0, 0.0), true);
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
            drawRayMiss(*this, ray);
        }
        for(RayHit2D rayhit : rayHits) {
            drawRayHit(*this, rayhit); 
        }
    }
    if(displayRadialScanSections) {
        for(RadialScanSection& section : radialScanSections) {
            switch (section.type) {
            case SectionType::hit: {
                RayHit2D& startHit = std::get<1>(section.startRay);    
                RayHit2D& endHit = std::get<1>(section.endRay);    
                
                drawRayHit(*this, startHit); 
                drawRayHit(*this, endHit); 
                draw_line(startHit.location, endHit.location, Color(0.0, 1.0, 0.0));
                break;
            }
            case SectionType::miss: {
                Ray2D& startRay = std::get<0>(section.startRay);    
                Ray2D& endRay = std::get<0>(section.endRay);    
                
                drawRayMiss(*this, startRay); 
                drawRayMiss(*this, endRay); 
                for(std::size_t i = 0; i < 10000 / 20; i++) {
                    draw_line((startRay.direction * (100 * i)) + startRay.origin, 
                        (endRay.direction * (100 * i)) + endRay.origin, 
                        Color(1.0, 0.0, 0.0));   
                }
                break;
            }
            default:
                break;
            }
        }
    }
    if(displayLinearScanSections) {
        for(LinearScanSection& section : linearScanSections) {
            switch (section.type) {
            case SectionType::hit: {
                RayHit2D& startHit = std::get<1>(section.startRay);    
                RayHit2D& endHit = std::get<1>(section.endRay);    
                
                drawRayHit(*this, startHit); 
                drawRayHit(*this, endHit); 
                draw_line(startHit.location, endHit.location, Color(0.0, 1.0, 0.0));
                break;
            }
            case SectionType::miss: {
                Ray2D& startRay = std::get<0>(section.startRay);    
                Ray2D& endRay = std::get<0>(section.endRay);    
                
                drawRayMiss(*this, startRay); 
                drawRayMiss(*this, endRay); 
                for(std::size_t i = 0; i < 10000 / 20; i++) {
                    draw_line((startRay.direction * (100 * i)) + startRay.origin, 
                        (endRay.direction * (100 * i)) + endRay.origin, 
                        Color(1.0, 0.0, 0.0));   
                }
                break;
            }
            default:
                break;
            }
        }
    }
}