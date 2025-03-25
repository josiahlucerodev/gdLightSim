#include "lightActor2D.h"

//binding
void LightActor2D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_light_actor_type"), &LightActor2D::_godot_get_light_actor_type);
	ClassDB::bind_method(D_METHOD("set_light_actor_type", "actor_type"), &LightActor2D::_godot_set_light_actor_type);
    const String enumElements = "beamLight, circleLight, spotLight, colorFilter, lens, mirror, lightCollider, lightSensor"; 
    //ADD_PROPERTY(PropertyInfo(Variant::INT, "actor_type", PROPERTY_HINT_ENUM, enumElements), "unknown, set_light_actor_type", "get_light_actor_type");
}

//constructor/destructor
LightActor2D::LightActor2D() {
    set_light_actor_type(LightActor2DType::unknown);
}
LightActor2D::~LightActor2D() {
}

//getter/setters
void LightActor2D::set_light_actor_type(LightActor2DType actorType) {
    this->type = actorType;
}
LightActor2DType LightActor2D::get_light_actor_type() const {
    return type;
}
void LightActor2D::_godot_set_light_actor_type(int actorType) {
    type = static_cast<LightActor2DType>(actorType);
}
int LightActor2D::_godot_get_light_actor_type() const {
    return static_cast<int>(type);
}