#pragma once

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"

using namespace godot;

std::vector<RayVariant> shotLinearLight(
    const std::optional<ShapeId>& shapeOriginId, 
    const Point2& leftPoint, const Point2& rightPoint, const Vector2& direction, 
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t linearRaySpread);

ShotScatterReturn shotScatterLight(
    const std::optional<ShapeId>& shapeOriginId, const Ray2D& startRay, const Ray2D& endRay,
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t scatterRaySpread);

std::vector<RayVariant> shotScatterSecondRadialLight(
    const std::optional<ShapeId>& shapeOriginId, const Ray2D& startRay, const Ray2D& endRay,
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t radialRaySpread);