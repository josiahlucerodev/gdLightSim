#include "lightEnvironment2D.h"

//std
#include <deque>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/packed_data_container.hpp>

//own
#include "spotLight2D.h"
#include "beamLight2D.h"
#include "circleLight2D.h"
#include "mirror2D.h"
#include "colorFilter2D.h"
#include "lens2D.h"
#include "util.h"
#include "settings.h"
#include "shotLight.h"

using namespace godot;

void LightEnvironment2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_display_bvh"), &LightEnvironment2D::get_display_bvh);
	ClassDB::bind_method(D_METHOD("set_display_bvh", "display_bvh"), &LightEnvironment2D::set_display_bvh);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayBVH2D"), "set_display_bvh", "get_display_bvh");

    ClassDB::bind_method(D_METHOD("get_display_aabb"), &LightEnvironment2D::get_display_aabb);
	ClassDB::bind_method(D_METHOD("set_display_aabb", "display_aabb"), &LightEnvironment2D::set_display_aabb);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayAABB"), "set_display_aabb", "get_display_aabb");

    ClassDB::bind_method(D_METHOD("get_display_points"), &LightEnvironment2D::get_display_points);
	ClassDB::bind_method(D_METHOD("set_display_points", "display_aabb"), &LightEnvironment2D::set_display_points);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayPoints"), "set_display_points", "get_display_points");

    ClassDB::bind_method(D_METHOD("get_display_midpoints"), &LightEnvironment2D::get_display_midpoints);
	ClassDB::bind_method(D_METHOD("set_display_midpoints", "display_aabb"), &LightEnvironment2D::set_display_midpoints);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayMidpoints"), "set_display_midpoints", "get_display_midpoints");

    ClassDB::bind_method(D_METHOD("get_display_rays"), &LightEnvironment2D::get_display_rays);
	ClassDB::bind_method(D_METHOD("set_display_rays", "display_aabb"), &LightEnvironment2D::set_display_rays);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayRays"), "set_display_rays", "get_display_rays");

    ClassDB::bind_method(D_METHOD("get_display_filled_light"), &LightEnvironment2D::get_display_filled_light);
	ClassDB::bind_method(D_METHOD("set_display_filled_light", "display_filled_light"), &LightEnvironment2D::set_display_filled_light);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayFilledLight"), "set_display_filled_light", "get_display_filled_light");

    ClassDB::bind_method(D_METHOD("get_display_light_color"), &LightEnvironment2D::get_display_light_color);
	ClassDB::bind_method(D_METHOD("set_display_light_color", "display_light_color"), &LightEnvironment2D::set_display_light_color);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayLightColor"), "set_display_light_color", "get_display_light_color");

    ClassDB::bind_method(D_METHOD("get_max_actions"), &LightEnvironment2D::get_max_actions);
	ClassDB::bind_method(D_METHOD("set_max_actions", "max_actions"), &LightEnvironment2D::set_max_actions);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_actions", PROPERTY_HINT_RANGE, "0,10000, 1"), "set_max_actions", "get_max_actions");

    //radial
    ClassDB::bind_method(D_METHOD("get_display_radial_sections"), &LightEnvironment2D::get_display_radial_sections);
	ClassDB::bind_method(D_METHOD("set_display_radial_sections", "display_rs"), &LightEnvironment2D::set_display_radial_sections);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayRadialSections"), "set_display_radial_sections", "get_display_radial_sections");
    
    ClassDB::bind_method(D_METHOD("get_radial_ray_spread"), &LightEnvironment2D::get_radial_ray_spread);
	ClassDB::bind_method(D_METHOD("set_radial_ray_spread", "radial_ray_spread"), &LightEnvironment2D::set_radial_ray_spread);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radialRaySpread", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_radial_ray_spread", "get_radial_ray_spread");
    
    ClassDB::bind_method(D_METHOD("get_radial_section_tolerance"), &LightEnvironment2D::get_radial_section_tolerance);
	ClassDB::bind_method(D_METHOD("set_radial_section_tolerance", "tolerance"), &LightEnvironment2D::set_radial_section_tolerance);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radialSectionTolerance", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_radial_section_tolerance", "get_radial_section_tolerance");

    //linear
    ClassDB::bind_method(D_METHOD("get_display_linear_sections"), &LightEnvironment2D::get_display_linear_sections);
	ClassDB::bind_method(D_METHOD("set_display_linear_sections", "display_ls"), &LightEnvironment2D::set_display_linear_sections);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayLinearSections"), "set_display_linear_sections", "get_display_linear_sections");
    
    ClassDB::bind_method(D_METHOD("get_linear_ray_spread"), &LightEnvironment2D::get_linear_ray_spread);
	ClassDB::bind_method(D_METHOD("set_linear_ray_spread", "linear_ray_spread"), &LightEnvironment2D::set_linear_ray_spread);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linearRaySpread", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_linear_ray_spread", "get_linear_ray_spread");
    
    ClassDB::bind_method(D_METHOD("get_linear_section_tolerance"), &LightEnvironment2D::get_linear_section_tolerance);
	ClassDB::bind_method(D_METHOD("set_linear_section_tolerance", "tolerance"), &LightEnvironment2D::set_linear_section_tolerance);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linearSectionTolerance", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_linear_section_tolerance", "get_linear_section_tolerance");

    //scatter
    ClassDB::bind_method(D_METHOD("get_display_scatter_sections"), &LightEnvironment2D::get_display_scatter_sections);
	ClassDB::bind_method(D_METHOD("set_display_scatter_sections", "display_ss"), &LightEnvironment2D::set_display_scatter_sections);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "displayScatterSections"), "set_display_scatter_sections", "get_display_scatter_sections");
    
    ClassDB::bind_method(D_METHOD("get_scatter_ray_spread"), &LightEnvironment2D::get_scatter_ray_spread);
	ClassDB::bind_method(D_METHOD("set_scatter_ray_spread", "scatter_ray_spread"), &LightEnvironment2D::set_scatter_ray_spread);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scatterRaySpread", PROPERTY_HINT_RANGE, "0.0001, 0.001, 0.0001"), "set_scatter_ray_spread", "get_scatter_ray_spread");
    
    ClassDB::bind_method(D_METHOD("get_scatter_section_tolerance"), &LightEnvironment2D::get_scatter_section_tolerance);
	ClassDB::bind_method(D_METHOD("set_scatter_section_tolerance", "tolerance"), &LightEnvironment2D::set_scatter_section_tolerance);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scatterSectionTolerance", PROPERTY_HINT_RANGE, "0.001, 0.5, 0.001"), "set_scatter_section_tolerance", "get_scatter_section_tolerance");
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

bool LightEnvironment2D::get_display_filled_light() const {
    return displayFilledLight;
}
void LightEnvironment2D::set_display_filled_light(const bool displayFilledLight) {
    this->displayFilledLight = displayFilledLight;
}
bool LightEnvironment2D::get_display_light_color() const {
    return displayLightColor;
}
void LightEnvironment2D::set_display_light_color(const bool displayLightColor) {
    this->displayLightColor = displayLightColor;
}

int64_t LightEnvironment2D::get_max_actions() const {
    return maxActions;
}
void LightEnvironment2D::set_max_actions(const int64_t maxActions) {
    this->maxActions = maxActions;
}

bool LightEnvironment2D::get_display_radial_sections() const {
    return displayRadialSections;
}
void LightEnvironment2D::set_display_radial_sections(const bool displayRadialSections) {
    this->displayRadialSections = displayRadialSections;
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

bool LightEnvironment2D::get_display_linear_sections() const {
    return displayLinearSections;
}
void LightEnvironment2D::set_display_linear_sections(const bool displayLinearSections) {
    this->displayLinearSections = displayLinearSections;
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

bool LightEnvironment2D::get_display_scatter_sections() const {
    return displayScatterSections;
}
void LightEnvironment2D::set_display_scatter_sections(const bool displayScatterSections) {
    this->displayScatterSections = displayScatterSections;
}
double LightEnvironment2D::get_scatter_ray_spread() const {
    return scatterRaySpread;
}
void LightEnvironment2D::set_scatter_ray_spread(const double scatterRaySpread) {
    this->scatterRaySpread = scatterRaySpread;
}
double LightEnvironment2D::get_scatter_section_tolerance() const {
    return scatterSectionTolerance;
}
void LightEnvironment2D::set_scatter_section_tolerance(const double scatterSectionTolerance) {
    this->scatterSectionTolerance = scatterSectionTolerance;
}

LightEnvironment2D::LightEnvironment2D() {}
LightEnvironment2D::~LightEnvironment2D() {}

void LightEnvironment2D::_ready() {
    displayAABB = true;
    displayBVH2D = true;
    displayPoints = true;
    displayMidpoints = true;
    displayRays = true;
    maxActions = 1000;
    displayFilledLight = true;
    displayLightColor = true;
    
    displayRadialSections = true;
    radialRaySpread = 10;
    radialSectionTolerance = 0.1;
    
    displayLinearSections = true;
    linearRaySpread = 0.01;
    linearSectionTolerance = 0.1;

    displayScatterSections = true;
    scatterRaySpread = 0.001;
    scatterSectionTolerance = 0.1;

    shapes.clear();
    points.clear();
    radialSections.clear();
    resetBVH2D(bvh);

    lightArrayMesh.instantiate();
    set_mesh(lightArrayMesh);
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

/*
void preformAdditionalLinearSectionOperations(
    std::vector<RayVariant>& allShotRaysDst, 
    std::vector<LinearSection>& linearSectionsDst, std::vector<ScatterSection>& scatterSectionsDst, 
    const std::vector<Shape2D>& shapes, const BVH2D& bvh,
    const real_t linearRaySpread, const real_t linearSectionTolerance, 
    const real_t scatterRaySpread, const real_t scatterSectionTolerance,
    const std::vector<LinearSection>& newLinearSections) {
        
    const std::vector<LinearSection> filterLinearSections = getFilterLinearSections(shapes, newLinearSections);
    for(const LinearSection& filterLinearSection : filterLinearSections) {
        std::vector<RayVariant> rays = shotFilterLinearSection(filterLinearSection, shapes, bvh,  linearRaySpread); 
        allShotRaysDst.insert(allShotRaysDst.end(), rays.begin(), rays.end());
        std::vector<LinearSection> sections = generateFilterLinearSections(filterLinearSection, rays, shapes, linearSectionTolerance);
        linearSectionsDst.insert(linearSectionsDst.end(), sections.begin(), sections.end());
        
        preformAdditionalLinearSectionOperations(allShotRaysDst, linearSectionsDst, scatterSectionsDst, shapes, bvh,
        linearRaySpread, linearSectionTolerance, scatterRaySpread, scatterSectionTolerance, sections
    );
}
}
*/

void updateColliderHit(std::unordered_map<ShapeId, LightColider2D*>& shapeIdToLightColider, ShapeId shapeId) {
    auto findIter = shapeIdToLightColider.find(shapeId);
    if(findIter == shapeIdToLightColider.end()) {
        return;
    }
    findIter->second->set_is_hit(true);
}

void LightEnvironment2D::_process(double delta) {
    shapes.clear();
    points.clear();
    allShotRays.clear();
    radialSections.clear();
    linearSections.clear();
    scatterSections.clear();
    shapeIdToLightColider.clear();
    resetBVH2D(bvh);

    std::vector<LightColider2D*> lightColiders = getChildrenOfType<LightColider2D>(*this, "LightColider2D");
    std::vector<Mirror2D*> mirrors = getChildrenOfType<Mirror2D>(*this, "Mirror2D");
    std::vector<ColorFilter2D*> filters = getChildrenOfType<ColorFilter2D>(*this, "ColorFilter2D");
    std::vector<Lens2D*> lenses = getChildrenOfType<Lens2D>(*this, "Lens2D");

    if(mirrors.empty() && lightColiders.empty() && filters.empty() && lenses.empty()) {
        queue_redraw();
        return;
    }

    for(LightColider2D* lightColider : lightColiders) {
        Shape2D shape = constructShape2D(*lightColider, shapes.size());
        shapes.push_back(shape);
        shapeIdToLightColider.insert({shape.shapeId, lightColider});
        lightColider->set_is_hit(false);
    }

    for(Mirror2D* mirror : mirrors) {
        Shape2D shape = constructShape2D(*mirror, shapes.size());
        shapes.push_back(shape);
    }

    for(ColorFilter2D* filter : filters) {
        Shape2D shape = constructShape2D(*filter, shapes.size());
        shapes.push_back(shape);
    }

    for(Lens2D* lens : lenses) {
        Shape2D shape = constructShape2D(*lens, shapes.size());
        shapes.push_back(shape);
    }

    constructBVH2D(bvh, shapes);
    points = getPoints(shapes);

    std::vector<SpotLight2D*> spotLights = getChildrenOfType<SpotLight2D>(*this, "SpotLight2D");
    for(SpotLight2D* spotLight : spotLights) {
        std::vector<RayVariant> rays = shotSpotLight2D(*spotLight, points, bvh,  radialRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<RadialSection> sections = generateSpotLight2DSections(*spotLight, rays, shapes, radialSectionTolerance);
        radialSections.insert(radialSections.end(), sections.begin(), sections.end());
    }
    
    std::vector<CircleLight2D*> circleLights = getChildrenOfType<CircleLight2D>(*this, "CircleLight2D");
    for(CircleLight2D* circleLight : circleLights) {
        ShotCircleLight2DInfo shotInfo = shotCircleLight2D(*circleLight, points, bvh, shapes, radialRaySpread, radialSectionTolerance); 
        allShotRays.insert(allShotRays.end(), shotInfo.rays.begin(), shotInfo.rays.end());
        radialSections.insert(radialSections.end(), shotInfo.sections.begin(), shotInfo.sections.end());
    }
    
    std::vector<BeamLight2D*> beamLights = getChildrenOfType<BeamLight2D>(*this, "BeamLight2D");
    for(BeamLight2D* beamLight : beamLights) {
        std::vector<RayVariant> rays = shotBeamLight2D(*beamLight, points, bvh,  linearRaySpread); 
        allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
        std::vector<LinearSection> sections = generateBeamLight2DSections(*beamLight, rays, shapes, linearSectionTolerance);
        linearSections.insert(linearSections.end(), sections.begin(), sections.end());
    }

    SectionActionQueue actionQueue;
    addRadialSectionActionsToQueue(actionQueue, shapes, radialSections);
    addLinearSectionActionsToQueue(actionQueue, shapes, linearSections);
    addScatterSectionActionsToQueue(actionQueue, shapes, scatterSections);

    std::size_t actionCount = 0;
    while(!actionQueue.empty() && actionCount < 1000) {
        SectionAction sessionAction = actionQueue.front();
        actionQueue.pop_front();
        
        switch(sessionAction.type) {
        case SectionActionType::mirror:
            std::visit([&](auto&& session) -> void {
                using session_type = std::decay_t<decltype(session)>;
                if constexpr(std::is_same_v<session_type, RadialSection>) {
                    std::vector<RayVariant> rays = shotMirrorRadialSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateMirrorScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, LinearSection>) {
                    std::vector<RayVariant> rays = shotMirrorLinearSection(session, shapes, bvh,  linearRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<LinearSection> sections = generateMirrorLinearSections(session, rays, shapes, linearSectionTolerance);
                    linearSections.insert(linearSections.end(), sections.begin(), sections.end());
                    addLinearSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, ScatterSection>) {
                    std::vector<RayVariant> rays = shotMirrorScatterSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateMirrorScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
            }, sessionAction.section);
            break;
        case SectionActionType::filter:
            std::visit([&](auto&& session) -> void {
                using session_type = std::decay_t<decltype(session)>;
                if constexpr(std::is_same_v<session_type, RadialSection>) {
                    std::vector<RayVariant> rays = shotFilterRadialSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateFilterScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, LinearSection>) {
                    std::vector<RayVariant> rays = shotFilterLinearSection(session, shapes, bvh,  linearRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<LinearSection> sections = generateFilterLinearSections(session, rays, shapes, linearSectionTolerance);
                    linearSections.insert(linearSections.end(), sections.begin(), sections.end());
                    addLinearSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, ScatterSection>) {
                    std::vector<RayVariant> rays = shotFilterScatterSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateFilterScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
            }, sessionAction.section);
            break;
        case SectionActionType::lens:
        std::visit([&](auto&& session) -> void {
            using session_type = std::decay_t<decltype(session)>;
            if constexpr(std::is_same_v<session_type, RadialSection>) {
                    std::vector<RayVariant> rays = shotLensRadialSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateLensScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, LinearSection>) {
                    std::vector<RayVariant> rays = shotLensLinearSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateLensScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
                if constexpr(std::is_same_v<session_type, ScatterSection>) {
                    std::vector<RayVariant> rays = shotLensScatterSection(session, shapes, bvh,  scatterRaySpread); 
                    allShotRays.insert(allShotRays.end(), rays.begin(), rays.end());
                    std::vector<ScatterSection> sections = generateLensScatterSections(session, rays, shapes, scatterSectionTolerance);
                    scatterSections.insert(scatterSections.end(), sections.begin(), sections.end());
                    addScatterSectionActionsToQueue(actionQueue, shapes, sections);
                }
            }, sessionAction.section);
            break;
        default:
            break;
        }

        actionCount++;
	}

    //check hit 
    for(const LinearSection& linearSection : linearSections) {
        updateColliderHit(shapeIdToLightColider, linearSection.shapeId);
    }
    for(const RadialSection& radialSection : radialSections) {
        updateColliderHit(shapeIdToLightColider, radialSection.shapeId);
    }
    for(const ScatterSection& scatterSection : scatterSections) {
        updateColliderHit(shapeIdToLightColider, scatterSection.shapeId);
    }

    queue_redraw();
}

Rect2 createRect(AABB2D& aabb) {
    return Rect2(aabb.min, aabb.max - aabb.min);
}

void drawOneAABB(LightEnvironment2D& env, AABB2D aabb, Color color) {
    env.draw_rect(createRect(aabb), color, false, Settings::debugLineWidth);
    env.draw_circle(aabb.min, Settings::pointRadius, color);
    env.draw_circle(aabb.max, Settings::pointRadius, color);
} 

void drawBVH2DNode(LightEnvironment2D& env, BVH2D::Node* node) {
    if(node == nullptr) {
        return;
    }
    drawOneAABB(env, node->aabb, Settings::bvhColor);
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
    env.draw_circle(node->midPoint, Settings::pointRadius, Settings::midPointColor);
    drawBVH2DNodeMidpoints(env, node->b1);
    drawBVH2DNodeMidpoints(env, node->b2);
}
void drawBVH2DMidpoints(LightEnvironment2D& env) {
    drawBVH2DNodeMidpoints(env, env.bvh.root);
}

void drawRayMiss(LightEnvironment2D& env, const Ray2D& ray) {
    env.draw_line(ray.origin, ray.origin + (ray.direction * Settings::debugDistance), Settings::rayMissColor, Settings::debugLineWidth);
}

void drawRayHit(LightEnvironment2D& env, const RayHit2D& rayHit) {
    env.draw_line(rayHit.ray.origin, rayHit.location, Settings::rayHitColor, Settings::debugLineWidth);
    env.draw_circle(rayHit.location, Settings::pointRadius, Settings::rayHitColor);
}

void drawRayMissSegment(LightEnvironment2D& env, const Ray2D& beginRay, const Ray2D& endRay) {
    drawRayMiss(env, beginRay);
    drawRayMiss(env, endRay);
    for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
        real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
        env.draw_line(
            (beginRay.direction * localDistance) + beginRay.origin, 
            (endRay.direction * localDistance) + endRay.origin, 
            Settings::rayMissColor, Settings::debugLineWidth);   
    }
}

void generateQuad(
    PackedVector3Array& vertices, PackedVector2Array& uvs, PackedColorArray& colors,
    const Color& color, const Point2 p1, const Point2 p2, const Point2 p3, const Point2 p4) {

    vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    vertices.push_back(Vector3(p2.x, p2.y, 0)); //p2
    vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3

    vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3
    vertices.push_back(Vector3(p4.x, p4.y, 0)); //p4

    uvs.push_back(Vector2(0, 0)); //p1
    uvs.push_back(Vector2(1, 0)); //p2
    uvs.push_back(Vector2(1, 1)); //p3

    uvs.push_back(Vector2(0, 0)); //p1
    uvs.push_back(Vector2(1, 1)); //p3
    uvs.push_back(Vector2(0, 1)); //p4

    colors.push_back(color); //p1
    colors.push_back(color); //p2
    colors.push_back(color); //p3

    colors.push_back(color); //p1
    colors.push_back(color); //p3
    colors.push_back(color); //p4
}

void generateTriangle(
    PackedVector3Array& vertices, PackedVector2Array& uvs, PackedColorArray& colors,
    const Color& color, const Point2 p1, const Point2 p2, const Point2 p3) {

    vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    vertices.push_back(Vector3(p2.x, p2.y, 0)); //p2
    vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3

    uvs.push_back(Vector2(0, 0)); //p1
    uvs.push_back(Vector2(1, 0)); //p2
    uvs.push_back(Vector2(1, 1)); //p3

    colors.push_back(color); //p1
    colors.push_back(color); //p2
    colors.push_back(color); //p3
}

void generateEmptyLightMesh(LightEnvironment2D& env) {
    env.lightArrayMesh->reset_state();
    env.set_mesh(nullptr);
}
void generateLightMesh(LightEnvironment2D& env) {
    if(env.radialSections.empty() && env.linearSections.empty() && env.scatterSections.empty()) {
        generateEmptyLightMesh(env);
        return;
    }


    auto pickColor = [&](const Color& color) -> Color {
        return env.get_display_light_color() ? 
            Color(color.r, color.g, color.b, 0.8) :
            Color(Settings::debugFilledLightColor.r, Settings::debugFilledLightColor.g, Settings::debugFilledLightColor.b, 0.8); 
    };

    PackedVector3Array vertices;
    PackedVector2Array uvs;
    PackedColorArray colors;

    for(const RadialSection& section : env.radialSections) {
        switch (section.type) {
        case SectionType::hit: {
            const RayHit2D& startHit = std::get<1>(section.startRay);    
            const RayHit2D& endHit = std::get<1>(section.endRay); 
            
            generateTriangle(vertices, uvs, colors, pickColor(section.color),
                startHit.ray.origin, startHit.location, endHit.location);
            break;
        }
        case SectionType::miss: {
            const std::vector<Ray2D> rays = generateMissSectionRays(section);

            for(std::size_t i = 0; i < rays.size() - 1; i++) {
                Ray2D ray1 = rays[i];
                Ray2D ray2 = rays[i + 1];
                Point2 dis1 = ray1.origin + (ray1.direction * Settings::debugDistance);
                Point2 dis2 = ray2.origin + (ray2.direction * Settings::debugDistance);
                generateTriangle(vertices, uvs, colors, pickColor(section.color), ray1.origin, dis1, dis2);
            }
            break;
        }
        default:
            break;
        }
    }

    for(const LinearSection& section : env.linearSections) {
        switch (section.type) {
        case SectionType::hit: {
            const RayHit2D& startHit = std::get<1>(section.startRay);    
            const RayHit2D& endHit = std::get<1>(section.endRay);    
            generateQuad(vertices, uvs, colors, pickColor(section.color),
                startHit.ray.origin, startHit.location, 
                endHit.location, endHit.ray.origin);
            break;
        }
        case SectionType::miss: {
            const Ray2D& startRay = std::get<0>(section.startRay);    
            const Ray2D& endRay = std::get<0>(section.endRay);    
            Point2 startDis = startRay.origin + (startRay.direction * Settings::debugDistance);
            Point2 endDis = endRay.origin + (endRay.direction * Settings::debugDistance);
            generateQuad(vertices, uvs, colors, pickColor(section.color),
                startRay.origin, startDis, 
                endDis, endRay.origin);
            break;
        }
        default:
            break;
        }
    }

    for(const ScatterSection& section : env.scatterSections) {
        switch (section.type) {
        case SectionType::hit: {
            const RayHit2D& startHit = std::get<1>(section.startRay);    
            const RayHit2D& endHit = std::get<1>(section.endRay);    
            generateQuad(vertices, uvs, colors, pickColor(section.color),
                startHit.ray.origin, startHit.location, 
                endHit.location, endHit.ray.origin);
            break;
        }
        case SectionType::miss: {
            const Ray2D& startRay = std::get<0>(section.startRay);    
            const Ray2D& endRay = std::get<0>(section.endRay);    
            Point2 startDis = startRay.origin + (startRay.direction * Settings::debugDistance);
            Point2 endDis = endRay.origin + (endRay.direction * Settings::debugDistance);
            generateQuad(vertices, uvs, colors, pickColor(section.color),
                startRay.origin, startDis, 
                endDis, endRay.origin);
            break;
        }
        default:
            break;
        }
    }

    Array surfaceArrays;
    surfaceArrays.resize(ArrayMesh::ARRAY_MAX);
    surfaceArrays[ArrayMesh::ARRAY_VERTEX] = vertices;
    surfaceArrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    surfaceArrays[ArrayMesh::ARRAY_COLOR] = colors;
    
    env.lightArrayMesh->reset_state();
    env.lightArrayMesh->add_surface_from_arrays(
        Mesh::PrimitiveType::PRIMITIVE_TRIANGLES,
        surfaceArrays
    );
    env.set_mesh(env.lightArrayMesh);
}

void LightEnvironment2D::_draw() {
    if(displayBVH2D) {
        drawBVH2D(*this);
    }
    if(displayAABB) {
        for(Shape2D& shape : shapes) {
            drawOneAABB(*this, shape.aabb, Settings::aabbColor);
        }
    }
    if(displayPoints) {
        for(Point2 point : points) {
            draw_circle(point, Settings::pointRadius, Settings::shapePointColor);
        }
    }
    if(displayMidpoints) {
        for(Shape2D& shape : shapes) {
            draw_circle(shape.midPoint, Settings::pointRadius, Settings::midPointColor);
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
    if(displayRadialSections) {
        for(RadialSection& section : radialSections) {
            switch (section.type) {
            case SectionType::hit: {
                RayHit2D& startHit = std::get<1>(section.startRay);    
                RayHit2D& endHit = std::get<1>(section.endRay);    
                
                drawRayHit(*this, startHit); 
                drawRayHit(*this, endHit); 
                draw_line(startHit.location, endHit.location, Settings::rayHitColor);
                break;
            }
            case SectionType::miss: {
                const std::vector<Ray2D> rays = generateMissSectionRays(section);
                for(std::size_t i = 0; i < rays.size() - 1; i++) {
                    Ray2D ray1 = rays[i];
                    Ray2D ray2 = rays[i + 1];
                    drawRayMissSegment(*this, ray1, ray2);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    if(displayLinearSections) {
        for(LinearSection& section : linearSections) {
            switch (section.type) {
            case SectionType::hit: {
                RayHit2D& startHit = std::get<1>(section.startRay);    
                RayHit2D& endHit = std::get<1>(section.endRay);    
                drawRayHit(*this, startHit); 
                drawRayHit(*this, endHit);                 
                draw_line(startHit.location, endHit.location, Settings::rayHitColor);
                break;
            }
            case SectionType::miss: {
                Ray2D& startRay = std::get<0>(section.startRay);    
                Ray2D& endRay = std::get<0>(section.endRay);    
                drawRayMissSegment(*this, startRay, endRay);
                break;
            }
            default:
                break;
            }
        }
    }
    if(displayScatterSections) {
        for(ScatterSection& section : scatterSections) {
            switch (section.type) {
            case SectionType::hit: {
                RayHit2D& startHit = std::get<1>(section.startRay);    
                RayHit2D& endHit = std::get<1>(section.endRay);    
                drawRayHit(*this, startHit); 
                drawRayHit(*this, endHit);                 
                draw_line(startHit.location, endHit.location, Settings::rayHitColor);
                break;
            }
            case SectionType::miss: {
                Ray2D& startRay = std::get<0>(section.startRay);    
                Ray2D& endRay = std::get<0>(section.endRay);    
                drawRayMissSegment(*this, startRay, endRay);
                break;
            }
            default:
                break;
            }
        }
    }
    
    if(displayFilledLight) {
        generateLightMesh(*this);
    } else {
        generateEmptyLightMesh(*this);
    }
}