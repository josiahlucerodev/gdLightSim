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
	bool drawDebug;
	Color color;
protected:
	static void _bind_methods();
public:
    CircleLight2D();
	~CircleLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
	Color get_color() const;
	void set_color(const Color color);
};


struct ShotCircleLight2DInfo {
	std::vector<RayVariant> rays;
	std::vector<RadialSection> sections;
};

ShotCircleLight2DInfo shotCircleLight2D(
	const CircleLight2D& circleLight,  const std::vector<Point2>& points, 
	BVH2D& bvh, const std::vector<Shape2D>& shapes,
	const real_t& radialRaySpread, const real_t& radialSectionTolerance);