#pragma once

//std
#include <unordered_map>

//own
#include "lightInteractor2D.h"
#include "../lightActor2D.h"

struct LightColider2DInfo {
    PackedVector2Array points;
};

struct LightColider2D : public LightActor2D {
	GDCLASS(LightColider2D, LightActor2D);

protected:
    static void _bind_methods();

private:
	bool isHit;

public:
    LightColider2D();
    ~LightColider2D();

    bool get_is_hit() const;
    void set_is_hit(const bool isHit);
    LightColider2DInfo getInfo() const;
};

template<>
struct LightInteractor<LightColider2D> {
	using source_info = LightColider2DInfo;
    using no_shoting = void;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
        Shape2D shape = constructShape2D(shapeId, source.points);
        shape.type = Shape2DType::wall;
        return shape;
	}
};

void checkAndSetIfHit(const SectionsInfo& sectionsInfo, const std::unordered_map<ShapeId, LightColider2D*>& shapeIdToColider);

