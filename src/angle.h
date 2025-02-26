#pragma once 

//godotcpp
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

inline real_t clockwiseAngle(Vector2 origin, Vector2 dir) {
    Vector2 translatedDir = dir - origin;
    if(translatedDir.y < 0) {
        return Math_PI * 2 - abs(translatedDir.angle());
    } else {
        return abs(translatedDir.angle());
    }
} 

inline real_t clockwiseAngle(Vector2 dir) {
    return clockwiseAngle(Vector2{0, 0}, dir);
}