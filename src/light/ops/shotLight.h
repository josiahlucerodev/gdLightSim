#pragma once

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

//own
#include "section2D.h"
#include "../data/bvh2D.h"

using namespace godot;

struct ShotInfo {
    const std::vector<Point2>& points;
    BVH2D& bvh;
    RaySettings raySettings;
};

std::vector<RayVariant> shotLinearLight(ShotInfo shotInfo, const std::optional<ShapeId>& shapeOriginId, 
    const Point2& leftPoint, const Point2& rightPoint, const Vector2& direction);

ShotScatterResult shotScatterLight(ShotInfo shotInfo, const std::optional<ShapeId>& shapeOriginId, 
    const Ray2D& startRay, const Ray2D& endRay, const bool divergeForward);

std::vector<RayVariant> shotScatterPostIntersectionLight(
    ShotInfo shotInfo, const std::optional<ShapeId>& shapeOriginId, 
    const Ray2D& startRay, const Ray2D& endRay);