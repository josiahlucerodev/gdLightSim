#pragma once

//std
#include <vector>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "lightEmitter2D.h"
#include "../ops/shotLight.h"
#include "../ops/util.h"

struct BeamLight2DInfo {
	real_t width;
	real_t rotation;
	Point2 position;
	Color color;
};

struct BeamLight2D : public Node2D {
	GDCLASS(BeamLight2D, Node2D);
private:
	bool drawDebug;
    double lightWidth;
	Color color;
protected:
	static void _bind_methods();

public:
    BeamLight2D();
	~BeamLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
	real_t get_light_width() const;
	void set_light_width(const real_t lightWidth);
	Color get_color() const;
	void set_color(const Color color);
	BeamLight2DInfo getInfo();
};

template<>
struct LightEmitter<BeamLight2D> {
	using source_info = BeamLight2DInfo;
	using shot_result = std::vector<RayVariant>;
	using generation_result = std::vector<LinearSection>;

	static shot_result shot(const source_info& source, EmitterShotInfo shotInfo) {
		constexpr real_t piOver2 = Math_PI / 2;
		const Vector2 direction = vectorFromAngle(source.rotation);
		const Vector2 rightPoint = source.position + (vectorFromAngle(source.rotation + piOver2) * (source.width / 2));
		const Vector2 leftPoint = source.position + (vectorFromAngle(source.rotation - piOver2) * (source.width / 2));
		return shotLinearLight(shotInfo, {}, leftPoint, rightPoint, direction);
	} 
	static generation_result generate(const source_info& source, shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		constexpr real_t piOver2 = Math_PI / 2;
		const Vector2 rightPoint = source.position + (vectorFromAngle(source.rotation + piOver2) * (source.width / 2));
		return generateLinearSections(generationInfo, source.color, rightPoint, shotResult);
	}
};