#include "colorFilter2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "util.h"
#include "settings.h"
#include "shotLight.h"

void ColorFilter2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &ColorFilter2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &ColorFilter2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
	
	ClassDB::bind_method(D_METHOD("get_filter_width"), &ColorFilter2D::get_filter_width);
	ClassDB::bind_method(D_METHOD("set_filter_width", "filter_width"), &ColorFilter2D::set_filter_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_filter_width", "get_filter_width");
	
	ClassDB::bind_method(D_METHOD("get_filter_color"), &ColorFilter2D::get_filter_color);
	ClassDB::bind_method(D_METHOD("set_filter_color", "filter_color"), &ColorFilter2D::set_filter_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "filter_color"), "set_filter_color", "get_filter_color");
}

bool ColorFilter2D::get_draw_debug() const {
	return drawDebug;
}
void ColorFilter2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t ColorFilter2D::get_filter_width() const {
	return filterWidth;
}
void ColorFilter2D::set_filter_width(const real_t filterWidth) {
	this->filterWidth = filterWidth;
}

Color ColorFilter2D::get_filter_color() const {
	return filterColor;
}
void ColorFilter2D::set_filter_color(const Color color) {
	this->filterColor = color;
}

ColorFilter2D::ColorFilter2D() {
	drawDebug = false;
	filterColor = Settings::defaultLightColor;
	filterWidth = 100;
}

ColorFilter2D::~ColorFilter2D() {
}

void ColorFilter2D::_ready() {
	queue_redraw();
}

void ColorFilter2D::_process(double delta) {
	queue_redraw();
}

void ColorFilter2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (filterWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (filterWidth / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}

Shape2D constructShape2D(ColorFilter2D& filter, ShapeId shapeId) {
	Shape2D shape;
    shape.type = Shape2DType::filter;
    shape.shapeId = shapeId;
	shape.filterColor = filter.get_filter_color();
    
    real_t filterWidth = filter.get_filter_width();
    Vector2 position = filter.get_transform().get_origin();

    real_t rotation = filter.get_transform().get_rotation() + (Math_PI / 2);
    Point2 dir = vectorFromAngle(rotation);
    Point2 rightPoint = position + (dir * (filterWidth / 2));
    Point2 leftPoint = position + ((dir * (-1)) * (filterWidth / 2));
    
    shape.aabb = AABB2D{
        Point2{rightPoint.x, rightPoint.y},
        Point2{rightPoint.x, rightPoint.y}
    };
    shape.aabb = updateAABB(shape.aabb, {Point2(leftPoint.x, leftPoint.y), Point2(leftPoint.x, leftPoint.y)});
    
    shape.midPoint = (leftPoint + rightPoint) / 2;
    shape.points.reserve(2);
    shape.points.push_back(rightPoint);
    shape.points.push_back(leftPoint);

    return shape;
}

Color applyFilter(const Color& input, const Color& filter) {
    return Color(
        std::clamp(input.r * filter.r, 0.0f, 1.0f),
        std::clamp(input.g * filter.g, 0.0f, 1.0f),
        std::clamp(input.b * filter.b, 0.0f, 1.0f),
		1.0
    );
}

//linear
std::vector<RayVariant> shotFilterLinearSection(
	const LinearSection& filterLinearSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t linearRaySpread) {

	const RayHit2D& startRay = std::get<1>(filterLinearSection.startRay);
	const RayHit2D& endRay = std::get<1>(filterLinearSection.endRay);
	const ShapeId originShapeId = filterLinearSection.shapeId;

	return shotLinearLight(originShapeId, startRay.location, endRay.location, startRay.ray.direction, 
		getPointsExcluding(originShapeId, shapes), bvh, linearRaySpread);
}
std::vector<LinearSection> generateFilterLinearSections(
	const LinearSection& filterLinearSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t linearSectionTolerance) {

	const ShapeId originShapeId = filterLinearSection.shapeId;
	const Color& filterColor = shapes[originShapeId].filterColor;
	const Color& color = applyFilter(filterLinearSection.color, filterColor);
	const RayHit2D& startRay = std::get<1>(filterLinearSection.startRay);

	return generateLinearSections(color, startRay.location, rays, shapes, linearSectionTolerance);
}

//radial
ShotScatterReturn shotFilterRadialSection(
	const RadialSection& filterRadialSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread) {
	
	const RayHit2D& startRay = std::get<1>(filterRadialSection.startRay);
	const RayHit2D& endRay = std::get<1>(filterRadialSection.endRay);
	const ShapeId filterShapeId = filterRadialSection.shapeId;

	const Ray2D filterStartRay = Ray2D{startRay.location, startRay.ray.direction};
	const Ray2D filterEndRay = Ray2D{endRay.location, endRay.ray.direction};
	
	return shotScatterLight(filterShapeId, filterStartRay, filterEndRay,  
		getPointsExcluding(filterShapeId, shapes), bvh, scatterRaySpread);
}
GenerateScatterSectionsReturn generateFilterScatterSections(
	const RadialSection& filterRadialSection,  std::vector<RayVariant>& rays, const ScatterSectionBehavior& behavior,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {
	const RayHit2D& startRay = std::get<1>(filterRadialSection.startRay);
	const RayHit2D& endRay = std::get<1>(filterRadialSection.endRay);
	
	const ShapeId originShapeId = filterRadialSection.shapeId;
	const Color& filterColor = shapes[originShapeId].filterColor;
	const Color& color = applyFilter(filterRadialSection.color, filterColor);

	return generateScatterSections(color, startRay, endRay, rays, behavior, shapes, scatterSectionTolerance);
}

//scatter
ShotScatterReturn shotFilterScatterSection(
	const ScatterSection& filterScatterSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread) {
	const RayHit2D& startRay = std::get<1>(filterScatterSection.startRay);
	const RayHit2D& endRay = std::get<1>(filterScatterSection.endRay);
	const ShapeId filterShapeId = filterScatterSection.shapeId;

	const Ray2D filterStartRay = Ray2D{startRay.location, startRay.ray.direction};
	const Ray2D filterEndRay = Ray2D{endRay.location, endRay.ray.direction};
	
	return shotScatterLight(filterShapeId, filterStartRay, filterEndRay,  
		getPointsExcluding(filterShapeId, shapes), bvh, scatterRaySpread);
}
GenerateScatterSectionsReturn generateFilterScatterSections(
	const ScatterSection& filterScatterSection, std::vector<RayVariant>& rays, const ScatterSectionBehavior& behavior,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {
	const RayHit2D& startRay = std::get<1>(filterScatterSection.startRay);
	const RayHit2D& endRay = std::get<1>(filterScatterSection.endRay);
	
	const ShapeId originShapeId = filterScatterSection.shapeId;
	const Color& filterColor = shapes[originShapeId].filterColor;
	const Color& color = applyFilter(filterScatterSection.color, filterColor);

	return generateScatterSections(color, startRay, endRay, rays, behavior, shapes, scatterSectionTolerance);
}