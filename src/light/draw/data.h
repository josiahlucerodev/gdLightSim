#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
using namespace godot;

//own
#include "../data/bvh2D.h"


void drawAABB(Node2D& node, AABB2D aabb, Color color);
void drawShapePoint(Node2D& node, const Point2& point);
void drawMidPoint(Node2D& node, const Point2& point);
void drawBVH2D(Node2D& node, BVH2D& bvh);
void drawBVH2DMidpoints(Node2D& node, BVH2D& bvh);