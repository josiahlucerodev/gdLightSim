#include "beamLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "settings.h"
#include "util.h"

using namespace godot;

void BeamLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_light_width"), &BeamLight2D::get_light_width);
	ClassDB::bind_method(D_METHOD("set_light_width", "light_width"), &BeamLight2D::set_light_width);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &BeamLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &BeamLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "light_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_light_width", "get_light_width");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double BeamLight2D::get_light_width() const {
	return lightWidth;
}
void BeamLight2D::set_light_width(const double lightWidth) {
	this->lightWidth = lightWidth;
}

bool BeamLight2D::get_draw_debug() const {
	return drawDebug;
}
void BeamLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

BeamLight2D::BeamLight2D() {
	lightWidth = 10;
	drawDebug = false;
}

BeamLight2D::~BeamLight2D() {
}

void BeamLight2D::_ready() {
	queue_redraw();
}

void BeamLight2D::_process(double delta) {
	queue_redraw();
}

void BeamLight2D::_draw() {
    if(drawDebug) {
        real_t r = get_rotation();	
        real_t piOver2 = Math_PI / 2;
        Point2 rightOffset = Point2(0, -1) * (lightWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (lightWidth / 2);

        draw_line(rightOffset, rightOffset + Vector2(Settings::debugDistance, 0)
            ,Settings::debugLightColor, Settings::debugLineWidth);
        draw_line(leftOffset, leftOffset + Vector2(Settings::debugDistance, 0)
            ,Settings::debugLightColor, Settings::debugLineWidth);

        for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
			real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
            draw_line(leftOffset + Vector2(localDistance, 0), rightOffset + Vector2(localDistance, 0)
                , Settings::debugLightColor, Settings::debugLineWidth);
        }
    }
}

std::vector<RayVariant> shotBeamLight2D(
	const BeamLight2D& beamLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t linearRaySpread) {
	Point2 beamLightLocation = beamLight.get_position();
	real_t beamLightWidth = beamLight.get_light_width();
	real_t beamLightRotation = beamLight.get_rotation();

	Vector2 beamDirection = vectorFromAngle(beamLightRotation);
	real_t piOver2 = Math_PI / 2;
	Vector2 unscaledRightBeamPoint = vectorFromAngle(beamLightRotation + piOver2);
	Vector2 rightBeamPoint = unscaledRightBeamPoint * (beamLightWidth / 2);
	rightBeamPoint += beamLightLocation;
	Vector2 unscaledLeftBeamPoint = vectorFromAngle(beamLightRotation - piOver2);
	Vector2 leftBeamPoint = unscaledLeftBeamPoint * (beamLightWidth / 2);
	leftBeamPoint += beamLightLocation;

	
	std::vector<RayVariant> rays;
	auto testRay = [&](Ray2D ray) {
		std::optional<RayHit2D> rayHit = shotRay(ray, {}, bvh);
        if(rayHit.has_value()) {
			rays.push_back(RayVariant(rayHit.value()));
        } else {
			rays.push_back(RayVariant(ray));
        }
	};
	
	std::vector<Point2> beamPoints;
	beamPoints.reserve(points.size());
	for(Point2 point : points) {
		std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, beamDirection * -1}, rightBeamPoint, leftBeamPoint);
		if(rayOrigin.has_value()) {
			beamPoints.push_back(rayOrigin.value());
		}
	}
	
	{
		testRay(Ray2D{leftBeamPoint, beamDirection});
		testRay(Ray2D{rightBeamPoint, beamDirection});
	}

	for(size_t i = 0; i < beamPoints.size(); i++) {
		Point2& beamPoint = beamPoints[i];

		for(std::size_t i = 0; i < Settings::rayCount; i++) {
			real_t offset = (linearRaySpread / -2) + ((linearRaySpread / Settings::rayCount) * i);
			Vector2 spreadOrigin = beamPoint + Vector2{offset, offset};
			testRay(Ray2D{spreadOrigin, beamDirection});
		}
	}

	return rays;
}

std::vector<LinearSection> generateBeamLight2DSections(
	const BeamLight2D& beamLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance) {
	Point2 beamLightLocation = beamLight.get_position();

	real_t beamLightRotation = beamLight.get_rotation();
	Vector2 beamDirection = vectorFromAngle(beamLightRotation);
	real_t piOver2 = Math_PI / 2;
	Vector2 unscaledRightBeamPoint = vectorFromAngle(beamLightRotation + piOver2);
	real_t beamLightWidth = beamLight.get_light_width();
	Vector2 rightBeamPoint = (unscaledRightBeamPoint * (beamLightWidth / 2)) + beamLightLocation;

	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			Point2 lhsOrigin = getRay(lhs).origin;
			Point2 rhsOrigin = getRay(rhs).origin;
			return lhsOrigin.distance_to(rightBeamPoint) < rhsOrigin.distance_to(rightBeamPoint);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > linearSectionTolerance;
	};

	return generateSectionsBase<LinearSection>(shapes, rays, predicate);
}