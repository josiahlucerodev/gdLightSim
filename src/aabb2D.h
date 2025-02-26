#pragma once

//godotcpp
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

struct AABB2D {
    Point2 min;
    Point2 max;
};

AABB2D updateAABB(const AABB2D rhs, const AABB2D lhs);