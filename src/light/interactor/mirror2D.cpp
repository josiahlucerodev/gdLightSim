#include "mirror2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "../settings.h"

using namespace godot;

//binding
void Mirror2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Mirror2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Mirror2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");

    ClassDB::bind_method(D_METHOD("get_mirror_width"), &Mirror2D::get_mirror_width);
	ClassDB::bind_method(D_METHOD("set_mirror_width", "mirror_width"), &Mirror2D::set_mirror_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mirror_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_mirror_width", "get_mirror_width");

	ClassDB::bind_method(D_METHOD("debug_draw"), &Mirror2D::debug_draw);
}

//getters/setters
bool Mirror2D::get_draw_debug() const {
	return drawDebug;
}
void Mirror2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t Mirror2D::get_mirror_width() const {
	return get_scale().y * 2;
}
void Mirror2D::set_mirror_width(const real_t mirrorWidth) {
	Vector2 scale = get_scale();
	scale.y = mirrorWidth / 2;
	set_scale(scale);
}

Mirror2DInfo Mirror2D::getInfo() const {
	Mirror2DInfo info;
	info.width = get_mirror_width();
	info.position = get_global_position();
	info.rotation = get_global_rotation();
	return info;
}

//constructor/deconstructor
Mirror2D::Mirror2D() {
	set_light_actor_type(LightActor2DType::mirror);
	drawDebug = false;
}

Mirror2D::~Mirror2D() {
}

//ops
void Mirror2D::_draw() {
	debug_draw();
}

void Mirror2D::debug_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (get_mirror_width() / 2);
        Point2 leftOffset = Point2(0, 1) * (get_mirror_width() / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}