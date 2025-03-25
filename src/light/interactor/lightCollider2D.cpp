#include "lightCollider2D.h"

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//bindings
void LightColider2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_is_hit"), &LightColider2D::get_is_hit);
    ClassDB::bind_method(D_METHOD("set_is_hit", "is_hit"), &LightColider2D::set_is_hit);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_hit"), "set_is_hit", "get_is_hit");
}

//constructor/deconstructor
LightColider2D::LightColider2D() {
	set_light_actor_type(LightActor2DType::lightCollider);
	isHit = false;
}
LightColider2D::~LightColider2D() {
}

//getter/setters
bool LightColider2D::get_is_hit() const {
	return isHit;
}
void LightColider2D::set_is_hit(const bool isHit) {
	this->isHit = isHit;
}
LightColider2DInfo LightColider2D::getInfo() const {
    LightColider2DInfo info;
    info.points = get_polygon();
	info.position = get_global_position();
	info.rotation = get_global_rotation();
    return info;
}

void updateColliderIfHit(const std::unordered_map<ShapeId, LightColider2D*>& shapeIdToColider, ShapeId shapeId) {
    if(shapeId == 0) {
        return;
    }
    auto findIter = shapeIdToColider.find(shapeId);
    if(findIter == shapeIdToColider.end()) {
        return;
    }
    findIter->second->set_is_hit(true);
}

void checkAndSetIfHit(const SectionsInfo& sectionsInfo, const std::unordered_map<ShapeId, LightColider2D*>& shapeIdToColider) {
	for(auto [id, colider] : shapeIdToColider) {
		colider->set_is_hit(false);
	}

    for(const LinearSection& linearSection : sectionsInfo.linearSections) {
        updateColliderIfHit(shapeIdToColider, linearSection.shapeId);
    }
    for(const RadialSection& radialSection : sectionsInfo.radialSections) {
        updateColliderIfHit(shapeIdToColider, radialSection.shapeId);
    }
    for(const ScatterSection& scatterSection : sectionsInfo.scatterSections) {
        updateColliderIfHit(shapeIdToColider, scatterSection.shapeId);
    }
}
