#pragma once 

//godotcpp
#include <godot_cpp/core/math.hpp>

struct RaySettings {
    real_t radialRaySpread;
    real_t radialSectionTolerance;
    real_t linearRaySpread;
    real_t linearSectionTolerance;
    real_t scatterRaySpread;
    real_t scatterSectionTolerance;
};
