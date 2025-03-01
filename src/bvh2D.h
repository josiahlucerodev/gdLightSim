#pragma once

//std
#include <vector>
#include <optional>

//own
#include "batchPool.h"
#include "aabb2D.h"
#include "shape2D.h"
#include "ray2D.h"

template<typename Type>
struct ArrayView {
    Type* data;
    std::size_t size;
};

struct BVH2D {
    struct Node {
        Node* b1;
        Node* b2;
        AABB2D aabb;
        Point2 midPoint;
        ArrayView<Shape2D> shapes;
    };
    
    BatchPool<Node> nodePool;
    std::vector<Shape2D> sortedShapes;
    Node* root;

    BVH2D() = default;
    BVH2D(const BVH2D&) = delete;
    BVH2D& operator=(const BVH2D&) = delete;
};

void constructBVH2D(BVH2D& bvh, const std::vector<Shape2D>& shapes);
void resetBVH2D(BVH2D& bvh);
std::optional<RayHit2D> shotRay(const Ray2D& ray, const std::optional<ShapeId>&, const BVH2D& bvh);