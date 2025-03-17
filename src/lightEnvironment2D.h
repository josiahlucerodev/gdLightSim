#pragma once

//std
#include <optional>
#include <variant>
#include <algorithm>
#include <vector>
#include <unordered_map>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>

//own
#include "batchPool.h"
#include "aabb2D.h"
#include "shape2D.h"
#include "ray2D.h"
#include "bvh2D.h"
#include "section2D.h"
#include "lightCollider2D.h"

using namespace godot;

class LightEnvironment2D : public MeshInstance2D {
	GDCLASS(LightEnvironment2D, MeshInstance2D);
protected:
	static void _bind_methods();
	bool displayBVH2D;
	bool displayAABB;
    bool displayPoints;
    bool displayMidpoints;
    bool displayRays;
    bool displayFilledLight;
    bool displayLightColor;
    int64_t maxActions;
    
    bool displayRadialSections;
    double radialRaySpread;
    double radialSectionTolerance;
    
    bool displayLinearSections;
    double linearRaySpread;
    double linearSectionTolerance;

    bool displayScatterSections;
    double scatterRaySpread;
    double scatterSectionTolerance;
public:
	BVH2D bvh;
	std::vector<Shape2D> shapes;
	std::vector<Point2> points;
    std::vector<RayVariant> allShotRays;
    std::vector<RadialSection> radialSections;
    std::vector<LinearSection> linearSections;
    std::vector<ScatterSection> scatterSections;
    Ref<ArrayMesh> lightArrayMesh;
    std::unordered_map<ShapeId, LightColider2D*> shapeIdToLightColider;
public:

    LightEnvironment2D();
	~LightEnvironment2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;
    void _notification(int pwhat);

	bool get_display_bvh() const;
	void set_display_bvh(const bool);
	bool get_display_aabb() const;
    void set_display_aabb(const bool);
    bool get_display_points() const;
	void set_display_points(const bool);
    bool get_display_midpoints() const;
	void set_display_midpoints(const bool);
    bool get_display_rays() const;
	void set_display_rays(const bool);
    
    bool get_display_filled_light() const;
	void set_display_filled_light(const bool);
    bool get_display_light_color() const;
	void set_display_light_color(const bool);

    int64_t get_max_actions() const;
	void set_max_actions(const int64_t);

    bool get_display_radial_sections() const;
	void set_display_radial_sections(const bool);
    double get_radial_ray_spread() const;
	void set_radial_ray_spread(const double);
    double get_radial_section_tolerance() const;
	void set_radial_section_tolerance(const double);
    
    bool get_display_linear_sections() const;
	void set_display_linear_sections(const bool);
    double get_linear_ray_spread() const;
	void set_linear_ray_spread(const double);
    double get_linear_section_tolerance() const;
	void set_linear_section_tolerance(const double);

    bool get_display_scatter_sections() const;
	void set_display_scatter_sections(const bool);
    double get_scatter_ray_spread() const;
	void set_scatter_ray_spread(const double);
    double get_scatter_section_tolerance() const;
	void set_scatter_section_tolerance(const double);
};
