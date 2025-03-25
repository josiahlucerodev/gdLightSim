#pragma once

//own
#include "lightInteractor2D.h"
#include "../lightActor2D.h"

struct LightSensor2DInfo {
    PackedVector2Array points;
};

class LightSensor2D : public LightActor2D {
	GDCLASS(LightSensor2D, LightActor2D);

protected:
    static void _bind_methods();

private:
	bool isHit;

public:
    LightSensor2D();
    ~LightSensor2D();

    bool get_is_hit() const;
    void set_is_hit(const bool isHit);
    LightSensor2DInfo getInfo() const;
};

template<>
struct LightInteractor<LightSensor2D> {
	using source_info = LightSensor2DInfo;
    using no_shoting = void;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
        Shape2D shape = constructShape2D(shapeId, source.points);
        shape.type = Shape2DType::sensor;
        return shape;
	}
};

void checkAndSetIfHit(const BVH2D& bvh, std::unordered_map<LightSensor2D*, ShapeId>& sensorsToShapeId);
