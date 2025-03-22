#include "bvh2D.h"

//std
#include <algorithm>

BVH2D::Node* constructBVH2DRecursiveHelper(BVH2D& bvh, ArrayView<Shape2D> shapes) {
    if(shapes.size == 0) {
        return nullptr;
    }

    BVH2D::Node& node = *bvh.nodePool.newElement();
    node.b1 = nullptr;
    node.b2 = nullptr;
    
    if (shapes.size < 5) {
        node.shapes = shapes;
        node.aabb = shapes.data[0].aabb;
        node.midPoint = Point2(0, 0);

        for(std::size_t i = 0; i < shapes.size; i++) {
            Shape2D& shape = shapes.data[i];
            node.aabb = updateAABB(node.aabb, shape.aabb);
            node.midPoint += shape.midPoint;
        }
        node.midPoint /= shapes.size;

    } else {
        std::size_t b1Size = shapes.size / 2;
        node.b1 = constructBVH2DRecursiveHelper(bvh, ArrayView<Shape2D>{shapes.data, b1Size});
        node.b2 = constructBVH2DRecursiveHelper(bvh, ArrayView<Shape2D>{shapes.data + b1Size, shapes.size - b1Size});
        
        node.aabb = node.b1->aabb;
        node.aabb = updateAABB(node.aabb, node.b2->aabb);
    }

    return &node;
}

void constructBVH2D(BVH2D& bvh, const std::vector<Shape2D>& shapes) {
    std::vector<Shape2D> sortedShapes = shapes;
    std::sort(sortedShapes.begin(), sortedShapes.end(), 
        [](Shape2D lhs, Shape2D rhs) -> bool { return lhs.midPoint < rhs.midPoint; }
    );
    bvh.sortedShapes = sortedShapes;
    bvh.shapeIsHit.resize(sortedShapes.size());
    bvh.root = constructBVH2DRecursiveHelper(bvh, ArrayView<Shape2D>{bvh.sortedShapes.data(), bvh.sortedShapes.size()});
}

void resetBVH2D(BVH2D& bvh) {
    if(bvh.root == nullptr) {
        return;
    }
    bvh.root = nullptr;
    bvh.sortedShapes.clear();
    bvh.nodePool.clear();
    bvh.shapeIsHit.clear();
}

struct ShapeHit {
    Shape2D* shape;
    RayHit2D shapeHit;
};

std::optional<ShapeHit> shotRayBVH2DNode(const Ray2D& ray, const std::optional<ShapeId>& shapeOriginId, BVH2D& bvh, BVH2D::Node* node) {
    if(node == nullptr) {
        return {};
    }
    if(!isRayIntersectsAABB(ray, node->aabb)) {
        return{};
    }

    std::optional<ShapeHit> shapeHit;
    auto checkClosesHit = [&](std::optional<ShapeHit> newShapeHit) {
        if(newShapeHit.has_value()) {
            bvh.sortedShapes;
            bvh.shapeIsHit[newShapeHit->shape->shapeId] = true;
            if(newShapeHit->shape->type == Shape2DType::sensor) {
                return;
            }

            if(shapeHit.has_value()) {
                if(ray.origin.distance_to(newShapeHit.value().shapeHit.location) < 
                    ray.origin.distance_to(shapeHit.value().shapeHit.location)) {
                    shapeHit = newShapeHit;
                } 
            } else {
                shapeHit = newShapeHit;
            }
        }
    };

    for(std::size_t i = 0; i < node->shapes.size; i++) {
        Shape2D& shape = node->shapes.data[i];
        if(shape.points.size() < 2) {
            continue;
        }
        if(!isRayIntersectsAABB(ray, shape.aabb) 
            || (shapeOriginId.has_value() && shapeOriginId.value() == shape.shapeId)) {
            continue;
        }

        std::optional<ShapeHit> shapeHit;
        for(std::size_t i = 0; i < shape.points.size(); i++) {
            std::optional<ShapeHit> newShapeHit;
            if(i == shape.points.size() - 1) {
                std::optional<RayHit2D> hit = rayLineHit(shape.shapeId, ray,
                    shape.points[i], shape.points[0]);
                if(hit.has_value()) {
                    ShapeHit currentShapeHit;
                    currentShapeHit.shape = &shape;
                    currentShapeHit.shapeHit = hit.value();
                    newShapeHit = currentShapeHit;
                }
            } else {
                std::optional<RayHit2D> hit = rayLineHit(shape.shapeId, ray, 
                    shape.points[i], shape.points[i + 1]);
                if(hit.has_value()) {
                    ShapeHit currentShapeHit;
                    currentShapeHit.shape = &shape;
                    currentShapeHit.shapeHit = hit.value();
                    newShapeHit = currentShapeHit;
                }
            }
            checkClosesHit(newShapeHit);
        }
    }

    if(node->b1) {
        checkClosesHit(shotRayBVH2DNode(ray, shapeOriginId, bvh, node->b1));
    }
    if(node->b1) {
        checkClosesHit(shotRayBVH2DNode(ray, shapeOriginId, bvh, node->b2));
    }
    return shapeHit;
}  

std::optional<RayHit2D> shotRay(const Ray2D& ray, const std::optional<ShapeId>& shapeOriginId, BVH2D& bvh) {
    std::optional<ShapeHit> shapeHit = shotRayBVH2DNode(ray, shapeOriginId, bvh, bvh.root);
    if(shapeHit.has_value()) {
        return shapeHit.value().shapeHit;
    } else {
        return std::nullopt;
    }
}  