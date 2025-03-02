#pragma once 

//godotcpp
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

struct Settings {
public:
    static inline const Color rayMissColor = Color{1.0, 0.0, 0.0};
    static inline const Color rayHitColor = Color{0.0, 1.0, 0.0};
    static inline const Color shapePointColor = Color{0.0, 1.0, 1.0};
    static inline const Color midPointColor = Color{1.0, 1.0, 0.0};
    static inline const Color aabbColor = Color{1.0, 0.0, 0.0};
    static inline const Color debugLightColor = Color{1.0, 1.0, 0.0};
    static inline const Color bvhColor = Color{1.0, 1.0, 1.0};
    static inline const real_t pointRadius = 10;
    static inline const real_t debugLineWidth = 5;
    static inline const std::size_t debugDistance = 10000;
    static inline const std::size_t debugSegmentCount = 50;
};