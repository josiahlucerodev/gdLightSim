#include "mirror2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "util.h"
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

void addMirrorLinearSectionsToQueue(
	const std::vector<Shape2D> shapes,
	std::deque<LinearSection>& linearSectionQueue,
	const std::vector<LinearSection> linearSections, 
	std::size_t bounceLimit) {
	
	for(LinearSection lss : linearSections) {
		if(lss.type == SectionType::hit && shapes[lss.shapeId].type == Shape2DType::mirror
			&& lss.bounceIndex < bounceLimit) {
			linearSectionQueue.push_back(lss);
        }
	}
}

std::vector<RadialSection> getMirrorRadialSections(const std::vector<Shape2D>& shapes, std::vector<RadialSection>& radialSections) {
	std::vector<RadialSection> radialMirrorSections;
	for(RadialSection rss : radialSections) {
		if(rss.type == SectionType::hit && shapes[rss.shapeId].type == Shape2DType::mirror) {
			radialMirrorSections.push_back(rss);
        }
	}
	return radialMirrorSections;
}

void addMirrorScatterSectionsToQueue(const std::vector<Shape2D> shapes, std::deque<ScatterSection>& scatterSectionQueue,
	const std::vector<ScatterSection> scatterSections, std::size_t bounceLimit) {
	
	for(ScatterSection ss : scatterSections) {
		if(ss.type == SectionType::hit && shapes[ss.shapeId].type == Shape2DType::mirror
			&& ss.bounceIndex < bounceLimit) {
			scatterSectionQueue.push_back(ss);
        }
	}
}

std::vector<RayVariant> shotMirrorLinearSection(
	const LinearSection& mirrorLinearSection,
	const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, 
	real_t linearRaySpread) {

	RayHit2D startRay = std::get<1>(mirrorLinearSection.startRay);
	RayHit2D endRay = std::get<1>(mirrorLinearSection.endRay);

	ShapeId mirrorShapeId = startRay.shapeId;
	Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
	real_t bounceWidth = startRay.location.distance_to(endRay.location);
	real_t mirrorRotation = clockwiseAngle(bounceMidLocation, startRay.location);
	real_t rayAngle = -(startRay.angle - mirrorRotation) + mirrorRotation;
	Vector2 rayDir = vectorFromAngle(rayAngle); 
	
	const Shape2D& mirrorShape = shapes[mirrorLinearSection.shapeId];
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

std::vector<LinearSection> generateMirrorLinearSections(
	const LinearSection& mirrorLinearSection, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance) {

	RayHit2D startRay = std::get<1>(mirrorLinearSection.startRay);
	RayHit2D endRay = std::get<1>(mirrorLinearSection.endRay);
	Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			Point2 lhsOrigin = getRay(lhs).origin;
			Point2 rhsOrigin = getRay(rhs).origin;
			Point2 startRayOrigin = std::get<1>(mirrorLinearSection.startRay).ray.origin;
			return lhsOrigin.distance_to(startRayOrigin) < rhsOrigin.distance_to(startRayOrigin);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		double slope1 = calculateSlope(r1.location, r2.location);
		double slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > linearSectionTolerance;
	};

	return generateSectionsBase<LinearSection>(shapes, rays, predicate);
}

inline Vector2 weightedSlerp(const Vector2& v1, real_t w2, const Vector2& v2, real_t w1) {
    const real_t t = 0.5;
    real_t dot = v1.dot(v2);
    dot = std::clamp(dot, -1.0f, 1.0f);
    real_t theta = std::acos(dot);
    if (theta == 0.0f) {
        return v1;
    }
    const real_t sinTheta = std::sin(theta);
    const real_t scaleV1 = w1 * std::sin((1 - t) * theta) / sinTheta;
    const real_t scaleV2 = w2 * std::sin(t * theta) / sinTheta;
    return Vector2{scaleV1 * v1[0] + scaleV2 * v2[0], scaleV1 * v1[1] + scaleV2 * v2[1]}.normalized();
}

std::vector<RayVariant> shotMirrorGeneralSection(const RayHit2D startRay, const RayHit2D endRay, 
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

	const real_t mirrorSlope = calculateSlope(startReflectRay.origin, endReflectRay.origin);
	const real_t mirrorSlopeAngle = atan(mirrorSlope);
	const Vector2 mirrorSlopeDir = vectorFromAngle(mirrorSlopeAngle);
	
	std::vector<Ray2D> testRays;
	for(Point2 point : getPointsExcluding(mirrorShapeId, shapes)) {
		Point2 parallelLineToPointStart = (mirrorSlopeDir * 100000) + point; 
		Point2 parallelLineToPointEnd = (-mirrorSlopeDir * 100000) + point; 
		
		auto getDistance = [&](Ray2D ray) -> real_t {
			std::optional<Point2> hit = rayLineIntersection(ray, parallelLineToPointStart, parallelLineToPointEnd);
			if(!hit.has_value()) {
				return 10000;
			}
			return hit.value().distance_to(point);
		};
		
		real_t distanceToStart = getDistance(startReflectRay);
		real_t distanceToEnd = getDistance(endReflectRay);
		Vector2 rayDir = weightedSlerp(startReflectRay.direction, distanceToStart, endReflectRay.direction, distanceToEnd);
		
		std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, rayDir * -1}, startReflectRay.origin, endReflectRay.origin);
		if(rayOrigin.has_value()) {
			testRays.push_back(Ray2D{rayOrigin.value(), rayDir});
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

	testRay(startReflectRay);
	testRay(endReflectRay);

	for(Ray2D ray : testRays) {
		Ray2D side1 = Ray2D{ray.origin + (mirrorSlopeDir * scatterRaySpread), 
			ray.direction.rotated(scatterRaySpread).normalized()};
		Ray2D side2 = Ray2D{ray.origin - (mirrorSlopeDir * scatterRaySpread), 
			ray.direction.rotated(-scatterRaySpread).normalized()};
		testRay(side2);
		testRay(ray);
		testRay(side1);
	}
	
	return rays;
}

std::vector<RayVariant> shotMirrorRadialSection(
	const RadialSection& mirrorRadialSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread) {

	const RayHit2D startRay = std::get<1>(mirrorRadialSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorRadialSection.endRay);
	return shotMirrorGeneralSection(startRay, endRay, shapes, bvh, scatterRaySpread);
}

std::vector<ScatterSection> generateMirrorScatterSections(
	const RayHit2D startRay, const RayHit2D endRay,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance) {
	const real_t mirrorSlope = calculateSlope(startRay.location, endRay.location);
	const real_t mirrorSlopeAngle = atan(mirrorSlope);
	const Vector2 mirrorSlopeDir = vectorFromAngle(mirrorSlopeAngle);

	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return startRay.location.distance_to(getRay(lhs).origin) 
				< startRay.location.distance_to(getRay(rhs).origin);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > scatterSectionTolerance;
	};

	return generateSectionsBase<ScatterSection>(shapes, rays, predicate);
}

std::vector<ScatterSection> generateMirrorScatterSections(
	const RadialSection& mirrorRadialSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance) {
	const RayHit2D startRay = std::get<1>(mirrorRadialSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorRadialSection.endRay);
	return generateMirrorScatterSections(startRay, endRay, rays, shapes, scatterSectionTolerance);
}

std::vector<RayVariant> shotMirrorScatterSection(
	const ScatterSection& mirrorScatterSection, const std::vector<Shape2D>& shapes, 
	BVH2D& bvh, const real_t& scatterRaySpread) {
	const RayHit2D startRay = std::get<1>(mirrorScatterSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorScatterSection.endRay);
	return shotMirrorGeneralSection(startRay, endRay, shapes, bvh, scatterRaySpread);
}

std::vector<ScatterSection> generateMirrorScatterSections(
	const ScatterSection& mirrorScatterSection,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& scatterSectionTolerance) {
	const RayHit2D startRay = std::get<1>(mirrorScatterSection.startRay);
	const RayHit2D endRay = std::get<1>(mirrorScatterSection.endRay);
	return generateMirrorScatterSections(startRay, endRay, rays, shapes, scatterSectionTolerance);
}
