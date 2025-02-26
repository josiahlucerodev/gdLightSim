#pragma once

//godotcpp
#include <godot_cpp/variant/vector2.hpp>

//std
#include <variant>
#include <optional>

//own
#include "aabb2D.h"

using namespace godot;

struct Ray2D{
    Point2 origin;
    Point2 direction;
};

struct RayHit2D {
    std::size_t shapeId;
    real_t angle;  
    Ray2D ray;
    Point2 location;
};

using RayVariant = std::variant<Ray2D, RayHit2D>;

bool isRayIntersectsAABB(const Ray2D& ray, const AABB2D& aabb);
std::optional<Point2> rayLineIntersection(const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd);
std::optional<RayHit2D> rayLineHit(std::size_t shapeId, const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd);
const Ray2D& getRay(const RayVariant& rayVariant);