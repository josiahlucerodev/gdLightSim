#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>

using namespace godot;

//own
#include "lightInteractor2D.h"


struct LightSensor2DInfo {
    real_t rotation;
    Point2 position;
    PackedVector2Array points;
};

class LightSensor2D : public Polygon2D {
	GDCLASS(LightSensor2D, Polygon2D);

protected:
    static void _bind_methods();

private:
	bool isHit;

public:
    bool get_is_hit() const;
    void set_is_hit(const bool isHit);
    LightSensor2DInfo getInfo() const;
};

template<>
struct LightInteractor<LightSensor2D> {
	using source_info = LightSensor2DInfo;
    using no_shoting = void;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
        Shape2D shape = constructShape2D(shapeId, source.rotation, source.position, source.points);
        shape.type = Shape2DType::sensor;
        return shape;
	}
};

void checkAndSetIfHit(const BVH2D& bvh, std::unordered_map<LightSensor2D*, ShapeId>& sensorsToShapeId);
