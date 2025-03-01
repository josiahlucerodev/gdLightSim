#include "lightEnvironment2D.h"

//std
#include <deque>

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
#include "mirror2D.h"
#include "angle.h"

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

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayBVH2D"), "set_display_bvh", "get_display_bvh");
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
    return displayBVH2D;
}
void LightEnvironment2D::set_display_bvh(const bool displayBVH2D) {
    this->displayBVH2D = displayBVH2D;
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

void LightEnvironment2D::_ready() {
    displayAABB = true;
    displayBVH2D = true;
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
    resetBVH2D(bvh);
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

bool isRelativelyEqual(real_t a, real_t b, real_t epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

void LightEnvironment2D::_process(double delta) {
    shapes.clear();
    points.clear();
    allShotRays.clear();
    radialScanSections.clear();
    linearScanSections.clear();
    resetBVH2D(bvh);

    std::vector<Polygon2D*> polygons = getChildrenOfType<Polygon2D>(*this, "Polygon2D");
    std::vector<Mirror2D*> mirrors = getChildrenOfType<Mirror2D>(*this, "Mirror2D");

    if(mirrors.empty() && polygons.empty()) {
        queue_redraw();
        return;
    }

    for(Polygon2D* polygon : polygons) {
        Shape2D shape = constructShape2D(*polygon, shapes.size());
        shapes.push_back(shape);
    }

    for(Mirror2D* mirror : mirrors) {
        Shape2D shape = constructShape2D(*mirror, shapes.size());
        shapes.push_back(shape);
    }


    constructBVH2D(bvh, shapes);
    points = getPoints(shapes);

    std::vector<SpotLight2D*> spotLights = getChildrenOfType<SpotLight2D>(*this, "SpotLight2D");
    for(SpotLight2D* spotLight : spotLights) {
        std::vector<RayVariant> rays = shotSpotLight2D(*spotLight, points, bvh,  radialRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<RadialScanSection> sections = generateSpotLight2DSections(*spotLight, rays, shapes, radialSectionTolerance);
        radialScanSections.insert(radialScanSections.end(), sections.begin(), sections.end());
    }
    
    std::vector<CircleLight2D*> circleLights = getChildrenOfType<CircleLight2D>(*this, "CircleLight2D");
    for(CircleLight2D* circleLight : circleLights) {
        std::vector<RayVariant> rays = shotCircleLight2D(*circleLight, points, bvh,  radialRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<RadialScanSection> sections = generateCircleLight2DSections(*circleLight, rays, shapes, radialSectionTolerance);
        radialScanSections.insert(radialScanSections.end(), sections.begin(), sections.end());
    }
    
    std::vector<BeamLight2D*> beamLights = getChildrenOfType<BeamLight2D>(*this, "BeamLight2D");
    for(BeamLight2D* beamLight : beamLights) {
        std::vector<RayVariant> rays = shotBeamLight2D(*beamLight, points, bvh,  linearRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<LinearScanSection> sections = generateBeamLight2DSections(*beamLight, rays, shapes, linearSectionTolerance);
        linearScanSections.insert(linearScanSections.end(), sections.begin(), sections.end());
    }


    std::deque<LinearScanSection> linearMirrorSectionQueue;
    addLinearMirrorBounceSectionsToQueue(shapes, linearMirrorSectionQueue, 
        linearScanSections, 100);

    std::size_t lssIndex = 0;
    while(!linearMirrorSectionQueue.empty() && lssIndex < 1000) {
        LinearScanSection linearMirrorSection = linearMirrorSectionQueue.front();
        linearMirrorSectionQueue.pop_front();

        std::vector<RayVariant> rays = shotMirrorBeam(linearMirrorSection, shapes, bvh,  linearRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<LinearScanSection> sections = generateMirrorBeamSections(linearMirrorSection, rays, shapes, linearSectionTolerance);
        linearScanSections.insert(linearScanSections.end(), sections.begin(), sections.end());
        
	    const Shape2D& mirrorShape = shapes[linearMirrorSection.shapeId];
        addLinearMirrorBounceSectionsToQueue(shapes, linearMirrorSectionQueue, sections, mirrorShape.maxBounce);

        lssIndex++;
	}

    queue_redraw();
}

Rect2 createRect(AABB2D& aabb) {
    return Rect2(aabb.min, aabb.max - aabb.min);
}

void drawOneAABB(LightEnvironment2D& env, AABB2D aabb, Color color) {
    env.draw_rect(createRect(aabb), color, false);
    env.draw_circle(aabb.min, 10, color, true);
    env.draw_circle(aabb.max, 10, color, true);
} 

void drawBVH2DNode(LightEnvironment2D& env, BVH2D::Node* node) {
    if(node == nullptr) {
        return;
    }
    drawOneAABB(env, node->aabb, Color(1.0, 1.0, 1.0));
    drawBVH2DNode(env, node->b1);
    drawBVH2DNode(env, node->b2);
}
void drawBVH2D(LightEnvironment2D& env) {
    drawBVH2DNode(env, env.bvh.root);
}

void drawBVH2DNodeMidpoints(LightEnvironment2D& env, BVH2D::Node* node) {
    if(node == nullptr) {
        return;
    }
    env.draw_circle(node->midPoint, 10, Color(1.0, 1.0, 0.0), true);
    drawBVH2DNodeMidpoints(env, node->b1);
    drawBVH2DNodeMidpoints(env, node->b2);
}
void drawBVH2DMidpoints(LightEnvironment2D& env) {
    drawBVH2DNodeMidpoints(env, env.bvh.root);
}

void drawRayMiss(LightEnvironment2D& env, const Ray2D& ray) {
    env.draw_line(ray.origin, ray.origin + (ray.direction * 10000), Color(1.0, 0.0, 0.0));
}

void drawRayHit(LightEnvironment2D& env, const RayHit2D& rayHit) {
    env.draw_line(rayHit.ray.origin, rayHit.location, Color(0.0, 1.0, 0.0));
    env.draw_circle(rayHit.location, 10, Color(0.0, 1.0, 0.0), true);
}


void LightEnvironment2D::_draw() {
    if(displayBVH2D) {
        drawBVH2D(*this);
    }
    if(displayAABB) {
        for(Shape2D& shape : shapes) {
            drawOneAABB(*this, shape.aabb, Color(1.0, 0.0, 0.0));
        }
    }
    if(displayPoints) {
        for(Point2 point : points) {
            draw_circle(point, 10, Color(0.0, 1.0, 1.0), true);
        }
    }
    if(displayMidpoints) {
        for(Shape2D& shape : shapes) {
            draw_circle(shape.midPoint, 10, Color(1.0, 1.0, 0.0), true);
        }
        drawBVH2DMidpoints(*this);
    }
    if(displayRays) {
        for(RayVariant ray : allShotRays) {
            if(std::holds_alternative<Ray2D>(ray)) {
                drawRayMiss(*this, std::get<0>(ray));
            } else {
                drawRayHit(*this, std::get<1>(ray));
            }
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
                Ray2D startRay = std::get<0>(section.startRay);    
                Ray2D endRay = std::get<0>(section.endRay);  
                
                drawRayMiss(*this, startRay); 
                drawRayMiss(*this, endRay); 
                
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