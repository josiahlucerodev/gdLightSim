#include "shape2D.h"

Shape2D constructShape2D(Polygon2D& polygon, std::size_t shapeId) {
    PackedVector2Array polygonPoints = polygon.get_polygon();
    if(polygonPoints.size() == 0) {
        return Shape2D{};
    }
    
    Shape2D shape;
    shape.type = Shape2DType::wall;
    shape.shapeId = shapeId;
    shape.maxBounce = 0;
    shape.rayCount = 0;
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

std::vector<Point2> getPoints(const std::vector<Shape2D>& shapes){
    std::vector<Point2> points;
    for(Shape2D shape : shapes) {
        points.insert(points.end(), shape.points.begin(),  shape.points.end());
    }
    return points;
}
std::vector<Point2> getPointsExcluding(ShapeId excludeId, const std::vector<Shape2D>& shapes) {
    std::vector<Point2> points;
    for(Shape2D shape : shapes) {
        if(shape.shapeId != excludeId) {
            points.insert(points.end(), shape.points.begin(),  shape.points.end());
        }
    }
    return points;
}