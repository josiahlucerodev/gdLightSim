#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"
#include "shape2D.h"

//std
#include <deque>

using namespace godot;


class Mirror2D : public Node2D {
	GDCLASS(Mirror2D, Node2D);
private:
    double mirrorWidth;
    int64_t rayCount;
	int64_t maxBounce;
	bool drawDebug;
protected:
	static void _bind_methods();
public:
    Mirror2D();
	~Mirror2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

    double get_mirror_width() const;
	void set_mirror_width(const double);
	int64_t get_ray_count() const;
	void set_ray_count(const int64_t);
	int64_t get_max_bounce() const;
	void set_max_bounce(const int64_t);
	bool get_draw_debug() const;
	void set_draw_debug(const bool);
};

Shape2D constructShape2D(Mirror2D& mirror, ShapeId shapeId);

void addLinearMirrorBounceSectionsToQueue(
	const std::vector<Shape2D> shapes,
	std::deque<LinearScanSection>& lssQueue,
	const std::vector<LinearScanSection> linearScanSections, 
	std::size_t bounceLimit);

std::vector<RayVariant> shotMirrorBeam(
	const LinearScanSection& mirrorHitLinearScanSection,
	const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, 
	real_t linearRaySpread);

std::vector<LinearScanSection> generateMirrorBeamSections(
	const LinearScanSection& mirrorHitLinearScanSection, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance);
