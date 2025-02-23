#include "circleLight2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void CircleLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_ray_count"), &CircleLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &CircleLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &CircleLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &CircleLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10000, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

int64_t CircleLight2D::get_ray_count() const {
	return rayCount;
}
void CircleLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool CircleLight2D::get_draw_debug() const {
	return drawDebug;
}
void CircleLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

CircleLight2D::CircleLight2D() {
	rayCount = 3;
	drawDebug = false;
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
		Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 1;
		real_t distance = 10000;
		draw_circle(Point2(0, 0), 30, color, true);
		
		std::size_t numOfDisSegments = 50;
		for(std::size_t i = 0; i < numOfDisSegments; i++) {
			real_t localDistance = (distance / numOfDisSegments) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = (Math_PI * 2) / numOfSegments;
				real_t localAngle = (segmentSpreadAngle * j);
				
				draw_line(
					Point2{cos(localAngle), sin(localAngle)} * localDistance,
					Point2{cos(localAngle + segmentSpreadAngle), sin(localAngle + segmentSpreadAngle)} * localDistance,
					color, lineWidth);
			}
		}
	}
}