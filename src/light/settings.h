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
    static inline const Color defaultLightColor = Color{1.0, 1.0, 0.0};
    static inline const Color debugFilledLightColor = Color{1.0, 1.0, 0.0};
    static inline const Color debugLightEmitterColor = Color{0.0, 0.0, 1.0};
    static inline const Color bvhColor = Color{1.0, 1.0, 1.0};
    static inline const Color defaultObjectColor = Color{1.0, 1.0, 1.0};
    static inline const real_t pointRadius = 10;
    static inline const real_t debugLineWidth = 1;
    static inline const std::size_t debugDistance = 10000;
    static inline const std::size_t debugSegmentCount = 50;
    static inline const std::size_t rayCount = 3;
    static inline const real_t arcSegmentLimit = Math_PI / 3;
};