#pragma once

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class LightEnvironment2D : public Node2D {
	GDCLASS(LightEnvironment2D, Node);
protected:
	static void _bind_methods();

public:

    LightEnvironment2D();
	~LightEnvironment2D();

	void _ready() override;
	void _process(double delta) override;
};
