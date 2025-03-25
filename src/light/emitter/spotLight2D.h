#pragma once

//own
#include "lightEmitter2D.h"
#include "../lightActor2D.h"

struct SpotLight2DInfo {
	real_t arc;
	real_t rotation;
	Point2 position;
	Color color;
};

struct SpotLight2D : public LightActor2D {
private:
	GDCLASS(SpotLight2D, LightActor2D);

private:
    double arc;
	bool drawDebug;
	Color lightColor;

protected:
	static void _bind_methods();

public:

    SpotLight2D();
	~SpotLight2D();

	void _draw() override;

	double get_arc() const;
	void set_arc(const double);
	bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
	Color get_light_color() const;
	void set_light_color(const Color lightColor);
	SpotLight2DInfo getInfo() const;
	void debug_draw();
};

std::vector<RayVariant> shotSpotLight2D(const SpotLight2DInfo& source, EmitterShotInfo& shotInfo);

template<>
struct LightEmitter<SpotLight2D> {
	using source_info = SpotLight2DInfo;
	using shot_result = std::vector<RayVariant>;
	using generation_result = std::vector<RadialSection>;

	static shot_result shot(const source_info& source, EmitterShotInfo& shotInfo) {
		return shotSpotLight2D(source, shotInfo);
	} 
	static generation_result generate(const source_info& source, shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		return generateRadialSections(generationInfo, source.color, source.rotation, shotResult);
	}
};