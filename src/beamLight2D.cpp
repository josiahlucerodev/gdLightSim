#include "beamLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "settings.h"
#include "util.h"
#include "shotLight.h"

using namespace godot;

void BeamLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &BeamLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &BeamLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
		
	ClassDB::bind_method(D_METHOD("get_light_width"), &BeamLight2D::get_light_width);
	ClassDB::bind_method(D_METHOD("set_light_width", "light_width"), &BeamLight2D::set_light_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "light_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_light_width", "get_light_width");
	ClassDB::bind_method(D_METHOD("get_color"), &BeamLight2D::get_color);
	ClassDB::bind_method(D_METHOD("set_color", "color"), &BeamLight2D::set_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
}

real_t BeamLight2D::get_light_width() const {
	return lightWidth;
}
void BeamLight2D::set_light_width(const real_t lightWidth) {
	this->lightWidth = lightWidth;
}

bool BeamLight2D::get_draw_debug() const {
	return drawDebug;
}
void BeamLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}
Color BeamLight2D::get_color() const {
	return color;
}
void BeamLight2D::set_color(const Color color) {
	this->color = color;
}

BeamLight2D::BeamLight2D() {
	lightWidth = 10;
	drawDebug = false;
	color = Settings::defaultLightColor;
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
        real_t r = get_rotation();	
        real_t piOver2 = Math_PI / 2;
        Point2 rightOffset = Point2(0, -1) * (lightWidth / 2);
        Point2 leftOffset = Point2(0, 1) * (lightWidth / 2);

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

std::vector<RayVariant> shotBeamLight2D(
	const BeamLight2D& beamLight, const std::vector<Point2>& points, 
	BVH2D& bvh, real_t linearRaySpread) {
	Point2 beamLightLocation = beamLight.get_position();
	real_t beamLightWidth = beamLight.get_light_width();
	real_t beamLightRotation = beamLight.get_rotation();

	Vector2 beamDirection = vectorFromAngle(beamLightRotation);
	real_t piOver2 = Math_PI / 2;
	Vector2 rightBeamPoint = beamLightLocation + (vectorFromAngle(beamLightRotation + piOver2) * (beamLightWidth / 2));
	Vector2 leftBeamPoint = beamLightLocation + (vectorFromAngle(beamLightRotation - piOver2) * (beamLightWidth / 2));

	return shotLinearLight({}, leftBeamPoint, rightBeamPoint, beamDirection, points, bvh, linearRaySpread);
}

std::vector<LinearSection> generateBeamLight2DSections(
	const BeamLight2D& beamLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance) {
	Point2 beamLightLocation = beamLight.get_position();
	real_t beamLightWidth = beamLight.get_light_width();
	real_t beamLightRotation = beamLight.get_rotation();

	Vector2 beamDirection = vectorFromAngle(beamLightRotation);
	real_t piOver2 = Math_PI / 2;
	Vector2 rightBeamPoint = beamLightLocation + (vectorFromAngle(beamLightRotation + piOver2) * (beamLightWidth / 2));

	return generateLinearSections(beamLight.get_color(), rightBeamPoint, rays, shapes, linearSectionTolerance);
}