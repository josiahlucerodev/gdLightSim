#include "spotLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "util.h"
#include "settings.h"

void SpotLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_arc"), &SpotLight2D::get_arc);
	ClassDB::bind_method(D_METHOD("set_arc", "arc"), &SpotLight2D::set_arc);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &SpotLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &SpotLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "arc", PROPERTY_HINT_RANGE, "1,180,0.01"), "set_arc", "get_arc");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double SpotLight2D::get_arc() const {
	return arc;
}
void SpotLight2D::set_arc(const double arc) {
	this->arc = arc;
}
bool SpotLight2D::get_draw_debug() const {
	return drawDebug;
}
void SpotLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

SpotLight2D::SpotLight2D() {
	arc = 30;
	drawDebug = false;
}

SpotLight2D::~SpotLight2D() {
}

void SpotLight2D::_ready() {
	queue_redraw();
}

void SpotLight2D::_process(double delta) {
	queue_redraw();
}

void SpotLight2D::_draw() {
	if(drawDebug) {
		const real_t arcRad = Math::deg_to_rad(arc);
		const real_t halfArcRad = arcRad / 2;
		draw_line(Point2(0, 0), vectorFromAngle(-halfArcRad) * Settings::debugDistance, 
			Settings::debugLightColor, Settings::debugLineWidth);
		draw_line(Point2(0, 0), vectorFromAngle(halfArcRad)  * Settings::debugDistance, 
			Settings::debugLightColor, Settings::debugLineWidth);
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::debugLightColor);
		
		for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
			real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = arcRad / numOfSegments;
				real_t localAngle = -halfArcRad + (segmentSpreadAngle * j);
				
				draw_line(
					vectorFromAngle(localAngle) * localDistance,
					vectorFromAngle(localAngle + segmentSpreadAngle) * localDistance,
					Settings::debugLightColor, Settings::debugLineWidth);
			}
		}
	}
}

std::vector<RayVariant> shotSpotLight2D(
	const real_t& angle, const Point2& spotLightLocation, const real_t& arc,
	const std::vector<Point2>& points, BVH2D& bvh, const real_t& radialRaySpread) {

	std::vector<Point2> pointsInSpotlightArc;
	pointsInSpotlightArc.reserve(points.size());
	std::vector<RayVariant> rays;
	
	auto testRay = [&](Point2 direction) {
		Ray2D ray = Ray2D{spotLightLocation, direction};
		std::optional<RayHit2D> rayHit = shotRay(ray, {}, bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
	};
	
	{
		real_t startAngle = angle - (arc / 2);
		Point2 startRay = vectorFromAngle(startAngle);
		Point2 endRay = vectorFromAngle(startAngle + arc);
		testRay(startRay);
		testRay(endRay);
	}
	
	Point2 lightMidDir = vectorFromAngle(angle);
	for(Point2 point : points) {
		real_t pointAngle = clockwiseAngle(spotLightLocation, point);
		Point2 pointDir = vectorFromAngle(pointAngle);

		if(abs(lightMidDir.angle_to(pointDir)) < arc / 2) {
			pointsInSpotlightArc.push_back(point);
		}
	}

	for(size_t i = 0; i < pointsInSpotlightArc.size(); i++) {
		Point2& point = pointsInSpotlightArc[i];
		real_t distance = spotLightLocation.distance_to(point);
		real_t arc = (radialRaySpread / distance) / Settings::rayCount;

		for(std::size_t i = 0; i < Settings::rayCount; i++) {
			real_t angle = spotLightLocation.angle_to_point(point) - (arc / 2);
			angle += ((arc / Settings::rayCount) * i);
			Point2 direction = vectorFromAngle(angle);
			testRay(direction);
		}
	}
    return rays;
}

std::vector<RayVariant> shotSpotLight2D(
	const SpotLight2D& spotLight, const std::vector<Point2>& points, 
	BVH2D& bvh, const real_t& radialRaySpread) {
	return shotSpotLight2D(spotLight.get_rotation(), spotLight.get_position(), 
		Math::deg_to_rad(spotLight.get_arc()), points, bvh, radialRaySpread);
}

std::vector<RadialSection> generateSpotLight2DSections(
	const real_t& angle, std::vector<RayVariant>& rays, 
	const std::vector<Shape2D>& shapes, real_t radialSectionTolerance) {
	Point2 lightMidDir = vectorFromAngle(angle);
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return lightMidDir.angle_to(getRay(lhs).direction) 
				< lightMidDir.angle_to(getRay(rhs).direction);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > radialSectionTolerance;
	};

	return generateSectionsBase<RadialSection>(shapes, rays, predicate);
}

std::vector<RadialSection> generateSpotLight2DSections(
	const SpotLight2D& spotLight,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t& radialSectionTolerance) {
	return generateSpotLight2DSections(spotLight.get_rotation(), rays, shapes, radialSectionTolerance);
}