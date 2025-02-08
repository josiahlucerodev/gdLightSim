#include "spotLight2D.h"

//godot_cpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void SpotLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_arc"), &SpotLight2D::get_arc);
	ClassDB::bind_method(D_METHOD("set_arc", "arc"), &SpotLight2D::set_arc);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "arc", PROPERTY_HINT_RANGE, "1,20,0.01"), "set_arc", "get_arc");
}

SpotLight2D::SpotLight2D() {
}

SpotLight2D::~SpotLight2D() {
}

void SpotLight2D::_ready() {
	
}

void SpotLight2D::_process(double delta) {
}

double SpotLight2D::get_arc() const {
	return arc;
}
void SpotLight2D::set_arc(const double arc) {
	this->arc = arc;
}

