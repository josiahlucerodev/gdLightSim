#pragma once

//std
#include <vector>

//own
#include "lightEmitter2D.h"
#include "../ops/shotLight.h"
#include "../ops/util.h"
#include "../lightActor2D.h"

struct BeamLight2DInfo {
	real_t width;
	real_t rotation;
	Point2 position;
	Color color;
};

struct BeamLight2D : public LightActor2D {
	GDCLASS(BeamLight2D, LightActor2D);
private:
	bool drawDebug;
	Color lightColor;
protected:
	static void _bind_methods();

public:
    BeamLight2D();
	~BeamLight2D();

	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
	real_t get_light_width() const;
	void set_light_width(const real_t lightWidth);
	Color get_light_color() const;
	void set_light_color(const Color lightColor);
	BeamLight2DInfo getInfo();

	void debug_draw();
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