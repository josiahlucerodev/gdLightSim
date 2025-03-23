#pragma once 

//std
#include <algorithm>
#include <optional>

//godotcpp
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

//own
#include "ray2D.h"

inline real_t clockwiseAngle(Vector2 origin, Vector2 dir) {
    Vector2 translatedDir = dir - origin;
    real_t angle = translatedDir.angle();
    if (angle< 0) {
        angle += 2 * Math_PI;
    }
    return angle;
    /*
    if(translatedDir.y < 0) {
        return Math_PI * 2 - abs(translatedDir.angle());
    } else {
        return abs(translatedDir.angle());
    }
    */
} 

inline real_t clockwiseAngle(Vector2 dir) {
    return clockwiseAngle(Vector2{0, 0}, dir);
}

inline bool isRelativelyEqual(real_t a, real_t b, real_t epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

inline Vector2 vectorFromAngle(real_t angle) {
    return Vector2{cos(angle), sin(angle)};
}

inline real_t calculateSlope(const Point2& p1, const Point2& p2) {
    return (p2.y - p1.y) / (p2.x - p1.x);
};     

inline Vector2 weightedSlerp(const Vector2& v1, real_t w2, const Vector2& v2, real_t w1) {
    const real_t t = 0.5;
    real_t dot = v1.dot(v2);
    dot = std::clamp(dot, -1.0f, 1.0f);
    real_t theta = std::acos(dot);
    if (theta == 0.0f) {
        return v1;
    }
    const real_t sinTheta = std::sin(theta);
    const real_t scaleV1 = w1 * std::sin((1 - t) * theta) / sinTheta;
    const real_t scaleV2 = w2 * std::sin(t * theta) / sinTheta;
    return Vector2{scaleV1 * v1[0] + scaleV2 * v2[0], scaleV1 * v1[1] + scaleV2 * v2[1]}.normalized();
}

inline Vector2 getDirectionVectorToFrom(const Vector2 &fromPoint, const Vector2 &toPoint) {
    const Vector2 direction = toPoint - fromPoint;
    return direction.normalized();
}

inline std::optional<Vector2> findRayIntersection(Ray2D ray1, Ray2D ray2) {
    const real_t crossProduct = ray1.direction.cross(ray2.direction);
    if (crossProduct == 0) {
        return std::nullopt;
    }
    const Vector2 delta = ray2.origin - ray1.origin;
    const real_t t1 = (delta.x * ray2.direction.y - delta.y * ray2.direction.x) / crossProduct;
    const Vector2 intersection = ray1.origin + ray1.direction * t1;
    if (t1 < 0) {
        return std::nullopt;
    }
    return intersection;
}

inline std::optional<Vector2> fineLineIntersection(const Vector2& p1, const Vector2& p2, const Vector2& q1, const Vector2& q2) {
    std::optional<Point2> intersection = rayLineIntersection(Ray2D{p1, p1.direction_to(p2)}, q1, q2);
    if(intersection.has_value() && intersection.value().distance_to(p2) < p1.distance_to(p2)) {
        return intersection;
    }
    return std::nullopt;
}