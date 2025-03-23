#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
using namespace godot;

//own
#include "../ops/section2D.h"

void drawSection(Node2D& node, const RadialSection& radialSection);
void drawSection(Node2D& node, const LinearSection& linearSection);
void drawSection(Node2D& node, const ScatterSection& scatterSection);