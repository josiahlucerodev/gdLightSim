#include "circleLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "angle.h"


void CircleLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_ray_count"), &CircleLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &CircleLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &CircleLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &CircleLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10000, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

int64_t CircleLight2D::get_ray_count() const {
	return rayCount;
}
void CircleLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool CircleLight2D::get_draw_debug() const {
	return drawDebug;
}
void CircleLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

CircleLight2D::CircleLight2D() {
	rayCount = 3;
	drawDebug = false;
}

CircleLight2D::~CircleLight2D() {
}

void CircleLight2D::_ready() {
	queue_redraw();
}

void CircleLight2D::_process(double delta) {
	queue_redraw();
}

void CircleLight2D::_draw() {
	if(drawDebug) {
		Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 5;
		real_t distance = 10000;
		draw_circle(Point2(0, 0), 30, color, true);
		
		std::size_t numOfDisSegments = 50;
		for(std::size_t i = 0; i < numOfDisSegments; i++) {
			real_t localDistance = (distance / numOfDisSegments) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = (Math_PI * 2) / numOfSegments;
				real_t localAngle = (segmentSpreadAngle * j);
				
				draw_line(
					Point2{cos(localAngle), sin(localAngle)} * localDistance,
					Point2{cos(localAngle + segmentSpreadAngle), sin(localAngle + segmentSpreadAngle)} * localDistance,
					color, lineWidth);
			}
		}
	}
}

std::vector<RayVariant> shotCircleLight2D(
	const CircleLight2D& circleLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t radialRaySpread) {
	Point2 circleLightLocation = circleLight.get_position();
	int64_t circleLightRayCount = circleLight.get_ray_count();


	std::vector<RayVariant> rays;
	auto testRay = [&](Ray2D ray) {
		std::optional<RayHit2D> rayHit = shotRay(ray, {}, bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
	};

	for(Point2 point : points) {
		real_t distance = circleLightLocation.distance_to(point);
		real_t arc = (radialRaySpread / distance) / circleLightRayCount;

		for(std::size_t i = 0; i < circleLightRayCount; i++) {
			real_t angle = circleLightLocation.angle_to_point(point) - (arc / 2);
			angle += ((arc / circleLightRayCount) * i);
			Point2 direction = Point2(cos(angle), sin(angle));
			testRay(Ray2D{circleLightLocation, direction});
		}
	}

	return rays;
}

std::vector<RadialScanSection> generateCircleLight2DSections(
	const CircleLight2D& circleLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t radialSectionTolerance) {
	Point2 circleLightLocation = circleLight.get_position();
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return clockwiseAngle(circleLightLocation, getRay(lhs).direction) 
				< clockwiseAngle(circleLightLocation, getRay(rhs).direction);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
			return (p2.y - p1.y) / (p2.x - p1.x);
		};            

		double slope1 = calculateSlope(r1.location, r2.location);
		double slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > radialSectionTolerance;
	};

	if(!rays.empty()) {
		RayVariant front = rays.front();
		rays.push_back(front);
	}

	return generateSectionsBase<RadialScanSection>(shapes, rays, predicate);
}