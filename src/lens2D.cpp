#include "lens2D.h"

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

void Lens2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Lens2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Lens2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
	
	ClassDB::bind_method(D_METHOD("get_lens_width"), &Lens2D::get_lens_width);
	ClassDB::bind_method(D_METHOD("set_lens_width", "lens_width"), &Lens2D::set_lens_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lens_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_lens_width", "get_lens_width");
	
	ClassDB::bind_method(D_METHOD("get_focal_length"), &Lens2D::get_focal_length);
	ClassDB::bind_method(D_METHOD("set_focal_length", "focal_length"), &Lens2D::set_focal_length);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "focal_length", PROPERTY_HINT_RANGE, "-2000,2000,0.1"), "set_focal_length", "get_focal_length");
}

bool Lens2D::get_draw_debug() const {
	return drawDebug;
}
void Lens2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t Lens2D::get_lens_width() const {
	return lensWidth;
}
void Lens2D::set_lens_width(const real_t lensWidth) {
	this->lensWidth = lensWidth;
}

real_t Lens2D::get_focal_length() const {
	return focalLength;
}
void Lens2D::set_focal_length(const real_t focalLength) {
	this->focalLength = focalLength;
}

Lens2D::Lens2D() {
	drawDebug = false;
	focalLength = 100;
	lensWidth = 100;
}

Lens2D::~Lens2D() {
}

void Lens2D::_ready() {
	queue_redraw();
}

void Lens2D::_process(double delta) {
	queue_redraw();
}

void Lens2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (lensWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (lensWidth / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}

Shape2D constructShape2D(Lens2D& lens, ShapeId shapeId) {
    Shape2D shape;
    shape.type = Shape2DType::lens;
    shape.shapeId = shapeId;
    shape.focalLength = lens.get_focal_length();
    
    real_t width = lens.get_lens_width();
    Vector2 position = lens.get_transform().get_origin();

    real_t rotation = lens.get_transform().get_rotation() + (Math_PI / 2);
    Point2 dir = vectorFromAngle(rotation);
    Point2 rightPoint = position + (dir * (width / 2));
    Point2 leftPoint = position + ((dir * (-1)) * (width / 2));
    
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


std::vector<RayVariant> shotLensRadialSection(
	const RadialSection& lensRadialSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread) {
	return {};
}
std::vector<ScatterSection> generateLensScatterSections(
	const RadialSection& lensRadialSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {
	return {};
}

Ray2D refractRayThroughLens(const Vector2& lensMidpoint, const real_t focalLength, const Ray2D& ray) {
    const Vector2 rayToLens = lensMidpoint - ray.origin;
    Vector2 incomingDir = ray.direction.normalized();
    real_t factor = (1.0f / focalLength) * rayToLens.length();
    Vector2 newDirection = incomingDir + (factor * rayToLens.normalized());
    return Ray2D{ray.origin, newDirection.normalized()};
}

std::vector<RayVariant> shotLensLinearSection(
	const LinearSection& lensLinearSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread) {

	RayHit2D startRay = std::get<1>(lensLinearSection.startRay);
	RayHit2D endRay = std::get<1>(lensLinearSection.endRay);

	const ShapeId lensShapeId = startRay.shapeId;
	const Vector2 lensMidPoint = shapes[lensShapeId].midPoint;
	const real_t focalLength = shapes[lensShapeId].focalLength;

	Ray2D startReflectRay = Ray2D{startRay.location, startRay.ray.direction};
	startReflectRay = refractRayThroughLens(lensMidPoint, focalLength, startReflectRay);
	Ray2D endReflectRay = Ray2D{endRay.location, endRay.ray.direction};
	endReflectRay =	refractRayThroughLens(lensMidPoint, focalLength, endReflectRay);
	
	return shotScatterLight(lensShapeId, endReflectRay, startReflectRay, 
		getPointsExcluding(lensShapeId, shapes), bvh, scatterRaySpread);
}
std::vector<ScatterSection> generateLensScatterSections(
	const LinearSection& lensLinearSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {

	const RayHit2D startRay = std::get<1>(lensLinearSection.startRay);
	const RayHit2D endRay = std::get<1>(lensLinearSection.endRay);

	return generateScatterSections(lensLinearSection.color, startRay, endRay, rays, shapes, scatterSectionTolerance);
}
				
std::vector<RayVariant> shotLensScatterSection(
	const ScatterSection& lensScatterSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread) {
	return {};
}
std::vector<ScatterSection> generateLensScatterSections(
	const ScatterSection& lensScatterSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {
	return {};
}