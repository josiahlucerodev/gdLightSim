#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>

//own
#include "shape2D.h"

using namespace godot;

class LightColider2D : public Polygon2D {
	GDCLASS(LightColider2D, Polygon2D);

protected:
    static void _bind_methods();

private:
	bool isHit;

public:
    bool get_is_hit() const;
    void set_is_hit(const bool isHit);
};

Shape2D constructShape2D(LightColider2D& polygon, ShapeId shapeId);