#include "data.h"

//own
#include "../settings.h"

Rect2 createRect(AABB2D& aabb) {
    return Rect2(aabb.min, aabb.max - aabb.min);
}

void drawAABB(Node2D& node, AABB2D aabb, Color color) {
    node.draw_rect(createRect(aabb), color, false, Settings::debugLineWidth);
    node.draw_circle(aabb.min, Settings::pointRadius, color);
    node.draw_circle(aabb.max, Settings::pointRadius, color);
} 

void drawShapePoint(Node2D& node, const Point2& point) {
    node.draw_circle(point, Settings::pointRadius, Settings::shapePointColor);
}
void drawMidPoint(Node2D& node, const Point2& point) {
    node.draw_circle(point, Settings::pointRadius, Settings::midPointColor);
}

void drawBVH2DNode(Node2D& node, BVH2D::Node* bvhNode) {
    if(bvhNode == nullptr) {
        return;
    }
    drawAABB(node, bvhNode->aabb, Settings::bvhColor);
    drawBVH2DNode(node, bvhNode->b1);
    drawBVH2DNode(node, bvhNode->b2);
}
void drawBVH2D(Node2D& node, BVH2D& bvh) {
    drawBVH2DNode(node, bvh.root);
}

void drawBVH2DNodeMidpoints(Node2D& node, BVH2D::Node* bvhNode) {
    if(bvhNode == nullptr) {
        return;
    }
    drawMidPoint(node, bvhNode->midPoint);
    drawBVH2DNodeMidpoints(node, bvhNode->b1);
    drawBVH2DNodeMidpoints(node, bvhNode->b2);
}
void drawBVH2DMidpoints(Node2D& node, BVH2D& bvh) {
    drawBVH2DNodeMidpoints(node, bvh.root);
}