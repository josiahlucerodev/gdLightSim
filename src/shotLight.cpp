#include "shotLight.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

//own
#include "settings.h"
#include "util.h"
#include "spotLight2D.h"

using namespace godot;

bool areRayLocationsRelativelyClose(const Point2& a, const Point2& b, const real_t& distanceThreshold) {
    return a.distance_to(b) <= distanceThreshold;
}

std::vector<RayVariant> shotLinearLight(
    const std::optional<ShapeId>& shapeOriginId, 
    const Point2& startPoint, const Point2& endPoint, const Vector2& direction, 
    const std::vector<Point2>& points, const BVH2D& bvh, real_t linearRaySpread) {
   
    std::vector<Point2> testPoints;
    testPoints.reserve(points.size());
    for(Point2 point : points) {
            std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, direction * -1}, startPoint, endPoint);
            if(rayOrigin.has_value()) {
                testPoints.push_back(rayOrigin.value());
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

    const Ray2D startRay = Ray2D{startPoint, direction};
    const Ray2D endRay = Ray2D{endPoint, direction};
    testRay(startRay);
    testRay(endRay);
    
    const real_t surfuceSlope = calculateSlope(startRay.origin, endRay.origin);
	const real_t surfaceSlopeAngle = atan(surfuceSlope);
	const Vector2 surfaceSlopeDir = vectorFromAngle(surfaceSlopeAngle);

    const Vector2 leftOffset = surfaceSlopeDir * linearRaySpread;
    const Vector2 rightOffset = -surfaceSlopeDir * linearRaySpread;

    for(size_t i = 0; i < testPoints.size(); i++) {
        const Point2& point = testPoints[i];
        
        const Point2 leftPoint = point + leftOffset;
        const Point2 rightPoint = point + rightOffset;
        
        const Ray2D leftRay = Ray2D{leftPoint, direction};
        const Ray2D middleRay = Ray2D{point, direction};
        const Ray2D rightRay = Ray2D{rightPoint, direction};

        testRay(leftRay);
        testRay(middleRay);
        testRay(rightRay);
        /*
        shotAndAddRaysAtPoint(rays, shapeOriginId, bvh, linearRaySpread,
        point, leftRay, middleRay, rightRay);
        */
    }
    
    return rays;
}

ShotScatterReturn shotScatterLight(
    const std::optional<ShapeId>& shapeOriginId, const Ray2D& startRay, const Ray2D& endRay,
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t scatterRaySpread) {

    const real_t surfuceSlope = calculateSlope(startRay.origin, endRay.origin);
	const real_t surfaceSlopeAngle = atan(surfuceSlope);
	const Vector2 surfaceSlopeDir = vectorFromAngle(surfaceSlopeAngle);
	
    const real_t intersectTestLineDistance = 1000000;
    const Point2 intersectTestLineStart = startRay.origin 
        + (startRay.direction * intersectTestLineDistance); 
    const Point2 intersectTestLineEnd = startRay.origin 
        + (startRay.direction * -intersectTestLineDistance);

    const Ray2D backwardEndRay = Ray2D{endRay.origin, -endRay.direction };
    const std::optional<Point2> convergePoint = rayLineIntersection(endRay, intersectTestLineStart, intersectTestLineEnd);
    const std::optional<Point2> divergePoint = rayLineIntersection(backwardEndRay, intersectTestLineStart, intersectTestLineEnd);
    
    std::vector<Ray2D> testRays;
    std::vector<RayVariant> rays;
    ScatterSectionBehavior behavior;
    if(!convergePoint.has_value() && !divergePoint.has_value()) {
        //light is a beam
        behavior = ScatterSectionBehavior::parallel;
        const Vector2 linearDirection = startRay.direction;
        const Vector2 startPoint = startRay.origin;
        const Vector2 endPoint = endRay.origin;
        for(Point2 point : points) {
            std::optional<Point2> rayOrigin = rayLineIntersection(
                Ray2D{point, -linearDirection}, startPoint, endPoint);
            if(rayOrigin.has_value()) {
                testRays.push_back(Ray2D{rayOrigin.value(), linearDirection});
            }
        }
    } else if (convergePoint.has_value()) {
        //converging light
        behavior = ScatterSectionBehavior::converges;
        for(Point2 point : points) {
            const Vector2 startPoint = startRay.origin;
            const Vector2 endPoint = endRay.origin;
            const Vector2 direction = getDirectionVectorToFrom(point, convergePoint.value());

            std::optional<Point2> rayOrigin = rayLineIntersection(
                Ray2D{point, -direction}, startPoint, endPoint);
            if(rayOrigin.has_value()) {
                testRays.push_back(Ray2D{rayOrigin.value(), direction});
            }
        }
        
    } else {
        //diverging light
        behavior = ScatterSectionBehavior::diverges;
         for(Point2 point : points) {
            const Vector2 startPoint = startRay.origin;
            const Vector2 endPoint = endRay.origin;
            const Vector2 direction = getDirectionVectorToFrom(point, divergePoint.value());
            
            std::optional<Point2> rayOrigin = rayLineIntersection(
                Ray2D{point, direction}, startPoint, endPoint);
            if(rayOrigin.has_value()) {
                testRays.push_back(Ray2D{rayOrigin.value(), -direction});
            }
        }
    }
	
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
        Ray2D side1 = Ray2D{ray.origin - (surfaceSlopeDir * scatterRaySpread), 
			ray.direction.rotated(scatterRaySpread).normalized()};
            Ray2D side2 = Ray2D{ray.origin + (surfaceSlopeDir * scatterRaySpread), 
			ray.direction.rotated(-scatterRaySpread).normalized()};
		testRay(side2);
		testRay(ray);
		testRay(side1);
	}

    ShotScatterReturn result;
	result.rays = std::move(rays);
    result.behavior = behavior;
	return result;
}

std::vector<RayVariant> shotScatterSecondRadialLight(
    const std::optional<ShapeId>& shapeOriginId, const Ray2D& startRay, const Ray2D& endRay,
    const std::vector<Point2>& points, const BVH2D& bvh, const real_t radialRaySpread) {
    const real_t arc = startRay.direction.angle_to(endRay.direction);
    const Point2 startLocation = startRay.origin;

    const Vector2 midpoint = ((startRay.direction.normalized() + endRay.direction.normalized()) / 2.0f).normalized();
    const real_t angle = clockwiseAngle(midpoint);
    return shotSpotLight2D(angle, startLocation, arc, points, bvh, radialRaySpread);
}