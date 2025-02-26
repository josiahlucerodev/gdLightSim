#include "beamLight2D.h"

//std
#include <algorithm>

//godotcpp
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void BeamLight2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_light_width"), &BeamLight2D::get_light_width);
	ClassDB::bind_method(D_METHOD("set_light_width", "light_width"), &BeamLight2D::set_light_width);
	ClassDB::bind_method(D_METHOD("get_ray_count"), &BeamLight2D::get_ray_count);
	ClassDB::bind_method(D_METHOD("set_ray_count", "ray_count"), &BeamLight2D::set_ray_count);
	ClassDB::bind_method(D_METHOD("get_draw_debug"), &BeamLight2D::get_draw_debug);
	ClassDB::bind_method(D_METHOD("set_draw_debug", "draw_debug"), &BeamLight2D::set_draw_debug);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "light_width", PROPERTY_HINT_RANGE, "1,10000,1"), "set_light_width", "get_light_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "ray_count", PROPERTY_HINT_RANGE, "3,10000, 1"), "set_ray_count", "get_ray_count");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_debug"), "set_draw_debug", "get_draw_debug");
}

double BeamLight2D::get_light_width() const {
	return lightWidth;
}
void BeamLight2D::set_light_width(const double lightWidth) {
	this->lightWidth = lightWidth;
}
int64_t BeamLight2D::get_ray_count() const {
	return rayCount;
}
void BeamLight2D::set_ray_count(const int64_t rayCount) {
	this->rayCount = rayCount;
}

bool BeamLight2D::get_draw_debug() const {
	return drawDebug;
}
void BeamLight2D::set_draw_debug(const bool drawDebug) {
	this->drawDebug = drawDebug;
}

BeamLight2D::BeamLight2D() {
	lightWidth = 10;
	rayCount = 3;
	drawDebug = false;
}

BeamLight2D::~BeamLight2D() {
}

void BeamLight2D::_ready() {
	queue_redraw();
}

void BeamLight2D::_process(double delta) {
	queue_redraw();
}

void BeamLight2D::_draw() {
    if(drawDebug) {
        Color color = Color(1.0, 1.0, 0.0);
		real_t lineWidth = 5;
		size_t distance = 10000;
        real_t r = get_rotation();
	
        real_t piOver2 = Math_PI / 2;
        Point2 rightOffset = Point2(0, -1) * lightWidth;
        Point2 leftOffset = Point2(0, 1) * lightWidth;

        draw_line(rightOffset, rightOffset + Vector2(distance, 0)
            ,color, lineWidth);
        draw_line(leftOffset, leftOffset + Vector2(distance, 0)
            ,color, lineWidth);

        size_t distanceBetweenSections = 100;
        for(size_t i = 0; i < (distance / distanceBetweenSections); i++) {
            size_t segmentDistance = (distanceBetweenSections * i);
            draw_line(leftOffset + Vector2(segmentDistance, 0), rightOffset + Vector2(segmentDistance, 0)
                ,color, lineWidth);
        }
    }
}

struct BeamRayToPoint {
    real_t beamOriginDistance;
    Vector2 rayOrigin;
};

std::optional<BeamRayToPoint> getBeamRayToPoint(Point2 beamLocation, real_t beamRotiation, 
    Point2 unscaledRightBeamPoint, Point2 point) {
        
    real_t theta = beamLocation.angle_to_point(point) - (beamRotiation + Math::deg_to_rad(real_t(90)));
    theta = fmodf(theta, Math::deg_to_rad(real_t(360)));
    if(theta > 0) {
        theta -=Math::deg_to_rad(real_t(360));
    }
    if(abs(theta + Math::deg_to_rad(real_t(90))) > (Math_PI / 2)) {
        return std::nullopt;
    }
    
    real_t h = beamLocation.distance_to(point);
    real_t a = cos(theta) * h;
    Vector2 origin = (unscaledRightBeamPoint * a) + beamLocation;

    BeamRayToPoint beamRayToPoint{};
    beamRayToPoint.beamOriginDistance = a;
    beamRayToPoint.rayOrigin = origin;
    return beamRayToPoint;
}

std::vector<RayVariant> shotBeamLight2D(
	BeamLight2D& beamLight, 
	const std::vector<Point2>& points, 
	BVH2D& bvh, 
	real_t linearRaySpread) {
	Point2 beamLightLocation = beamLight.get_position();
	real_t beamLightWidth = beamLight.get_light_width();
	int64_t beamLightRayCount = beamLight.get_ray_count();
	real_t beamLightRotation = beamLight.get_rotation();

	Vector2 beamDirection = Vector2(cos(beamLightRotation), sin(beamLightRotation));
	real_t piOver2 = Math_PI / 2;
	Vector2 unscaledRightBeamPoint = Vector2(cos(beamLightRotation + piOver2), sin(beamLightRotation + piOver2));
	Vector2 rightBeamPoint = unscaledRightBeamPoint * beamLightWidth;
	rightBeamPoint += beamLightLocation;
	Vector2 unscaledLeftBeamPoint = Vector2(cos(beamLightRotation - piOver2), sin(beamLightRotation - piOver2));
	Vector2 leftBeamPoint = unscaledLeftBeamPoint * beamLightWidth;
	leftBeamPoint += beamLightLocation;

	std::vector<BeamRayToPoint> beamRays;
	beamRays.reserve(points.size());

	std::vector<RayVariant> rays;
	auto testRay = [&](Ray2D ray) {
		std::optional<RayHit2D> rayHit = shotRay(ray, bvh);
        if(rayHit.has_value()) {
            rays.push_back(RayVariant(rayHit.value()));
        } else {
            rays.push_back(RayVariant(ray));
        }
	};

	for(Point2 point : points) {
		std::optional<BeamRayToPoint> rayToPointOption = getBeamRayToPoint(beamLightLocation, beamLightRotation
			, unscaledRightBeamPoint, point);
		if(rayToPointOption.has_value()) {
			BeamRayToPoint rayToPoint = rayToPointOption.value();
			if(abs(rayToPoint.beamOriginDistance) < (beamLightWidth)) {
				beamRays.push_back(rayToPoint);
			}
		}
	}
	
	{
		testRay(Ray2D{leftBeamPoint, beamDirection});
		testRay(Ray2D{rightBeamPoint, beamDirection});
	}

	for(size_t i = 0; i < beamRays.size(); i++) {
		BeamRayToPoint& rayToPoint = beamRays[i];

		for(std::size_t i = 0; i < beamLightRayCount; i++) {
			real_t offset = (linearRaySpread / -2) + ((linearRaySpread / beamLightRayCount) * i);
			Vector2 spreadOrigin = rayToPoint.rayOrigin + Vector2{offset, offset};
			testRay(Ray2D{spreadOrigin, beamDirection});
		}
	}

	return rays;
}

std::vector<LinearScanSection> generateBeamLight2DSections(
	BeamLight2D& beamLight, 
	std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes,
	real_t linearSectionTolerance) {
	Point2 beamLightLocation = beamLight.get_position();

	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			Point2 lhsO = getRay(lhs).origin;
			Point2 rhsO = getRay(rhs).origin;

			return Math::sign(beamLightLocation.angle_to(lhsO)) * beamLightLocation.distance_to(lhsO)
				< Math::sign(beamLightLocation.angle_to(rhsO)) * beamLightLocation.distance_to(rhsO);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		auto calculateSlope = [](const Point2& p1, const Point2& p2) -> double {
			return (p2.y - p1.y) / (p2.x - p1.x);
		};            

		double slope1 = calculateSlope(r1.location, r2.location);
		double slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > linearSectionTolerance;
	};

	return generateSectionsBase<LinearScanSection>(shapes, rays, predicate);
}