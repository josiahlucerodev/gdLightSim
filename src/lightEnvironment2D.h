#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>


//std
#include <optional>
#include <variant>
#include <algorithm>
#include <vector>

//own
#include "batchPool.h"
#include "aabb2D.h"
#include "shape2D.h"
#include "ray2D.h"
#include "bvh2D.h"
#include "section2D.h"

using namespace godot;

class LightEnvironment2D : public Node2D {
	GDCLASS(LightEnvironment2D, Node2D);
protected:
	static void _bind_methods();
	bool displayBVH2D;
	bool displayAABB;
    bool displayPoints;
    bool displayMidpoints;
    bool displayRays;
    bool displayRadialScanSections;
    bool displayLinearScanSections;
    double radialRaySpread;
    double radialSectionTolerance;
    double linearRaySpread;
    double linearSectionTolerance;
public:
	BVH2D bvh;
	std::vector<Shape2D> shapes;
	std::vector<Point2> points;
    std::vector<RayVariant> allShotRays;
    std::vector<RadialScanSection> radialScanSections;
    std::vector<LinearScanSection> linearScanSections;
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
    bool get_display_radial_scan_sections() const;
	void set_display_radial_scan_sections(const bool);
    bool get_display_linear_scan_sections() const;
	void set_display_linear_scan_sections(const bool);

    double get_radial_ray_spread() const;
	void set_radial_ray_spread(const double);
    double get_radial_section_tolerance() const;
	void set_radial_section_tolerance(const double);
    double get_linear_ray_spread() const;
	void set_linear_ray_spread(const double);
    double get_linear_section_tolerance() const;
	void set_linear_section_tolerance(const double);
};
