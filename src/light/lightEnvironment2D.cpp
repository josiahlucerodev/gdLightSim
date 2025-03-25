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
using namespace godot;

//own
#include "emitter/spotLight2D.h"
#include "emitter/beamLight2D.h"
#include "emitter/circleLight2D.h"
#include "interactor/mirror2D.h"
#include "interactor/lens2D.h"
#include "interactor/colorFilter2D.h"
#include "interactor/lightCollider2D.h"
#include "interactor/lightSensor2D.h"
#include "ops/util.h"
#include "ops/shotLight.h"
#include "draw/ray.h"
#include "draw/section.h"
#include "draw/data.h"
#include "mesh/meshData.h"
#include "mesh/section.h"
#include "lightActor2D.h"
#include "lightActor2DOperations.h"
#include "settings.h"

//binding
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

//getters/setters
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
    return raySettings.radialRaySpread;
}
void LightEnvironment2D::set_radial_ray_spread(const double radialRaySpread) {
    this->raySettings.radialRaySpread = radialRaySpread;
}
double LightEnvironment2D::get_radial_section_tolerance() const {
    return raySettings.radialSectionTolerance;
}
void LightEnvironment2D::set_radial_section_tolerance(const double radialSectionTolerance) {
    this->raySettings.radialSectionTolerance = radialSectionTolerance;
}

bool LightEnvironment2D::get_display_linear_sections() const {
    return displayLinearSections;
}
void LightEnvironment2D::set_display_linear_sections(const bool displayLinearSections) {
    this->displayLinearSections = displayLinearSections;
}
double LightEnvironment2D::get_linear_ray_spread() const {
    return raySettings.linearRaySpread;
}
void LightEnvironment2D::set_linear_ray_spread(const double linearRaySpread) {
    this->raySettings.linearRaySpread = linearRaySpread;
}
double LightEnvironment2D::get_linear_section_tolerance() const {
    return raySettings.linearSectionTolerance;
}
void LightEnvironment2D::set_linear_section_tolerance(const double linearSectionTolerance) {
    this->raySettings.linearSectionTolerance = linearSectionTolerance;
}

bool LightEnvironment2D::get_display_scatter_sections() const {
    return displayScatterSections;
}
void LightEnvironment2D::set_display_scatter_sections(const bool displayScatterSections) {
    this->displayScatterSections = displayScatterSections;
}
double LightEnvironment2D::get_scatter_ray_spread() const {
    return raySettings.scatterRaySpread;
}
void LightEnvironment2D::set_scatter_ray_spread(const double scatterRaySpread) {
    this->raySettings.scatterRaySpread = scatterRaySpread;
}
double LightEnvironment2D::get_scatter_section_tolerance() const {
    return raySettings.scatterSectionTolerance;
}
void LightEnvironment2D::set_scatter_section_tolerance(const double scatterSectionTolerance) {
    this->raySettings.scatterSectionTolerance = scatterSectionTolerance;
}

//constructor/deconstructor
LightEnvironment2D::LightEnvironment2D() {
    displayAABB = true;
    displayBVH2D = true;
    displayPoints = true;
    displayMidpoints = true;
    displayRays = true;
    maxActions = 1000;
    displayFilledLight = true;
    displayLightColor = true;
    
    displayRadialSections = true;
    displayLinearSections = true;
    displayScatterSections = true;

    raySettings.radialRaySpread = 10;
    raySettings.radialSectionTolerance = 0.1;
    raySettings.linearRaySpread = 0.01;
    raySettings.linearSectionTolerance = 0.1;
    raySettings.scatterRaySpread = 0.001;
    raySettings.scatterSectionTolerance = 0.1;
}
LightEnvironment2D::~LightEnvironment2D() {}

//ops
void LightEnvironment2D::_ready() {
    sectionsInfo.reset();
    envData.reset();

    lightArrayMesh.instantiate();
    set_mesh(lightArrayMesh);
}

void LightEnvironment2D::_process(double delta) {
    LightEnvironment2D& env = *this; 
    sectionsInfo.reset();
    envData.reset();

    //creates an empty shape so that a shaped with shape id 0 can never be hit 
    //a shape id of 0 is used as the defualt shape id for a section that misses 
    envData.shapes.push_back(constructShape2D()); 

    std::vector<ColorFilter2D*> filters = getLightInteractor<ColorFilter2D>(env, "ColorFilter2D");
    constructLightInteractorShapes<ColorFilter2D>(envData.shapes, filters);
    std::vector<Lens2D*> lenses = getLightInteractor<Lens2D>(env, "Lens2D");
    constructLightInteractorShapes<Lens2D>(envData.shapes, lenses);
    std::vector<Mirror2D*> mirrors = getLightInteractor<Mirror2D>(env, "Mirror2D");
    constructLightInteractorShapes<Mirror2D>(envData.shapes, mirrors);
    
    ShapeId shapeId = envData.shapes.size();
    std::unordered_map<ShapeId, LightColider2D*> shapeIdToColider;
    std::vector<LightColider2D*> lightColiders = getLightInteractor<LightColider2D>(env, "LightColider2D");
    constructLightInteractorShapes<LightColider2D>(envData.shapes, lightColiders);
    {
        for(LightColider2D* lightColider : lightColiders) {
            shapeIdToColider.insert({shapeId, lightColider});
            shapeId++;
        }
    }
    
    shapeId = envData.shapes.size();
    std::unordered_map<LightSensor2D*, ShapeId> sensorsToShapeId;
    std::vector<LightSensor2D*> lightSensors = getLightInteractor<LightSensor2D>(env, "LightSensor2D");
    constructLightInteractorShapes<LightSensor2D>(envData.shapes, lightSensors);
    { 
        for(LightSensor2D* lightSensor : lightSensors) {
            sensorsToShapeId.insert({lightSensor, shapeId});
            shapeId++;
        }
    }
    
    constructBVH2D(envData.bvh, envData.shapes);
    envData.points = getPoints(envData.shapes);

    std::vector<SpotLight2D*> spotLights = getLightEmitter<SpotLight2D>(env, "SpotLight2D");
    std::vector<CircleLight2D*> circleLights = getLightEmitter<CircleLight2D>(env, "CircleLight2D");
    std::vector<BeamLight2D*> beamLights = getLightEmitter<BeamLight2D>(env, "BeamLight2D");

    for(SpotLight2D* spotLight : spotLights) {
        preformEmitterAction(envData, sectionsInfo, raySettings, *spotLight);
    }
    for(CircleLight2D* circleLight : circleLights) {
        preformEmitterAction(envData, sectionsInfo, raySettings, *circleLight);
    }
    for(BeamLight2D* beamLight : beamLights) {
        preformEmitterAction(envData, sectionsInfo, raySettings, *beamLight);
    }

    std::size_t actionCount = 0;
    while(!sectionsInfo.actionQueue.empty() && actionCount < maxActions) {
        SectionAction sectionAction = sectionsInfo.actionQueue.front();
        sectionsInfo.actionQueue.pop_front();
        
        switch(sectionAction.type) {
        case SectionActionType::mirror:
            preformInteractorAction<Mirror2D>(envData, sectionsInfo, raySettings, sectionAction.section);
            break;
        case SectionActionType::filter:
            preformInteractorAction<ColorFilter2D>(envData, sectionsInfo, raySettings, sectionAction.section);
            break;
        case SectionActionType::lens:
            preformInteractorAction<Lens2D>(envData, sectionsInfo, raySettings, sectionAction.section);
            break;
        default:
            break;
        }

        actionCount++;
	}

    //colider hit detection
    checkAndSetIfHit(sectionsInfo, shapeIdToColider);

    //sensor hit detection
    checkAndSetIfHit(envData.bvh, sensorsToShapeId);

    queue_redraw();
}

void LightEnvironment2D::_draw() {
    LightEnvironment2D& env = *this; 
    
    if(displayBVH2D) {
        drawBVH2D(env, envData.bvh);
    }
    if(displayAABB) {
        for(Shape2D& shape : envData.shapes) {
            drawAABB(env, shape.aabb, Settings::aabbColor);
        }
    }
    if(displayPoints) {
        for(Point2 point : envData.points) {
            drawShapePoint(env, point);
        }
    }
    if(displayMidpoints) {
        for(Shape2D& shape : envData.shapes) {
            drawMidPoint(env, shape.midPoint);
        }
        drawBVH2DMidpoints(env, envData.bvh);
    }
    if(displayRays) {
        for(RayVariant ray : envData.allShotRays) {
            drawRay(env, ray);
        }
    }
    if(displayRadialSections) {
        for(RadialSection& section : sectionsInfo.radialSections) {
            drawSection(env, section);
        }
    }
    if(displayLinearSections) {
        for(LinearSection& section : sectionsInfo.linearSections) {
            drawSection(env, section);
        }
    }
    if(displayScatterSections) {
        for(ScatterSection& section : sectionsInfo.scatterSections) {
            drawSection(env, section);
        }
    }
    
    if(!displayFilledLight || !env.sectionsInfo.hasSections()) {
        setEmptyMesh(env, env.lightArrayMesh);
    } else {    
        MeshData meshData;
        SectionDrawSettings drawSettings;
        drawSettings.drawColor = env.get_display_light_color();
    
        for(const RadialSection& section : env.sectionsInfo.radialSections) {
            generateMeshSection(meshData, drawSettings, section);
        }
        for(const LinearSection& section : env.sectionsInfo.linearSections) {
            generateMeshSection(meshData, drawSettings, section);
        }
        for(const ScatterSection& section : env.sectionsInfo.scatterSections) {
            generateMeshSection(meshData, drawSettings, section);
        }
        
        env.lightArrayMesh->reset_state();
        env.lightArrayMesh->add_surface_from_arrays(
            Mesh::PrimitiveType::PRIMITIVE_TRIANGLES,
            getSurfaceArrays(meshData)
        );
        env.set_mesh(env.lightArrayMesh);
    }
}