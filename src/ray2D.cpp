#include "ray2D.h"
#include "util.h"

bool isRayIntersectsAABB(const Ray2D& ray, const AABB2D& aabb) {
    if (ray.origin.x >= aabb.min.x && ray.origin.x <= aabb.max.x &&
        ray.origin.y >= aabb.min.y && ray.origin.y <= aabb.max.y) {
        return true;
    }
    real_t tmin = (aabb.min.x - ray.origin.x) / ray.direction.x;
    real_t tmax = (aabb.max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax)  {
        std::swap(tmin, tmax);
    }

    real_t tymin = (aabb.min.y - ray.origin.y) / ray.direction.y;
    real_t tymax = (aabb.max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) {
        std::swap(tymin, tymax);
    }

    if (tmin > tymax || tymin > tmax) {
        return false; 
    }

    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }
    return tmax >= 0;
}

std::optional<Point2> rayLineIntersection(const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd) {
    Point2 lineDir = lineEnd - lineStart;
    Point2 rayToLine = lineStart - ray.origin;

    real_t denom = ray.direction.cross(lineDir);
    if (std::fabs(denom) < 1e-5) {
        return {};
    }

    real_t t = rayToLine.cross(lineDir) / denom;
    real_t u = rayToLine.cross(ray.direction) / denom;
    if (t >= 0 && u >= 0 && u <= 1) {
        Point2 intersection = ray.origin + ray.direction * t;
        return intersection;
    }

    return {};
}

std::optional<RayHit2D> rayLineHit(std::size_t shapeId, const Ray2D& ray, const Point2& lineStart, const Point2& lineEnd) {
    std::optional<Point2> intersectionOption = rayLineIntersection(ray, lineStart, lineEnd);
    if(!intersectionOption.has_value()) {
        return std::nullopt;
    }
    Point2 intersection = intersectionOption.value();
    Vector2 lineDir = lineEnd - lineStart;
    Vector2 normal = {-lineDir.y, lineDir.x};  
    normal.normalize();
    real_t angle = clockwiseAngle(ray.direction);

    return RayHit2D{shapeId, angle, ray, intersection};
}

const Ray2D& getRay(const RayVariant& rayVariant) {
    return std::visit([](auto&& arg) -> const Ray2D& {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Ray2D>) {
            return arg;
        } else {
            return arg.ray;
        }
    }, rayVariant);
} ;

real_t getRayReflectionAngle(const real_t& rayAngle, const real_t& mirrorRotation) {
    return -(rayAngle - mirrorRotation) + mirrorRotation;
}

bool doLinesIntersect(const Point2& p1, const Point2& q1, const Point2& p2, const Point2& q2) {
    Point2 r = q1 - p1;
    Point2 s = q2 - p2;

    real_t rxs = r.cross(s);
    real_t qp1xr = (p2 - p1).cross(r);
    real_t qp1xs = (p2 - p1).cross(s);

    if (rxs == 0) {
        return false;
    }

    real_t t = qp1xs / rxs;
    real_t u = qp1xr / rxs;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}