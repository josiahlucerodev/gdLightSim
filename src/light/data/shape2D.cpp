#include "shape2D.h"

//own
#include "../ops/util.h"

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
Shape2D constructShape2D(const ShapeId shapeId) {
    Shape2D shape{};
    shape.type = Shape2DType::wall;
    shape.shapeId = shapeId;
    shape.aabb = AABB2D{};
    return shape;
}
Shape2D constructShape2D(const ShapeId shapeId, const PackedVector2Array& points) {
    if(points.size() == 0) {
        return constructShape2D(shapeId);
    }

    Shape2D shape;
    shape.type = Shape2DType::unknown;
    shape.shapeId = shapeId;
    
    shape.midPoint = Point2(0, 0);
    shape.points.reserve(points.size());
    shape.aabb = createAABB(points[0]);

    for(std::size_t i = 0; i < points.size(); i++) {
        Point2 point = points[i];
        shape.aabb = updateAABB(shape.aabb, point);
        shape.midPoint += point;
        shape.points.push_back(point);
    }
    shape.midPoint /= points.size();
    return shape;
}

Shape2D constructShape2D(const ShapeId shapeId, real_t width, real_t rotation, Point2 position) {
    const Point2 dir = vectorFromAngle(rotation + (Math_PI / 2));
    const Point2 rightPoint = position + (dir * (width / 2));
    const Point2 leftPoint = position + ((-dir) * (width / 2));

    Shape2D shape;
    shape.type = Shape2DType::unknown;
    shape.shapeId = shapeId;
    shape.midPoint = (leftPoint + rightPoint) / 2;
    shape.points = {rightPoint, leftPoint};

    shape.aabb = createAABB(rightPoint);
    shape.aabb = updateAABB(shape.aabb, leftPoint);
    return shape;
}