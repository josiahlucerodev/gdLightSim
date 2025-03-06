#include "circleLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "util.h"
#include "settings.h"
#include "spotLight2D.h"


void CircleLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &CircleLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &CircleLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");

	ClassDB::bind_method(D_METHOD("get_color"), &CircleLight2D::get_color);
	ClassDB::bind_method(D_METHOD("set_color", "color"), &CircleLight2D::set_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
}

bool CircleLight2D::get_draw_debug() const {
	return drawDebug;
}
void CircleLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

Color CircleLight2D::get_color() const {
	return color;
}
void CircleLight2D::set_color(const Color color) {
	this->color = color;
}

CircleLight2D::CircleLight2D() {
	drawDebug = false;
	color = Settings::defaultLightColor;
}

CircleLight2D::~CircleLight2D() {
}

void CircleLight2D::_ready() {
	queue_redraw();
}

void CircleLight2D::_process(double delta) {
	queue_redraw();
}

void CircleLight2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::debugLightEmitterColor);
		
		for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
			real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = (Math_PI * 2) / numOfSegments;
				real_t localAngle = (segmentSpreadAngle * j);
				
				draw_line(
					vectorFromAngle(localAngle) * localDistance,
					vectorFromAngle(localAngle + segmentSpreadAngle) * localDistance,
					Settings::debugLightEmitterColor, Settings::debugLineWidth);
			}
		}
	}
}

ShotCircleLight2DInfo shotCircleLight2D(
	const CircleLight2D& circleLight, const std::vector<Point2>& points, 
	BVH2D& bvh, const std::vector<Shape2D>& shapes,
	const real_t& radialRaySpread, const real_t& radialSectionTolerance) {
	Point2 circleLightLocation = circleLight.get_position();
		
	std::vector<RayVariant> h1Rays = shotSpotLight2D(0, circleLightLocation, Math_PI, points, bvh, radialRaySpread);
	std::vector<RadialSection> h1Sections = generateRadialSections(circleLight.get_color(), 0, h1Rays, shapes, radialSectionTolerance);
	std::vector<RayVariant> h2Rays = shotSpotLight2D(Math_PI, circleLightLocation, Math_PI, points, bvh, radialRaySpread);
	std::vector<RadialSection> h2Sections = generateRadialSections(circleLight.get_color(), Math_PI, h2Rays, shapes, radialSectionTolerance);

	ShotCircleLight2DInfo raySections;
	raySections.rays.insert(raySections.rays.end(), h1Rays.begin(), h1Rays.end());
	raySections.rays.insert(raySections.rays.end(), h2Rays.begin(), h2Rays.end());
	raySections.sections.insert(raySections.sections.end(), h1Sections.begin(), h1Sections.end());
	raySections.sections.insert(raySections.sections.end(), h2Sections.begin(), h2Sections.end());
	return raySections;
}