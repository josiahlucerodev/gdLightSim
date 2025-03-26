#pragma once

//own
#include "lightInteractor2D.h"
#include "../lightActor2D.h"

//std
#include <algorithm>

struct ColorFilter2DInfo {
	real_t width;
	Color filterColor;
	real_t rotation;
	Point2 position;
};

class ColorFilter2D : public LightActor2D {
	GDCLASS(ColorFilter2D, LightActor2D);
protected:
    static void _bind_methods();
private:
	bool drawDebug;
	Color filterColor;
public:
    ColorFilter2D();
    ~ColorFilter2D();

    void _draw() override;

    bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
    real_t get_filter_width() const;
    Color get_filter_color() const;
	void set_filter_color(const Color filterColor);
	ColorFilter2DInfo getInfo() const;

	void debug_draw();
};

template<>
struct LightInteractor<ColorFilter2D> {
	using source_info = ColorFilter2DInfo;

	using radial_shot_result = ShotScatterResult;
	using radial_generation_result = GenerateScatterSectionResult;
	using linear_shot_result = std::vector<RayVariant>;
	using linear_generation_result = std::vector<LinearSection>;
	using scatter_shot_result = ShotScatterResult;
	using scatter_generation_result = GenerateScatterSectionResult;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
		Shape2D shape = constructShape2D(shapeId, source.width, source.rotation, source.position);
    	shape.type = Shape2DType::filter;
    	shape.filterColor = source.filterColor;
    	return shape;
	}

	static Color applyFilter(const Color& input, const Color& filter) {
		return Color(
			std::clamp(input.r * filter.r, 0.0f, 1.0f),
			std::clamp(input.g * filter.g, 0.0f, 1.0f),
			std::clamp(input.b * filter.b, 0.0f, 1.0f),
			1.0
		);
	}
	static radial_shot_result shot(const RadialSection& radialSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D& startRay = std::get<1>(radialSection.startRay);
		const RayHit2D& endRay = std::get<1>(radialSection.endRay);
		const ShapeId filterShapeId = radialSection.shapeId;
		const Ray2D filterStartRay = Ray2D{startRay.location, startRay.ray.direction};
		const Ray2D filterEndRay = Ray2D{endRay.location, endRay.ray.direction};
		std::vector<Point2> points = getPointsExcluding(radialSection.shapeId, shotInfo.shapes);
		return shotScatterLight(shotInfo.generateBaseInfo(points), filterShapeId, filterStartRay, filterEndRay, true);
	}	
	static radial_generation_result generate(const RadialSection& radialSection, radial_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D& startRay = std::get<1>(radialSection.startRay);
		const RayHit2D& endRay = std::get<1>(radialSection.endRay);
		const ShapeId originShapeId = radialSection.shapeId;
		const Color& filterColor = generationInfo.shapes[originShapeId].filterColor;
		const Color& color = applyFilter(radialSection.color, filterColor);
		return generateScatterSections(generationInfo, color, startRay, endRay, shotResult);
	}
	
	static linear_shot_result shot(const LinearSection& linearSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D& startRay = std::get<1>(linearSection.startRay);
		const RayHit2D& endRay = std::get<1>(linearSection.endRay);
		std::vector<Point2> points = getPointsExcluding(linearSection.shapeId, shotInfo.shapes);
		return shotLinearLight(shotInfo.generateBaseInfo(points), linearSection.shapeId, 
			startRay.location, endRay.location, startRay.ray.direction);
	}	
	static linear_generation_result generate(const LinearSection& linearSection, linear_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const ShapeId originShapeId = linearSection.shapeId;
		const Color& filterColor = generationInfo.shapes[originShapeId].filterColor;
		const Color& color = applyFilter(linearSection.color, filterColor);
		const RayHit2D& startRay = std::get<1>(linearSection.startRay);
		return generateLinearSections(generationInfo, color, startRay.location, shotResult);
	}

	static scatter_shot_result shot(const ScatterSection& scatterSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D& startRay = std::get<1>(scatterSection.startRay);
		const RayHit2D& endRay = std::get<1>(scatterSection.endRay);
		const ShapeId filterShapeId = scatterSection.shapeId;
		const Ray2D filterStartRay = Ray2D{startRay.location, startRay.ray.direction};
		const Ray2D filterEndRay = Ray2D{endRay.location, endRay.ray.direction};
		std::vector<Point2> points = getPointsExcluding(scatterSection.shapeId, shotInfo.shapes);
		return shotScatterLight(shotInfo.generateBaseInfo(points), filterShapeId, filterStartRay, filterEndRay, true);
	}	
	static scatter_generation_result generate(const ScatterSection& scatterSection, scatter_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D& startRay = std::get<1>(scatterSection.startRay);
		const RayHit2D& endRay = std::get<1>(scatterSection.endRay);
		const ShapeId originShapeId = scatterSection.shapeId;
		const Color& filterColor = generationInfo.shapes[originShapeId].filterColor;
		const Color& color = applyFilter(scatterSection.color, filterColor);
		return generateScatterSections(generationInfo, color, startRay, endRay, shotResult);
	}
};

