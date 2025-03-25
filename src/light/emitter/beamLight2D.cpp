#include "beamLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "../settings.h"

using namespace godot;

//binding
void BeamLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &BeamLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &BeamLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
		
	ClassDB::bind_method(D_METHOD("get_light_width"), &BeamLight2D::get_light_width);
	ClassDB::bind_method(D_METHOD("set_light_width", "light_width"), &BeamLight2D::set_light_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "light_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_light_width", "get_light_width");
	ClassDB::bind_method(D_METHOD("get_light_color"), &BeamLight2D::get_light_color);
	ClassDB::bind_method(D_METHOD("set_light_color", "light_color"), &BeamLight2D::set_light_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "light_color"), "set_light_color", "get_light_color");

	ClassDB::bind_method(D_METHOD("debug_draw"), &BeamLight2D::debug_draw);
}

//getters/setters
real_t BeamLight2D::get_light_width() const {
	return get_scale().y * 2;
}
void BeamLight2D::set_light_width(const real_t lightWidth) {
	Vector2 scale = get_scale();
	scale.y = lightWidth / 2;
	set_scale(scale);
}

bool BeamLight2D::get_draw_debug() const {
	return drawDebug;
}
void BeamLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}
Color BeamLight2D::get_light_color() const {
	return lightColor;
}
void BeamLight2D::set_light_color(const Color lightColor) {
	this->lightColor = lightColor;
}
BeamLight2DInfo BeamLight2D::getInfo() {
	BeamLight2DInfo info;
	info.color = get_light_color();
	info.width = get_light_width();
	info.position = get_global_position();
	info.rotation = get_global_rotation();
	return info;
}

//constructor/destructor
BeamLight2D::BeamLight2D() {
	set_light_actor_type(LightActor2DType::beamLight);
	drawDebug = false;
	lightColor = Settings::defaultLightColor;
}

BeamLight2D::~BeamLight2D() {
}

//ops
void BeamLight2D::_draw() {
	debug_draw();
}

void BeamLight2D::debug_draw() {
    if(drawDebug) {
        real_t r = get_rotation();	
        real_t piOver2 = Math_PI / 2;
        Point2 rightOffset = Point2(0, -1) * (get_light_width() / 2);
        Point2 leftOffset = Point2(0, 1) * (get_light_width() / 2);

        draw_line(rightOffset, rightOffset + Vector2(Settings::debugDistance, 0)
            ,Settings::debugLightEmitterColor, Settings::debugLineWidth);
        draw_line(leftOffset, leftOffset + Vector2(Settings::debugDistance, 0)
            ,Settings::debugLightEmitterColor, Settings::debugLineWidth);

        for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
			real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
            draw_line(leftOffset + Vector2(localDistance, 0), rightOffset + Vector2(localDistance, 0)
                , Settings::debugLightEmitterColor, Settings::debugLineWidth);
        }
    }
}