#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
using namespace godot;

enum LightActor2DType {
    unknown,
    beamLight,
    circleLight,
    spotLight,
    colorFilter,
    lens,
    mirror,
    lightCollider,
    lightSensor,
};


struct LightActor2D : public Polygon2D {
	GDCLASS(LightActor2D, Polygon2D);

protected:
    static void _bind_methods();

private:
    LightActor2DType type;

public:
    LightActor2D();
	virtual ~LightActor2D();

    void set_light_actor_type(LightActor2DType actorType);
    LightActor2DType get_light_actor_type() const;
    void _godot_set_light_actor_type(int actorType);
    int _godot_get_light_actor_type() const;
};
