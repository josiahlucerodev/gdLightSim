#include "colorFilter2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "../settings.h"

//bindings
void ColorFilter2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &ColorFilter2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &ColorFilter2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
	
	ClassDB::bind_method(D_METHOD("get_filter_width"), &ColorFilter2D::get_filter_width);
	ClassDB::bind_method(D_METHOD("set_filter_width", "filter_width"), &ColorFilter2D::set_filter_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "filter_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_filter_width", "get_filter_width");
	
	ClassDB::bind_method(D_METHOD("get_filter_color"), &ColorFilter2D::get_filter_color);
	ClassDB::bind_method(D_METHOD("set_filter_color", "filter_color"), &ColorFilter2D::set_filter_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "filter_color"), "set_filter_color", "get_filter_color");
}

//getter/setters
bool ColorFilter2D::get_draw_debug() const {
	return drawDebug;
}
void ColorFilter2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

real_t ColorFilter2D::get_filter_width() const {
	return filterWidth;
}
void ColorFilter2D::set_filter_width(const real_t filterWidth) {
	this->filterWidth = filterWidth;
}

Color ColorFilter2D::get_filter_color() const {
	return filterColor;
}
void ColorFilter2D::set_filter_color(const Color color) {
	this->filterColor = color;
}
ColorFilter2DInfo ColorFilter2D::getInfo() const {
	ColorFilter2DInfo info;
	info.filterColor = get_filter_color();
	info.width = get_filter_width();
	info.position = get_global_position();
	info.rotation = get_global_rotation();
	return info;
}

//constructor/destructor
ColorFilter2D::ColorFilter2D() {
	drawDebug = false;
	filterColor = Settings::defaultLightColor;
	filterWidth = 100;
}

ColorFilter2D::~ColorFilter2D() {
}

//ops
void ColorFilter2D::_ready() {
	queue_redraw();
}

void ColorFilter2D::_process(double delta) {
	queue_redraw();
}

void ColorFilter2D::_draw() {
	if(drawDebug) {
		draw_circle(Point2(0, 0), Settings::pointRadius, Settings::defaultObjectColor, true);

        Point2 rightOffset = Point2(0, -1) * (filterWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (filterWidth / 2);

        draw_line(leftOffset, rightOffset
			, Settings::defaultObjectColor, Settings::debugLineWidth);
	}
}