#include "mirror2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "angle.h"
#include "settings.h"

using namespace godot;

void Mirror2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_mirror_width"), &Mirror2D::get_mirror_width);
	ClassDB::bind_method(D_METHOD("set_mirror_width", "mirror_width"), &Mirror2D::set_mirror_width);
	ClassDB::bind_method(D_METHOD("get_max_bounce"), &Mirror2D::get_max_bounce);
	ClassDB::bind_method(D_METHOD("set_max_bounce", "max_bounce"), &Mirror2D::set_max_bounce);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Mirror2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Mirror2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mirror_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_mirror_width", "get_mirror_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_bounce", PROPERTY_HINT_RANGE, "0,1000, 1"), "set_max_bounce", "get_max_bounce");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}


double Mirror2D::get_mirror_width() const {
	return mirrorWidth;
}
void Mirror2D::set_mirror_width(const double mirrorWidth) {
	this->mirrorWidth = mirrorWidth;
}
int64_t Mirror2D::get_max_bounce() const {
	return maxBounce;
}
void Mirror2D::set_max_bounce(const int64_t maxBounce) {
	this->maxBounce = maxBounce;
}

bool Mirror2D::get_draw_debug() const {
	return drawDebug;
}
void Mirror2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

Mirror2D::Mirror2D() {
	maxBounce = 10;
	drawDebug = false;
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
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::debugLightColor, true);

        Point2 rightOffset = Point2(0, -1) * (mirrorWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (mirrorWidth / 2);

        draw_line(leftOffset, rightOffset
			, Settings::debugLightColor, Settings::debugLineWidth);
	}
}

Shape2D constructShape2D(Mirror2D& mirror, std::size_t shapeId) {
    Shape2D shape;
    shape.type = Shape2DType::mirror;
    shape.shapeId = shapeId;
    shape.maxBounce = mirror.get_max_bounce();
    
    real_t mirrorWidth = mirror.get_mirror_width();
    Vector2 position = mirror.get_transform().get_origin();

    real_t rotation = mirror.get_transform().get_rotation() + (Math_PI / 2);
    Point2 dir = Point2{cos(rotation), sin(rotation)};
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

void addLinearMirrorBounceSectionsToQueue(
	const std::vector<Shape2D> shapes,
	std::deque<LinearScanSection>& lssQueue,
	const std::vector<LinearScanSection> linearScanSections, 
	std::size_t bounceLimit) {
	
	for(LinearScanSection lss : linearScanSections) {
		if(lss.type == SectionType::hit && shapes[lss.shapeId].type == Shape2DType::mirror
			&& lss.bounceIndex < bounceLimit) {
            lssQueue.push_back(lss);
        }
	}
}

std::vector<RayVariant> shotMirrorBeam(
	const LinearScanSection& mirrorHitLinearScanSection,
	const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, 
	real_t linearRaySpread) {

	RayHit2D startRay = std::get<1>(mirrorHitLinearScanSection.startRay);
	RayHit2D endRay = std::get<1>(mirrorHitLinearScanSection.endRay);

	ShapeId mirrorShapeId = startRay.shapeId;
	Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
	real_t bounceWidth = startRay.location.distance_to(endRay.location);
	real_t mirrorRotation = clockwiseAngle(bounceMidLocation, startRay.location);
	real_t rayAngle = -(startRay.angle - mirrorRotation) + mirrorRotation;
	Vector2 rayDir = Vector2{cos(rayAngle), sin(rayAngle)}; 
	
	const Shape2D& mirrorShape = shapes[mirrorHitLinearScanSection.shapeId];
	Vector2 unscaledRightPoint = (endRay.location - bounceMidLocation).normalized();
	Vector2 unscaledLeftPoint = (startRay.location - bounceMidLocation).normalized();

	std::vector<Point2> beamPoints;
	for(Point2 point : getPointsExcluding(mirrorShapeId, shapes)) {
		std::optional<Point2> rayOrigin 
			= rayLineIntersection(Ray2D{point, rayDir * -1}, startRay.location, endRay.location);
		if(rayOrigin.has_value()) {
			beamPoints.push_back(rayOrigin.value());
		}
	}

	std::vector<RayVariant> rays;
	auto testRay = [&](Ray2D ray) {
		std::optional<RayHit2D> rayHit = shotRay(ray, mirrorShapeId, bvh);
		if(rayHit.has_value()) {
			rays.push_back(RayVariant(rayHit.value()));
		} else {
			rays.push_back(RayVariant(ray));
		}
	};


	{
		testRay(Ray2D{startRay.location, rayDir});
		testRay(Ray2D{endRay.location, rayDir});
	}

	for(size_t i = 0; i < beamPoints.size(); i++) {
		Point2& beamPoint = beamPoints[i];

		for(std::size_t i = 0; i < Settings::rayCount; i++) {
			real_t offset = (linearRaySpread / -2) + ((linearRaySpread / Settings::rayCount) * i);
			Vector2 spreadOrigin = beamPoint + Vector2{offset, offset};
			testRay(Ray2D{spreadOrigin, rayDir});
		}
	}

	return rays;
}

std::vector<LinearScanSection> generateMirrorBeamSections(
	const LinearScanSection& mirrorHitLinearScanSection, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance) {

	RayHit2D startRay = std::get<1>(mirrorHitLinearScanSection.startRay);
	RayHit2D endRay = std::get<1>(mirrorHitLinearScanSection.endRay);
	Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			Point2 lhsOrigin = getRay(lhs).origin;
			Point2 rhsOrigin = getRay(rhs).origin;
			Point2 startRayOrigin = std::get<1>(mirrorHitLinearScanSection.startRay).ray.origin;
			return lhsOrigin.distance_to(startRayOrigin) < rhsOrigin.distance_to(startRayOrigin);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
			return (p2.y - p1.y) / (p2.x - p1.x);
		};            

		double slope1 = calculateSlope(r1.location, r2.location);
		double slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > linearSectionTolerance;
	};

	return generateSectionsBase<LinearScanSection>(shapes, rays, predicate);
}