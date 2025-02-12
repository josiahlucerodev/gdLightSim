#include "spotLight2D.h"

//godot_cpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void SpotLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_arc"), &SpotLight2D::get_arc);
	ClassDB::bind_method(D_METHOD("set_arc", "arc"), &SpotLight2D::set_arc);
	ClassDB::bind_method(D_METHOD("get_ray_count"), &SpotLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &SpotLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &SpotLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &SpotLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "arc", PROPERTY_HINT_RANGE, "1,180,0.01"), "set_arc", "get_arc");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double SpotLight2D::get_arc() const {
	return arc;
}
void SpotLight2D::set_arc(const double arc) {
	this->arc = arc;
}
int64_t SpotLight2D::get_ray_count() const {
	return rayCount;
}
void SpotLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool SpotLight2D::get_draw_debug() const {
	return drawDebug;
}
void SpotLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

SpotLight2D::SpotLight2D() {
	arc = 30;
	rayCount = 3;
	drawDebug = false;
}

SpotLight2D::~SpotLight2D() {
}

void SpotLight2D::_ready() {
	queue_redraw();
}

void SpotLight2D::_process(double delta) {
	queue_redraw();
}

void SpotLight2D::_draw() {
	if(drawDebug) {
		real_t arcRad = Math::deg_to_rad(arc);
		real_t angle = get_rotation() - (arcRad / 2);
		Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 5;
		real_t distance = 10000;
		draw_line(Point2(0, 0), Point2{cos(angle), sin(angle)} * distance, color, lineWidth);
		draw_line(Point2(0, 0), Point2{cos(angle + arcRad), sin(angle + arcRad)} * distance, color, lineWidth);
		draw_circle(Point2(0, 0), 30, color, true);
		
		std::size_t numOfDisSegments = 50;
		for(std::size_t i = 0; i < numOfDisSegments; i++) {
			real_t localDistance = (distance / numOfDisSegments) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = arcRad / numOfSegments;
				real_t localAngle = angle + (segmentSpreadAngle * j);
				
				draw_line(
					Point2{cos(localAngle), sin(localAngle)} * localDistance,
					Point2{cos(localAngle + segmentSpreadAngle), sin(localAngle + segmentSpreadAngle)} * localDistance,
					color, lineWidth);
			}
		}
	}
}