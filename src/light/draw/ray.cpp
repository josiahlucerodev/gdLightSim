#include "ray.h"

//own
#include "../settings.h"

void drawRay(Node2D& node, const Ray2D& ray) {
    node.draw_line(ray.origin, ray.origin + (ray.direction * Settings::debugDistance), Settings::rayMissColor, Settings::debugLineWidth);
}
void drawRay(Node2D& node, const RayHit2D& rayHit) {
    node.draw_line(rayHit.ray.origin, rayHit.location, Settings::rayHitColor, Settings::debugLineWidth);
    node.draw_circle(rayHit.location, Settings::pointRadius, Settings::rayHitColor);
}
void drawRay(Node2D& node, const RayVariant& rayVaraint) {
    if(std::holds_alternative<Ray2D>(rayVaraint)) {
        drawRay(node, std::get<0>(rayVaraint));
    } else {
        drawRay(node, std::get<1>(rayVaraint));
    }
}