#include "lens2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "../settings.h"
//binding
void Lens2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Lens2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Lens2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
	
	ClassDB::bind_method(D_METHOD("get_focal_length"), &Lens2D::get_focal_length);
	ClassDB::bind_method(D_METHOD("set_focal_length", "focal_length"), &Lens2D::set_focal_length);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "focal_length", PROPERTY_HINT_RANGE, "-2000,2000,0.1"), "set_focal_length", "get_focal_length");

	ClassDB::bind_method(D_METHOD("debug_draw"), &Lens2D::debug_draw);
}

//getter/setters
bool Lens2D::get_draw_debug() const {
	return drawDebug;
}
void Lens2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t Lens2D::get_lens_width() const {
	return get_global_scale().y * 2;
}

real_t Lens2D::get_focal_length() const {
	return focalLength;
}
void Lens2D::set_focal_length(const real_t focalLength) {
	this->focalLength = focalLength;
}
Lens2DInfo Lens2D::getInfo() const {
	Lens2DInfo info;
	info.focalLength = get_focal_length();
	info.width = get_lens_width();
	info.position = get_global_position();
	info.rotation = get_global_rotation();
	return info;
}

//constructor/destructor
Lens2D::Lens2D() {
	set_light_actor_type(LightActor2DType::lens);
	drawDebug = false;
	focalLength = 100;
}

Lens2D::~Lens2D() {
}

//ops
void Lens2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (get_lens_width() / 2);
        Point2 leftOffset = Point2(0, 1) * (get_lens_width() / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}

void Lens2D::debug_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (get_lens_width() / 2);
        Point2 leftOffset = Point2(0, 1) * (get_lens_width() / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}