#pragma once 

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/polygon2d.hpp>

//own
#include "aabb2D.h"

enum struct Shape2DType {
    unknown,
    wall,
    mirror,
    filter,
    lens,
};

using ShapeId = std::size_t;

struct Shape2D {
    Shape2DType type;
    ShapeId shapeId;
    Color filterColor;
    Point2 midPoint;
    real_t focalLength;
    AABB2D aabb;
    std::vector<Point2> points;
};


Shape2D constructShape2D(Polygon2D& polygon, ShapeId shapeId);
std::vector<Point2> getPoints(const std::vector<Shape2D>& shapes);
std::vector<Point2> getPointsExcluding(ShapeId excludeId, const std::vector<Shape2D>& shapes);