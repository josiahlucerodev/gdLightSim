#include "lightSensor2D.h"

//binding
void LightSensor2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_is_hit"), &LightSensor2D::get_is_hit);
    ClassDB::bind_method(D_METHOD("set_is_hit", "is_hit"), &LightSensor2D::set_is_hit);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_hit"), "set_is_hit", "get_is_hit");
}

//getter/setter
bool LightSensor2D::get_is_hit() const {
	return isHit;
}
void LightSensor2D::set_is_hit(const bool isHit) {
	this->isHit = isHit;
}

LightSensor2DInfo LightSensor2D::getInfo() const {
    const Transform2D globalTransform = get_global_transform();
    auto points = get_polygon();
    for(Point2& point : points) {
        point = globalTransform.xform(point);
    }

    LightSensor2DInfo info;
    info.points = points;
    return info;
}

//constructor/deconstructor
LightSensor2D::LightSensor2D() {
	set_light_actor_type(LightActor2DType::lightSensor);
	isHit = false;
}
LightSensor2D::~LightSensor2D() {
}

void checkAndSetIfHit(const BVH2D& bvh, std::unordered_map<LightSensor2D*, ShapeId>& sensorsToShapeId) {
	for(auto [sensor, id] : sensorsToShapeId) {
		sensor->set_is_hit(bvh.shapeIsHit[id]);
	}
}