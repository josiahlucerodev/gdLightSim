#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "lightInteractor2D.h"

struct Lens2DInfo {
	real_t width;
	real_t focalLength;
	real_t rotation;
	Point2 position;
};

struct Lens2D : public Node2D {
	GDCLASS(Lens2D, Node2D);
protected:
    static void _bind_methods();
private:
	bool drawDebug;
    real_t lensWidth;
	real_t focalLength;
public:
    Lens2D();
    ~Lens2D();

    void _ready() override;
    void _process(double delta) override;
    void _draw() override;

    bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
    real_t get_lens_width() const;
	void set_lens_width(const real_t);
    real_t get_focal_length() const;
	void set_focal_length(const real_t focalLength);
	Lens2DInfo getInfo() const;
};

template<>
struct LightInteractor<Lens2D> {
	using source_info = Lens2DInfo;

	using radial_shot_result = ShotScatterResult;
	using radial_generation_result = GenerateScatterSectionResult;
	using linear_shot_result = ShotScatterResult;
	using linear_generation_result = GenerateScatterSectionResult;
	using scatter_shot_result = ShotScatterResult;
	using scatter_generation_result = GenerateScatterSectionResult;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
		Shape2D shape = constructShape2D(shapeId, source.width, source.rotation, source.position);
    	shape.focalLength = source.focalLength;
    	shape.type = Shape2DType::lens;
    	return shape;
	}

	template<typename Section>
	static ShotScatterResult shotLensSection(const Section& section, LightInteractorShotInfo shotInfo) {
		const RayHit2D startRay = std::get<1>(section.startRay);
		const RayHit2D endRay = std::get<1>(section.endRay);

		const ShapeId lensShapeId = startRay.shapeId;
		const Vector2 lensMidPoint = shotInfo.shapes[lensShapeId].midPoint;
		const real_t focalLength = shotInfo.shapes[lensShapeId].focalLength;

		Ray2D startReflectRay = Ray2D{startRay.location, startRay.ray.direction};
		startReflectRay = refractRayThroughLens(lensMidPoint, focalLength, startReflectRay);
		Ray2D endReflectRay = Ray2D{endRay.location, endRay.ray.direction};
		endReflectRay =	refractRayThroughLens(lensMidPoint, focalLength, endReflectRay);
		
		std::vector<Point2> points = getPointsExcluding(lensShapeId, shotInfo.shapes);
		
		return shotScatterLight(shotInfo.generateBaseInfo(points), 
			lensShapeId, endReflectRay, startReflectRay, false);
	}

	static radial_shot_result shot(const RadialSection& radialSection, LightInteractorShotInfo shotInfo) {
		return shotLensSection<RadialSection>(radialSection, shotInfo);
	}	
	static radial_generation_result generate(const RadialSection& radialSection, radial_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {

		const RayHit2D startRay = std::get<1>(radialSection.startRay);
		const RayHit2D endRay = std::get<1>(radialSection.endRay);

		GenerateScatterSectionResult generationResult = generateScatterSections(
			generationInfo, radialSection.color, startRay, endRay, shotResult);
		for(RadialSection& radialSection : generationResult.postIntersectionSections) {
			std::swap(radialSection.startRay, radialSection.endRay);
		}
		return generationResult;
	}
	
	static linear_shot_result shot(const LinearSection& linearSection, LightInteractorShotInfo shotInfo) {
		return shotLensSection<LinearSection>(linearSection, shotInfo);
	}	
	static linear_generation_result generate(const LinearSection& linearSection, linear_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D startRay = std::get<1>(linearSection.startRay);
		const RayHit2D endRay = std::get<1>(linearSection.endRay);
		return generateScatterSections(generationInfo, linearSection.color, startRay, endRay, shotResult);
	}

	static scatter_shot_result shot(const ScatterSection& scatterSection, LightInteractorShotInfo shotInfo) {
		return shotLensSection<ScatterSection>(scatterSection, shotInfo);
	}	
	static scatter_generation_result generate(const ScatterSection& scatterSection, scatter_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D startRay = std::get<1>(scatterSection.startRay);
		const RayHit2D endRay = std::get<1>(scatterSection.endRay);
		return generateScatterSections(generationInfo, scatterSection.color, startRay, endRay, shotResult);
	}
};