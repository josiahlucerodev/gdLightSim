#pragma once 

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

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