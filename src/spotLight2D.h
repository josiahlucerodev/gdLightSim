#pragma once

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class SpotLight2D : public Node2D {
	GDCLASS(SpotLight2D, Node2D);
private:
    double arc;
	int64_t rayCount;
	bool drawDebug;
protected:
	static void _bind_methods();

public:

    SpotLight2D();
	~SpotLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	double get_arc() const;
	void set_arc(const double);
	int64_t get_ray_count() const;
	void set_ray_count(const int64_t);
	bool get_draw_debug() const;
	void set_draw_debug(const bool);
};
