#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class Mirror2D : public Node2D {
	GDCLASS(Mirror2D, Node2D);
private:
    double mirrorWidth;
    int64_t rayCount;
	bool drawDebug;
protected:
	static void _bind_methods();
public:
    Mirror2D();
	~Mirror2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

    double get_mirror_width() const;
	void set_mirror_width(const double);
	int64_t get_ray_count() const;
	void set_ray_count(const int64_t);
	bool get_draw_debug() const;
	void set_draw_debug(const bool);
};
