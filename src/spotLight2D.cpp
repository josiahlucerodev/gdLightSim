#include "spotLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

//own
#include "angle.h"

void SpotLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_arc"), &SpotLight2D::get_arc);
	ClassDB::bind_method(D_METHOD("set_arc", "arc"), &SpotLight2D::set_arc);
	ClassDB::bind_method(D_METHOD("get_ray_count"), &SpotLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &SpotLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &SpotLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &SpotLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "arc", PROPERTY_HINT_RANGE, "1,180,0.01"), "set_arc", "get_arc");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10000, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double SpotLight2D::get_arc() const {
	return arc;
}
void SpotLight2D::set_arc(const double arc) {
	this->arc = arc;
}
int64_t SpotLight2D::get_ray_count() const {
	return rayCount;
}
void SpotLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool SpotLight2D::get_draw_debug() const {
	return drawDebug;
}
void SpotLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

SpotLight2D::SpotLight2D() {
	arc = 30;
	rayCount = 3;
	drawDebug = false;
}

SpotLight2D::~SpotLight2D() {
}

void SpotLight2D::_ready() {
	queue_redraw();
}

void SpotLight2D::_process(double delta) {
	queue_redraw();
}

void SpotLight2D::_draw() {
	if(drawDebug) {
		real_t arcRad = Math::deg_to_rad(arc);
		real_t angle = get_rotation() - (arcRad / 2);
		Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 5;
		real_t distance = 10000;
		draw_line(Point2(0, 0), Point2{cos(angle), sin(angle)} * distance, color, lineWidth);
		draw_line(Point2(0, 0), Point2{cos(angle + arcRad), sin(angle + arcRad)} * distance, color, lineWidth);
		draw_circle(Point2(0, 0), 30, color, true);
		
		std::size_t numOfDisSegments = 50;
		for(std::size_t i = 0; i < numOfDisSegments; i++) {
			real_t localDistance = (distance / numOfDisSegments) * i;
			std::size_t numOfSegments = 6 * (i + 2) / 2;
			
			for(std::size_t j = 0; j < numOfSegments; j++) {
				real_t segmentSpreadAngle = arcRad / numOfSegments;
				real_t localAngle = angle + (segmentSpreadAngle * j);
				
				draw_line(
					Point2{cos(localAngle), sin(localAngle)} * localDistance,
					Point2{cos(localAngle + segmentSpreadAngle), sin(localAngle + segmentSpreadAngle)} * localDistance,
					color, lineWidth);
			}
		}
	}
}

std::vector<RayVariant> shotSpotLight2D(
	const SpotLight2D& spotLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t radialRaySpread) {
	real_t spotLightAngle = spotLight.get_rotation();
	Point2 spotLightLocation = spotLight.get_position();
	real_t spotLightArc = Math::deg_to_rad(spotLight.get_arc());
	int64_t spotLightRayCount = spotLight.get_ray_count();

	std::vector<Point2> pointsInSpotlightArc;
	pointsInSpotlightArc.reserve(points.size());
	std::vector<RayVariant> rays;
	
	auto testRay = [&](Point2 direction) {
		Ray2D ray = Ray2D{spotLightLocation, direction};
		std::optional<RayHit2D> rayHit = shotRay(ray, {}, bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
	};
	
	{
		real_t angle = spotLightAngle - (spotLightArc / 2);
		Point2 startRay = Point2(cos(angle), sin(angle));
		testRay(startRay);
		Point2 endRay = Point2(cos(angle + spotLightArc), sin(angle + spotLightArc));
		testRay(endRay);
	}
	
	
	for(Point2 point : points) {
		if(abs(spotLightLocation.angle_to_point(point)) <= spotLightArc / 2) {
			pointsInSpotlightArc.push_back(point);
		}
	}

	for(size_t i = 0; i < pointsInSpotlightArc.size(); i++) {
		Point2& point = pointsInSpotlightArc[i];
		real_t distance = spotLightLocation.distance_to(point);
		real_t arc = (radialRaySpread / distance) / spotLightRayCount;

		for(std::size_t i = 0; i < spotLightRayCount; i++) {
			real_t angle = spotLightLocation.angle_to_point(point) - (arc / 2);
			angle += ((arc / spotLightRayCount) * i);
			Point2 direction = Point2(cos(angle), sin(angle));
			testRay(direction);
		}
	}
    return rays;
}

std::vector<RadialScanSection> generateSpotLight2DSections(
	const SpotLight2D& spotLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t radialSectionTolerance) {
	Point2 spotLightLocation = spotLight.get_position();

	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return clockwiseAngle(spotLightLocation, getRay(lhs).direction) 
				< clockwiseAngle(spotLightLocation, getRay(rhs).direction);
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

	return generateSectionsBase<RadialScanSection>(shapes, rays, predicate);
}