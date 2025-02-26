#pragma once

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"

using namespace godot;

class BeamLight2D : public Node2D {
	GDCLASS(BeamLight2D, Node2D);
private:
    double lightWidth;
	int64_t rayCount;
	bool drawDebug;
protected:
	static void _bind_methods();

public:
    BeamLight2D();
	~BeamLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	double get_light_width() const;
	void set_light_width(const double);
	int64_t get_ray_count() const;
	void set_ray_count(const int64_t);
	bool get_draw_debug() const;
	void set_draw_debug(const bool);
};

std::vector<RayVariant> shotBeamLight2D(
	BeamLight2D& beamLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t linearRaySpread);
std::vector<LinearScanSection> generateBeamLight2DSections(
	BeamLight2D& beamLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance); 