#include "spotLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "../settings.h"
#include "../ops/util.h"

//binding
void SpotLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_arc"), &SpotLight2D::get_arc);
	ClassDB::bind_method(D_METHOD("set_arc", "arc"), &SpotLight2D::set_arc);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "arc", PROPERTY_HINT_RANGE, "1,180,0.01"), "set_arc", "get_arc");

	ClassDB::bind_method(D_METHOD("get_draw_debug"), &SpotLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &SpotLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
	
	ClassDB::bind_method(D_METHOD("get_light_color"), &SpotLight2D::get_light_color);
	ClassDB::bind_method(D_METHOD("set_light_color", "light_color"), &SpotLight2D::set_light_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "light_color"), "set_light_color", "get_light_color");

	ClassDB::bind_method(D_METHOD("debug_draw"), &SpotLight2D::debug_draw);
}

//getter/setters
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

Color SpotLight2D::get_light_color() const {
	return lightColor;
}
void SpotLight2D::set_light_color(const Color lightColor) {
	this->lightColor = lightColor;
}
SpotLight2DInfo SpotLight2D::getInfo() const {
	SpotLight2DInfo info;
	info.color = get_light_color();
	info.arc = Math::deg_to_rad(get_arc());
	info.position = get_global_position();
	info.rotation = get_global_rotation();
	return info;
}

//constructor/destructor
SpotLight2D::SpotLight2D() {
	set_light_actor_type(LightActor2DType::spotLight);
	arc = 30;
	drawDebug = false;
	lightColor = Settings::defaultLightColor;
}

SpotLight2D::~SpotLight2D() {
}

//ops
void SpotLight2D::_draw() {
	debug_draw();
}
void SpotLight2D::debug_draw() {
	if(drawDebug) {
		const real_t arcRad = Math::deg_to_rad(arc);
		const real_t halfArcRad = arcRad / 2;
		draw_line(Point2(0, 0), vectorFromAngle(-halfArcRad) * Settings::debugDistance, 
			Settings::debugLightEmitterColor, Settings::debugLineWidth);
		draw_line(Point2(0, 0), vectorFromAngle(halfArcRad)  * Settings::debugDistance, 
			Settings::debugLightEmitterColor, Settings::debugLineWidth);
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::debugLightEmitterColor);
		
		for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
			real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = arcRad / numOfSegments;
				real_t localAngle = -halfArcRad + (segmentSpreadAngle * j);
				
				draw_line(
					vectorFromAngle(localAngle) * localDistance,
					vectorFromAngle(localAngle + segmentSpreadAngle) * localDistance,
					Settings::debugLightEmitterColor, Settings::debugLineWidth);
			}
		}
	}
}

//shot
std::vector<RayVariant> shotSpotLight2D(const SpotLight2DInfo& source, EmitterShotInfo& shotInfo) {

	std::vector<Point2> pointsInSpotlightArc;
	pointsInSpotlightArc.reserve(shotInfo.points.size());
	std::vector<RayVariant> rays;
	
	auto testRay = [&](Point2 direction) {
		const Ray2D ray = Ray2D{source.position, direction};
		std::optional<RayHit2D> rayHit = shotRay(ray, {}, shotInfo.bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
	};
	
	{
		real_t startAngle = source.rotation - (source.arc / 2);
		Point2 startRay = vectorFromAngle(startAngle);
		Point2 endRay = vectorFromAngle(startAngle + source.arc);
		testRay(startRay);
		testRay(endRay);
	}
	
	Point2 lightMidDir = vectorFromAngle(source.rotation);
	for(Point2 point : shotInfo.points) {
		real_t pointAngle = clockwiseAngle(source.position, point);
		Point2 pointDir = vectorFromAngle(pointAngle);

		if(abs(lightMidDir.angle_to(pointDir)) < source.arc / 2) {
			pointsInSpotlightArc.push_back(point);
		}
	}

	for(size_t i = 0; i < pointsInSpotlightArc.size(); i++) {
		Point2& point = pointsInSpotlightArc[i];
		real_t distance = source.position.distance_to(point);
		real_t arc = (shotInfo.raySettings.radialRaySpread / distance) / Settings::rayCount;

		for(std::size_t i = 0; i < Settings::rayCount; i++) {
			real_t angle = source.position.angle_to_point(point) - (arc / 2);
			angle += ((arc / Settings::rayCount) * i);
			Point2 direction = vectorFromAngle(angle);
			testRay(direction);
		}
	}
    return rays;
}