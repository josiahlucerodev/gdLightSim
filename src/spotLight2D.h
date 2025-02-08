#pragma once

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class SpotLight2D : public Node2D {
	GDCLASS(SpotLight2D, Node);
private:
    double arc;

protected:
	static void _bind_methods();

public:

    SpotLight2D();
	~SpotLight2D();

	void _ready() override;
	void _process(double delta) override;

	double get_arc() const;
	void set_arc(const double);
};
