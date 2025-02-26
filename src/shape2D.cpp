#include "shape2D.h"

Shape2D constructShape2D(Polygon2D& polygon, std::size_t shapeId) {
    PackedVector2Array polygonPoints = polygon.get_polygon();
    if(polygonPoints.size() == 0) {
        return Shape2D{};
    }
    
    Shape2D shape;
    shape.type = Shape2DType::wall;
    shape.shapeId = shapeId;
    shape.points.reserve(polygonPoints.size());
    Point2 point = polygon.get_transform().get_origin() + polygonPoints[0];
    shape.midPoint = Point2(0, 0);
    shape.aabb = AABB2D{
        Point2{point.x, point.y},
        Point2{point.x, point.y}
    };
    
    for(std::size_t i = 0; i < polygonPoints.size(); i++) {
        Point2 point = polygonPoints[i] + polygon.get_transform().get_origin();
        shape.aabb = updateAABB(shape.aabb, {Point2(point.x, point.y), Point2(point.x, point.y)});
        shape.midPoint += point;
        shape.points.push_back(point);
    }
    shape.midPoint /= polygonPoints.size();

    return shape;
}

Shape2D constructShape2D(Mirror2D& mirror, std::size_t shapeId) {
    Shape2D shape;
    shape.type = Shape2DType::mirror;
    shape.shapeId = shapeId;
    
    real_t mirrorWidth = mirror.get_mirror_width();
    Vector2 position = mirror.get_transform().get_origin();
    Point2 rightPoint = position + (Point2(0, -1) * mirrorWidth);
    Point2 leftPoint = position + (Point2(0, 1) * mirrorWidth);
    
    shape.aabb = AABB2D{
        Point2{rightPoint.x, rightPoint.y},
        Point2{rightPoint.x, rightPoint.y}
    };
    shape.aabb = updateAABB(shape.aabb, {Point2(leftPoint.x, leftPoint.y), Point2(leftPoint.x, leftPoint.y)});
    
    shape.midPoint = (leftPoint + rightPoint) / 2;
    shape.points.reserve(2);
    shape.points.push_back(rightPoint);
    shape.points.push_back(leftPoint);

    return shape;
}
