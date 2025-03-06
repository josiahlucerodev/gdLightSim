#include "shotLight.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "settings.h"
#include "util.h"

using namespace godot;


std::vector<RayVariant> shotLinearLight(
    const std::optional<ShapeId>& shapeOriginId, 
    const Point2& leftPoint, const Point2& rightPoint, const Vector2& direction, 
    const std::vector<Point2>& points, const BVH2D& bvh, real_t linearRaySpread) {
   
    std::vector<RayVariant> rays;
    auto testRay = [&](Ray2D ray) {
        std::optional<RayHit2D> rayHit = shotRay(ray, shapeOriginId, bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
    };
    
    std::vector<Point2> beamPoints;
    beamPoints.reserve(points.size());
    for(Point2 point : points) {
        std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, direction * -1}, leftPoint, rightPoint);
        if(rayOrigin.has_value()) {
            beamPoints.push_back(rayOrigin.value());
        }
    }
    
    {
        testRay(Ray2D{leftPoint, direction});
        testRay(Ray2D{rightPoint, direction});
    }

    for(size_t i = 0; i < beamPoints.size(); i++) {
        Point2& beamPoint = beamPoints[i];
        for(std::size_t i = 0; i < Settings::rayCount; i++) {
            real_t offset = (linearRaySpread / -2) + ((linearRaySpread / Settings::rayCount) * i);
            Vector2 spreadOrigin = beamPoint + Vector2{offset, offset};
            testRay(Ray2D{spreadOrigin, direction});
        }
    }

    return rays;
}

inline Vector2 weightedSlerp(const Vector2& v1, real_t w2, const Vector2& v2, real_t w1) {
    const real_t t = 0.5;
    real_t dot = v1.dot(v2);
    dot = std::clamp(dot, -1.0f, 1.0f);
    real_t theta = std::acos(dot);
    if (theta == 0.0f) {
        return v1;
    }
    const real_t sinTheta = std::sin(theta);
    const real_t scaleV1 = w1 * std::sin((1 - t) * theta) / sinTheta;
    const real_t scaleV2 = w2 * std::sin(t * theta) / sinTheta;
    return Vector2{scaleV1 * v1[0] + scaleV2 * v2[0], scaleV1 * v1[1] + scaleV2 * v2[1]}.normalized();
}

std::vector<RayVariant> shotScatterLight(
    const std::optional<ShapeId>& shapeOriginId, const Ray2D& startRay, const Ray2D& endRay,
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t scatterRaySpread) {

    const real_t surfuceSlope = calculateSlope(startRay.origin, endRay.origin);
	const real_t surfaceSlopeAngle = atan(surfuceSlope);
	const Vector2 surfaceSlopeDir = vectorFromAngle(surfaceSlopeAngle);
	
	std::vector<Ray2D> testRays;
	for(Point2 point : points) {
		Point2 parallelLineToPointStart = (surfaceSlopeDir * 100000) + point; 
		Point2 parallelLineToPointEnd = (-surfaceSlopeDir * 100000) + point; 
		
		auto getDistance = [&](Ray2D ray) -> real_t {
			std::optional<Point2> hit = rayLineIntersection(ray, parallelLineToPointStart, parallelLineToPointEnd);
			if(!hit.has_value()) {
				return 10000;
			}
			return hit.value().distance_to(point);
		};
		
		real_t distanceToStart = getDistance(startRay);
		real_t distanceToEnd = getDistance(endRay);
		Vector2 rayDir = weightedSlerp(startRay.direction, distanceToStart, endRay.direction, distanceToEnd);
		
		std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, rayDir * -1}, startRay.origin, endRay.origin);
		if(rayOrigin.has_value()) {
			testRays.push_back(Ray2D{rayOrigin.value(), rayDir});
		}
	}
	
	std::vector<RayVariant> rays;
	auto testRay = [&](Ray2D ray) {
		std::optional<RayHit2D> rayHit = shotRay(ray, shapeOriginId, bvh);
		if(rayHit.has_value()) {
			rays.push_back(RayVariant(rayHit.value()));
		} else {
			rays.push_back(RayVariant(ray));
		}
	};

	testRay(startRay);
	testRay(endRay);

	for(Ray2D ray : testRays) {
		Ray2D side1 = Ray2D{ray.origin + (surfaceSlopeDir * scatterRaySpread), 
			ray.direction.rotated(scatterRaySpread).normalized()};
		Ray2D side2 = Ray2D{ray.origin - (surfaceSlopeDir * scatterRaySpread), 
			ray.direction.rotated(-scatterRaySpread).normalized()};
		testRay(side2);
		testRay(ray);
		testRay(side1);
	}
	
	return rays;
}