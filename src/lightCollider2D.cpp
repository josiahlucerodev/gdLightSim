#include "lightCollider2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

void LightColider2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_is_hit"), &LightColider2D::get_is_hit);
    ClassDB::bind_method(D_METHOD("set_is_hit", "is_hit"), &LightColider2D::set_is_hit);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_hit"), "set_is_hit", "get_is_hit");
}

bool LightColider2D::get_is_hit() const {
	return isHit;
}
void LightColider2D::set_is_hit(const bool isHit) {
	this->isHit = isHit;
}

Shape2D constructShape2D(LightColider2D& polygon, ShapeId shapeId) {
    PackedVector2Array polygonPoints = polygon.get_polygon();
    real_t rotation = polygon.get_rotation();
    if(polygonPoints.size() == 0) {
        return Shape2D{};
    }

    for(std::size_t i = 0; i < polygonPoints.size(); i++) {
        polygonPoints[i] = polygonPoints[i].rotated(rotation);
    }
    
    Shape2D shape;
    shape.type = Shape2DType::wall;
    shape.shapeId = shapeId;
    shape.points.reserve(polygonPoints.size());

    Point2 point = polygon.get_transform().get_origin() + polygonPoints[0];
    shape.midPoint = Point2(0, 0);
    shape.aabb = AABB2D{
        Point2{point.x, point.y},
        Point2{point.x, point.y}
    };
    
    for(std::size_t i = 0; i < polygonPoints.size(); i++) {
        Point2 point = polygonPoints[i] + polygon.get_transform().get_origin();
        shape.aabb = updateAABB(shape.aabb, {Point2(point.x, point.y), Point2(point.x, point.y)});
        shape.midPoint += point;
        shape.points.push_back(point);
    }
    shape.midPoint /= polygonPoints.size();
    return shape;
}