#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "lightEmitter2D.h"
#include "spotLight2D.h"

struct CircleLight2DInfo {
	Point2 position;
	Color color;
};

class CircleLight2D : public Node2D {
	GDCLASS(CircleLight2D, Node2D);
private:
	bool drawDebug;
	Color color;
protected:
	static void _bind_methods();
public:
    CircleLight2D();
	~CircleLight2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
	Color get_color() const;
	void set_color(const Color color);
	CircleLight2DInfo getInfo() const;
};

struct ShotCircleLight2DResult {
	std::vector<RayVariant> h1Rays;
	std::vector<RayVariant> h2Rays;
};

template<>
struct LightEmitter<CircleLight2D> {
	using spot_light_emitter = LightEmitter<SpotLight2D>; 
	using source_info = CircleLight2DInfo;
	using shot_result = ShotCircleLight2DResult;
	using generation_result = std::vector<RadialSection>;

	static SpotLight2DInfo getH1Info(const source_info& source) {
		SpotLight2DInfo h1Info;
		h1Info.color = source.color;
		h1Info.position = source.position;
		h1Info.arc = Math_PI;
		h1Info.rotation = 0;
		return h1Info;
	}
	static SpotLight2DInfo getH2Info(const source_info& source) {
		SpotLight2DInfo h2Info;
		h2Info.color = source.color;
		h2Info.position = source.position;
		h2Info.arc = Math_PI;
		h2Info.rotation = Math_PI;
		return h2Info;
	}

	static shot_result shot(const source_info& source, EmitterShotInfo shotInfo) {
		const SpotLight2DInfo h1Info = getH1Info(source);
		const SpotLight2DInfo h2Info = getH2Info(source);

		ShotCircleLight2DResult result;
		result.h1Rays = spot_light_emitter::shot(h1Info, shotInfo);
		result.h2Rays = spot_light_emitter::shot(h2Info, shotInfo);
		return result;
	} 
	static generation_result generate(const source_info& source, shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const SpotLight2DInfo h1Info = getH1Info(source);
		const SpotLight2DInfo h2Info = getH2Info(source);
		
		generation_result result = spot_light_emitter::generate(h1Info, shotResult.h1Rays, generationInfo);
		generation_result h2Result = spot_light_emitter::generate(h2Info, shotResult.h2Rays, generationInfo);
		result.insert(result.end(), h2Result.begin(), h2Result.end());
		return result;
	}
};