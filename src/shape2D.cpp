#include "shape2D.h"

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
Shape2D constructShape2D() {
    Shape2D shape{};
    shape.type = Shape2DType::wall;
    shape.shapeId = 0;
    shape.aabb = AABB2D{};
    return shape;
}