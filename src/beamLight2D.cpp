#include "beamLight2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void BeamLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_light_width"), &BeamLight2D::get_light_width);
	ClassDB::bind_method(D_METHOD("set_light_width", "light_width"), &BeamLight2D::set_light_width);
	ClassDB::bind_method(D_METHOD("get_ray_count"), &BeamLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &BeamLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &BeamLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &BeamLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "light_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_light_width", "get_light_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double BeamLight2D::get_light_width() const {
	return lightWidth;
}
void BeamLight2D::set_light_width(const double lightWidth) {
	this->lightWidth = lightWidth;
}
int64_t BeamLight2D::get_ray_count() const {
	return rayCount;
}
void BeamLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool BeamLight2D::get_draw_debug() const {
	return drawDebug;
}
void BeamLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

BeamLight2D::BeamLight2D() {
	lightWidth = 10;
	rayCount = 3;
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
        Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 5;
		size_t distance = 10000;
        real_t r = get_rotation();
	
        real_t piOver2 = Math_PI / 2;
        Point2 rightOffset = Point2(0, -1) * lightWidth;
        Point2 leftOffset = Point2(0, 1) * lightWidth;

        draw_line(rightOffset, rightOffset + Vector2(distance, 0)
            ,color, lineWidth);
        draw_line(leftOffset, leftOffset + Vector2(distance, 0)
            ,color, lineWidth);

        size_t distanceBetweenSections = 100;
        for(size_t i = 0; i < (distance / distanceBetweenSections); i++) {
            size_t segmentDistance = (distanceBetweenSections * i);
            draw_line(leftOffset + Vector2(segmentDistance, 0), rightOffset + Vector2(segmentDistance, 0)
                ,color, lineWidth);
        }
    }
}