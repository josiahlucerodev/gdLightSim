#include "aabb2D.h"

//std
#include <algorithm>

AABB2D createAABB(const Point2 point) {
    return AABB2D{
        Point2{point.x, point.y},
        Point2{point.x, point.y}
    };
}

AABB2D updateAABB(const AABB2D rhs, const AABB2D lhs) {
    return AABB2D {
        Point2{std::min(lhs.min.x, rhs.min.x), std::min(lhs.min.y, rhs.min.y)},
        Point2{std::max(lhs.max.x, rhs.max.x), std::max(lhs.max.y, rhs.max.y)}
    };
}

AABB2D updateAABB(const AABB2D rhs, const Point2 point) {
    return updateAABB(rhs, AABB2D{
        Point2{point.x, point.y},
        Point2{point.x, point.y}
    });
}