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

void constructBVH2D(BVH2D& bvh, std::vector<Shape2D>& shapes) {
    std::sort(shapes.begin(), shapes.end(), 
        [](Shape2D lhs, Shape2D rhs) -> bool { return lhs.midPoint < rhs.midPoint; }
    );
    bvh.shapes = shapes;
    bvh.root = constructBVH2DRecursiveHelper(bvh, ArrayView<Shape2D>{bvh.shapes.data(), bvh.shapes.size()});
}

void resetBVH2D(BVH2D& bvh) {
    if(bvh.root == nullptr) {
        return;
    }
    bvh.root = nullptr;
    bvh.shapes.clear();
    bvh.nodePool.clear();
}

std::optional<RayHit2D> shotRayBVH2DNode(const Ray2D& ray, const BVH2D::Node* node) {
    if(node == nullptr) {
        return {};
    }
    if(!isRayIntersectsAABB(ray, node->aabb)) {
        return{};
    }

    std::optional<RayHit2D> rayHit;
    auto checkClosesHit = [&](std::optional<RayHit2D> newRayHit) {
        if(newRayHit.has_value()) {
            if(rayHit.has_value()) {
                if(ray.origin.distance_to(newRayHit.value().location) < 
                    ray.origin.distance_to(rayHit.value().location)) {
                    rayHit = newRayHit;
                } 
            } else {
                rayHit = newRayHit;
            }
        }
    };

    for(std::size_t i = 0; i < node->shapes.size; i++) {
        Shape2D& shape = node->shapes.data[i];
        if(shape.points.size() < 2) {
            continue;
        }
        if(!isRayIntersectsAABB(ray, shape.aabb)) {
            continue;
        }

        std::optional<RayHit2D> rayHit;
        for(std::size_t i = 0; i < shape.points.size(); i++) {
            std::optional<RayHit2D> newRayHit;
            if(i == shape.points.size() - 1) {
                newRayHit = rayLineHit(shape.shapeId, ray,
                    shape.points[i], shape.points[0]);
            } else {
                newRayHit = rayLineHit(shape.shapeId, ray, 
                    shape.points[i], shape.points[i + 1]);
            }
            checkClosesHit(newRayHit);
        }
    }

    if(node->b1) {
        checkClosesHit(shotRayBVH2DNode(ray, node->b1));
    }
    if(node->b1) {
        checkClosesHit(shotRayBVH2DNode(ray, node->b2));
    }
    return rayHit;
}  

std::optional<RayHit2D> shotRay(const Ray2D& ray, const BVH2D& bvh) {
    return shotRayBVH2DNode(ray, bvh.root);
}  