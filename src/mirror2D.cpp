#include "mirror2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "util.h"
#include "settings.h"
#include "shotLight.h"

using namespace godot;

void Mirror2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Mirror2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Mirror2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");

    ClassDB::bind_method(D_METHOD("get_mirror_width"), &Mirror2D::get_mirror_width);
	ClassDB::bind_method(D_METHOD("set_mirror_width", "mirror_width"), &Mirror2D::set_mirror_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mirror_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_mirror_width", "get_mirror_width");
}


bool Mirror2D::get_draw_debug() const {
	return drawDebug;
}
void Mirror2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t Mirror2D::get_mirror_width() const {
	return mirrorWidth;
}
void Mirror2D::set_mirror_width(const real_t mirrorWidth) {
	this->mirrorWidth = mirrorWidth;
}

Mirror2D::Mirror2D() {
	drawDebug = false;
	mirrorWidth = 100;
}

Mirror2D::~Mirror2D() {
}

void Mirror2D::_ready() {
	queue_redraw();
}

void Mirror2D::_process(double delta) {
	queue_redraw();
}

void Mirror2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (mirrorWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (mirrorWidth / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}

Shape2D constructShape2D(Mirror2D& mirror, std::size_t shapeId) {
    Shape2D shape;
    shape.type = Shape2DType::mirror;
    shape.shapeId = shapeId;
    
    real_t mirrorWidth = mirror.get_mirror_width();
    Vector2 position = mirror.get_transform().get_origin();

    real_t rotation = mirror.get_transform().get_rotation() + (Math_PI / 2);
    Point2 dir = vectorFromAngle(rotation);
    Point2 rightPoint = position + (dir * (mirrorWidth / 2));
    Point2 leftPoint = position + ((dir * (-1)) * (mirrorWidth / 2));
    
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

std::vector<RayVariant> shotMirrorLinearSection(
	const LinearSection& mirrorLinearSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t linearRaySpread) {

	RayHit2D startRay = std::get<1>(mirrorLinearSection.startRay);
	RayHit2D endRay = std::get<1>(mirrorLinearSection.endRay);

	ShapeId mirrorShapeId = startRay.shapeId;
	Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
	
	real_t mirrorRotation = clockwiseAngle(bounceMidLocation, startRay.location);
	real_t reflectAngle = getRayReflectionAngle(startRay.angle, mirrorRotation);
	Vector2 reflectDirection = vectorFromAngle(reflectAngle); 

	return shotLinearLight(mirrorShapeId, startRay.location, endRay.location, reflectDirection, 
		getPointsExcluding(mirrorShapeId, shapes), bvh, linearRaySpread);
}

std::vector<LinearSection> generateMirrorLinearSections(
	const LinearSection& mirrorLinearSection, std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, real_t linearSectionTolerance) {
	RayHit2D startRay = std::get<1>(mirrorLinearSection.startRay);
	return generateLinearSections(mirrorLinearSection.color, startRay.ray.origin, rays, shapes, linearSectionTolerance);
}

std::vector<RayVariant> shotMirrorScatterLight(const RayHit2D startRay, const RayHit2D endRay, 
	const std::vector<Shape2D>& shapes, BVH2D& bvh, const real_t& scatterRaySpread) {
	
	const ShapeId mirrorShapeId = startRay.shapeId;
	const Point2 hitMidLocation = (startRay.location + endRay.location) / 2;
	const real_t sectionHitWidth = startRay.location.distance_to(endRay.location);
	const real_t mirrorRotation = clockwiseAngle(hitMidLocation, startRay.location);

	const Ray2D startReflectRay = Ray2D{startRay.location, 
		vectorFromAngle(getRayReflectionAngle(startRay.angle, mirrorRotation))
	};
	const Ray2D endReflectRay = Ray2D{endRay.location, 
		vectorFromAngle(getRayReflectionAngle(endRay.angle, mirrorRotation))
	};

	return shotScatterLight(mirrorShapeId, startReflectRay, endReflectRay, 
		getPointsExcluding(mirrorShapeId, shapes), bvh, scatterRaySpread);
}

std::vector<RayVariant> shotMirrorRadialSection(
	const RadialSection& mirrorRadialSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread) {

	const RayHit2D startRay = std::get<1>(mirrorRadialSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorRadialSection.endRay);
	return shotMirrorScatterLight(startRay, endRay, shapes, bvh, scatterRaySpread);
}

std::vector<ScatterSection> generateMirrorScatterSections(
	const RadialSection& mirrorRadialSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance) {
	const RayHit2D startRay = std::get<1>(mirrorRadialSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorRadialSection.endRay);
	return generateScatterSections(mirrorRadialSection.color, startRay, endRay, rays, shapes, scatterSectionTolerance);
}

std::vector<RayVariant> shotMirrorScatterSection(
	const ScatterSection& mirrorScatterSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread) {
	const RayHit2D startRay = std::get<1>(mirrorScatterSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorScatterSection.endRay);
	return shotMirrorScatterLight(startRay, endRay, shapes, bvh, scatterRaySpread);
}

std::vector<ScatterSection> generateMirrorScatterSections(
	const ScatterSection& mirrorScatterSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance) {
	const RayHit2D startRay = std::get<1>(mirrorScatterSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorScatterSection.endRay);
	return generateScatterSections(mirrorScatterSection.color, startRay, endRay, rays, shapes, scatterSectionTolerance);
}