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
	bool drawDebug;
    real_t mirrorWidth;
protected:
	static void _bind_methods();
public:
    Mirror2D();
	~Mirror2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool);
    real_t get_mirror_width() const;
	void set_mirror_width(const real_t);
};

Shape2D constructShape2D(Mirror2D& mirror, ShapeId shapeId);

std::vector<RayVariant> shotMirrorLinearSection(
	const LinearSection& mirrorLinearSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t linearRaySpread);
std::vector<LinearSection> generateMirrorLinearSections(
	const LinearSection& mirrorLinearSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, real_t linearSectionTolerance);
				
std::vector<RayVariant> shotMirrorRadialSection(
	const RadialSection& mirrorRadialSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread);
std::vector<ScatterSection> generateMirrorScatterSections(
	const RadialSection& mirrorRadialSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance);
	
std::vector<RayVariant> shotMirrorScatterSection(
	const ScatterSection& mirrorScatterSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread);
std::vector<ScatterSection> generateMirrorScatterSections(
	const ScatterSection& mirrorScatterSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance);
