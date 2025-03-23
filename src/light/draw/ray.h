#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
using namespace godot;

//own
#include "../ops/ray2D.h"

void drawRay(Node2D& node, const Ray2D& ray);
void drawRay(Node2D& node, const RayHit2D& rayHit);
void drawRay(Node2D& node, const RayVariant& rayVaraint);