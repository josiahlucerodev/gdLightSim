#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "lightInteractor2D.h"
#include "../ops/util.h"

struct Mirror2DInfo {
	real_t width;
	real_t rotation;
	Point2 position;
};

class Mirror2D : public Node2D {
	GDCLASS(Mirror2D, Node2D);
private:
	bool drawDebug;
    real_t mirrorWidth;
protected:
	static void _bind_methods();
public:
    Mirror2D();
	~Mirror2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;

	bool get_draw_debug() const;
	void set_draw_debug(const bool);
    real_t get_mirror_width() const;
	void set_mirror_width(const real_t);
	Mirror2DInfo getInfo() const;
};

template<>
struct LightInteractor<Mirror2D> {
	using source_info = Mirror2DInfo;

	using radial_shot_result = ShotScatterResult;
	using radial_generation_result = GenerateScatterSectionResult;
	using linear_shot_result = std::vector<RayVariant>;
	using linear_generation_result = std::vector<LinearSection>;
	using scatter_shot_result = ShotScatterResult;
	using scatter_generation_result = GenerateScatterSectionResult;

	static Shape2D constructShape(const source_info& source, const ShapeId shapeId) {
		Shape2D shape = constructShape2D(shapeId, source.width, source.rotation, source.position);
    	shape.type = Shape2DType::mirror;
    	return shape;
	}

	static radial_shot_result shot(const RadialSection& radialSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D startRay = std::get<1>(radialSection.startRay);
		const RayHit2D endRay = std::get<1>(radialSection.endRay);
		const ShapeId mirrorShapeId = startRay.shapeId;
		const Point2 hitMidLocation = (startRay.location + endRay.location) / 2;
		const real_t sectionHitWidth = startRay.location.distance_to(endRay.location);
		const real_t mirrorRotation = clockwiseAngle(hitMidLocation, startRay.location);
		const Ray2D startReflectRay = Ray2D{startRay.location, vectorFromAngle(getRayReflectionAngle(startRay.angle, mirrorRotation)) };
		const Ray2D endReflectRay = Ray2D{endRay.location, vectorFromAngle(getRayReflectionAngle(endRay.angle, mirrorRotation))};
		std::vector<Point2> points = getPointsExcluding(mirrorShapeId, shotInfo.shapes);
		return shotScatterLight(shotInfo.generateBaseInfo(points), mirrorShapeId, startReflectRay, endReflectRay, true);
	}	
	static radial_generation_result generate(const RadialSection& radialSection, radial_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D startRay = std::get<1>(radialSection.startRay);
		const RayHit2D endRay = std::get<1>(radialSection.endRay);
		return generateScatterSections(generationInfo, radialSection.color, startRay, endRay, shotResult);
	}
	
	static linear_shot_result shot(const LinearSection& linearSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D startRay = std::get<1>(linearSection.startRay);
		const RayHit2D endRay = std::get<1>(linearSection.endRay);
		const Point2 bounceMidLocation = (startRay.location + endRay.location) / 2;
		const real_t mirrorRotation = clockwiseAngle(bounceMidLocation, startRay.location);
		const real_t reflectAngle = getRayReflectionAngle(startRay.angle, mirrorRotation);
		const Vector2 reflectDirection = vectorFromAngle(reflectAngle); 
		std::vector<Point2> points = getPointsExcluding(startRay.shapeId, shotInfo.shapes);
		return shotLinearLight(shotInfo.generateBaseInfo(points), linearSection.shapeId, 
			startRay.location, endRay.location, reflectDirection);
	}	
	static linear_generation_result generate(const LinearSection& linearSection, linear_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D startRay = std::get<1>(linearSection.startRay);
		return generateLinearSections(generationInfo, linearSection.color, startRay.ray.origin, shotResult);
	}

	static scatter_shot_result shot(const ScatterSection& scatterSection, LightInteractorShotInfo shotInfo) {
		const RayHit2D startRay = std::get<1>(scatterSection.startRay);
		const RayHit2D endRay = std::get<1>(scatterSection.endRay);
		const ShapeId mirrorShapeId = startRay.shapeId;
		const Point2 hitMidLocation = (startRay.location + endRay.location) / 2;
		const real_t sectionHitWidth = startRay.location.distance_to(endRay.location);
		const real_t mirrorRotation = clockwiseAngle(hitMidLocation, startRay.location);
		const Ray2D startReflectRay = Ray2D{startRay.location, vectorFromAngle(getRayReflectionAngle(startRay.angle, mirrorRotation)) };
		const Ray2D endReflectRay = Ray2D{endRay.location, vectorFromAngle(getRayReflectionAngle(endRay.angle, mirrorRotation))};
		std::vector<Point2> points = getPointsExcluding(mirrorShapeId, shotInfo.shapes);
		return shotScatterLight(shotInfo.generateBaseInfo(points), mirrorShapeId, startReflectRay, endReflectRay, true);
	}	
	static scatter_generation_result generate(const ScatterSection& scatterSection, scatter_shot_result& shotResult, 
		SectionGenerationInfo generationInfo) {
		const RayHit2D startRay = std::get<1>(scatterSection.startRay);
		const RayHit2D endRay = std::get<1>(scatterSection.endRay);
		return generateScatterSections(generationInfo, scatterSection.color, startRay, endRay, shotResult);
	}
};