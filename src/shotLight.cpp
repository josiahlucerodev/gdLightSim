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

bool areRayLocationsRelativelyClose(const Point2& a, const Point2& b, const real_t& distanceThreshold) {
    return a.distance_to(b) <= distanceThreshold;
}

void shotAndAddRaysAtPoint(
    std::vector<RayVariant>& raysDst, const std::optional<ShapeId>& shapeOriginId,
    const BVH2D& bvh, const real_t& linearRaySpread, const Point2& point,
    const Ray2D& leftRay, const Ray2D& middleRay, const Ray2D& rightRay) {
    const std::optional<RayHit2D> leftHit = shotRay(leftRay, shapeOriginId, bvh);
    const std::optional<RayHit2D> middleHit = shotRay(middleRay, shapeOriginId, bvh);
    const std::optional<RayHit2D> rightHit = shotRay(rightRay, shapeOriginId, bvh);
    
    auto alignHit = [&](const RayHit2D& rayHit) -> RayHit2D {
        RayHit2D aHit = rayHit;
        aHit.location = point;
        return rayHit;
    };
    
    const real_t distanceThreshold = 0.01;
    if(middleHit.has_value()) {
        raysDst.push_back(middleHit.value());

        if(!leftHit.has_value()) {
            raysDst.push_back(leftRay);
        } else if (!areRayLocationsRelativelyClose(
            leftHit.value().location, middleHit.value().location, distanceThreshold)) { 
            raysDst.push_back(leftHit.value());
        }

        if(!rightHit.has_value()) {
            raysDst.push_back(rightRay);
        } else if (!areRayLocationsRelativelyClose(
            rightHit.value().location, middleHit.value().location, distanceThreshold)) {
            raysDst.push_back(rightHit.value());
        }
    } else {
        raysDst.push_back(middleRay);

        if(leftHit.has_value()) { //only hit
            raysDst.push_back(alignHit(leftHit.value()));
        } else if(rightHit.has_value()) { //only hit
            raysDst.push_back(alignHit(rightHit.value()));
        }
    }
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
    std::vector<RayVariant> rays;
	for(Point2 point : points) {
		Point2 parallelLineToPointStart = (surfaceSlopeDir * 1000000) + point; 
		Point2 parallelLineToPointEnd = (-surfaceSlopeDir * 1000000) + point; 
		
		auto getDistance = [&](Ray2D ray) -> std::optional<real_t> {
            std::optional<Point2> hit = rayLineIntersection(ray, parallelLineToPointStart, parallelLineToPointEnd);
			if(!hit.has_value()) {
                return {};
			}
			return hit.value().distance_to(point);
		};
		
		std::optional<real_t> distanceToStart = getDistance(startRay);
        if(!distanceToStart.has_value()) {
            continue;
        }
		std::optional<real_t> distanceToEnd = getDistance(endRay);
        if(!distanceToEnd.has_value()) {
            continue;
        }

        Vector2 directionToStart = point.direction_to(startRay.origin);
        Vector2 directionToEnd = point.direction_to(endRay.origin);

        /*
		Vector2 rayDir = weightedSlerp(startRay.direction, distanceToStart, endRay.direction, distanceToEnd);
        rays.push_back(Ray2D{point, rayDir * -1});
        std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, rayDir * -1}, startRay.origin, endRay.origin);
        */
       
        Vector2 rayDir = weightedSlerp(directionToStart, distanceToStart.value(), directionToEnd, distanceToEnd.value());
        //rays.push_back(Ray2D{point, rayDir});
        std::optional<Point2> rayOrigin = rayLineIntersection(Ray2D{point, rayDir}, startRay.origin, endRay.origin);

		if(rayOrigin.has_value()) {
			testRays.push_back(Ray2D{rayOrigin.value(), rayDir * -1});
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