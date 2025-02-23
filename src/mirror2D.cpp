#include "mirror2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void Mirror2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_mirror_width"), &Mirror2D::get_mirror_width);
	ClassDB::bind_method(D_METHOD("set_mirror_width", "mirror_width"), &Mirror2D::set_mirror_width);
	ClassDB::bind_method(D_METHOD("get_ray_count"), &Mirror2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &Mirror2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &Mirror2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &Mirror2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mirror_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_mirror_width", "get_mirror_width");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10000, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}


double Mirror2D::get_mirror_width() const {
	return mirrorWidth;
}
void Mirror2D::set_mirror_width(const double mirrorWidth) {
	this->mirrorWidth = mirrorWidth;
}
int64_t Mirror2D::get_ray_count() const {
	return rayCount;
}
void Mirror2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool Mirror2D::get_draw_debug() const {
	return drawDebug;
}
void Mirror2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

Mirror2D::Mirror2D() {
	rayCount = 3;
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
		Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 1;
		real_t distance = 10000;
		draw_circle(Point2(0, 0), 30, color, true);

        Point2 rightOffset = Point2(0, -1) * mirrorWidth;
        Point2 leftOffset = Point2(0, 1) * mirrorWidth;

        draw_line(leftOffset, rightOffset
            ,color, lineWidth);
	}
}