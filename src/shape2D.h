#pragma once 

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/polygon2d.hpp>

//own
#include "aabb2D.h"
#include "mirror2D.h"

enum struct Shape2DType {
    unknown,
    wall,
    mirror
};

struct Shape2D {
    Shape2DType type;
    std::size_t shapeId;
    Point2 midPoint;
    AABB2D aabb;
    std::vector<Point2> points;
};


Shape2D constructShape2D(Polygon2D& polygon, std::size_t shapeId);
Shape2D constructShape2D(Mirror2D& mirror, std::size_t shapeId);