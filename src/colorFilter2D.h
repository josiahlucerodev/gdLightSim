#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"

class ColorFilter2D : public Node2D {
	GDCLASS(ColorFilter2D, Node2D);
protected:
    static void _bind_methods();
private:
	bool drawDebug;
    real_t filterWidth;
	Color filterColor;
public:
    ColorFilter2D();
    ~ColorFilter2D();

    void _ready() override;
    void _process(double delta) override;
    void _draw() override;

    bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
    real_t get_filter_width() const;
	void set_filter_width(const real_t);
    Color get_filter_color() const;
	void set_filter_color(const Color filterColor);
};

Shape2D constructShape2D(ColorFilter2D& filter, ShapeId shapeId);

std::vector<RayVariant> shotFilterRadialSection(
	const RadialSection& filterRadialSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread);
std::vector<ScatterSection> generateFilterScatterSections(
	const RadialSection& filterRadialSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);
	
std::vector<RayVariant> shotFilterLinearSection(
	const LinearSection& filterLinearSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t linearRaySpread);
std::vector<LinearSection> generateFilterLinearSections(
	const LinearSection& filterLinearSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t linearSectionTolerance);
				
std::vector<RayVariant> shotFilterScatterSection(
	const ScatterSection& filterScatterSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread);
std::vector<ScatterSection> generateFilterScatterSections(
	const ScatterSection& filterScatterSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);

