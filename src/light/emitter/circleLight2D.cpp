#include "circleLight2D.h"

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
void CircleLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &CircleLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &CircleLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");

	ClassDB::bind_method(D_METHOD("get_light_color"), &CircleLight2D::get_light_color);
	ClassDB::bind_method(D_METHOD("set_light_color", "light_color"), &CircleLight2D::set_light_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "light_color"), "set_light_color", "get_light_color");

	ClassDB::bind_method(D_METHOD("debug_draw"), &CircleLight2D::debug_draw);
}

//getter/setters
bool CircleLight2D::get_draw_debug() const {
	return drawDebug;
}
void CircleLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

Color CircleLight2D::get_light_color() const {
	return lightColor;
}
void CircleLight2D::set_light_color(const Color lightColor) {
	this->lightColor = lightColor;
}
CircleLight2DInfo CircleLight2D::getInfo() const {
	CircleLight2DInfo info;
	info.color = get_light_color();
	info.position = get_global_position();
	return info;
}

//constructor/deconstructor
CircleLight2D::CircleLight2D() {
	set_light_actor_type(LightActor2DType::circleLight);
	drawDebug = false;
	lightColor = Settings::defaultLightColor;
}

CircleLight2D::~CircleLight2D() {
}

//ops
void CircleLight2D::_draw() {
	debug_draw();
}

void CircleLight2D::debug_draw() {
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