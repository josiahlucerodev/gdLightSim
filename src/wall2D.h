#pragma once

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class Wall2D : public Node2D {
	GDCLASS(Wall2D, Node);
protected:
	static void _bind_methods();

public:

    Wall2D();
	~Wall2D();

	void _ready() override;
	void _process(double delta) override;
};
