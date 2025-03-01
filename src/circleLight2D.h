#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"

class CircleLight2D : public Node2D {
	GDCLASS(CircleLight2D, Node2D);
private:
    int64_t rayCount;
	bool drawDebug;
protected:
	static void _bind_methods();
public:
    CircleLight2D();
	~CircleLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	int64_t get_ray_count() const;
	void set_ray_count(const int64_t);
	bool get_draw_debug() const;
	void set_draw_debug(const bool);
};

std::vector<RayVariant> shotCircleLight2D(
	const CircleLight2D& beamLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t radialRaySpread);
std::vector<RadialScanSection> generateCircleLight2DSections(
	const CircleLight2D& beamLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t radialSectionTolerance); 