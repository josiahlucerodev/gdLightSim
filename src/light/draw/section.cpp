#include "section.h"

//own
#include "ray.h"
#include "../settings.h"

void drawRayMissSegment(Node2D& node, const Ray2D& beginRay, const Ray2D& endRay) {
    drawRay(node, beginRay);
    drawRay(node, endRay);
    for(std::size_t i = 0; i < Settings::debugDistance / Settings::debugSegmentCount; i++) {
        real_t localDistance = (Settings::debugDistance / Settings::debugSegmentCount / 4) * i;
        node.draw_line(
            (beginRay.direction * localDistance) + beginRay.origin, 
            (endRay.direction * localDistance) + endRay.origin, 
            Settings::rayMissColor, Settings::debugLineWidth);   
    }
}

void drawSection(Node2D& node, const RadialSection& radialSection) {
    switch (radialSection.type) {
    case SectionType::hit: {
        const RayHit2D& startHit = std::get<1>(radialSection.startRay);    
        const RayHit2D& endHit = std::get<1>(radialSection.endRay);    
        
        drawRay(node, startHit); 
        drawRay(node, endHit); 
        node.draw_line(startHit.location, endHit.location, Settings::rayHitColor);
        break;
    }
    case SectionType::miss: {
        const std::vector<Ray2D> rays = generateMissSectionRays(radialSection);
        for(std::size_t i = 0; i < rays.size() - 1; i++) {
            Ray2D ray1 = rays[i];
            Ray2D ray2 = rays[i + 1];
            drawRayMissSegment(node, ray1, ray2);
        }
        break;
    }
    default:
        break;
    }
}
void drawSection(Node2D& node, const LinearSection& linearSection) {
    switch (linearSection.type) {
    case SectionType::hit: {
        const RayHit2D& startHit = std::get<1>(linearSection.startRay);    
        const RayHit2D& endHit = std::get<1>(linearSection.endRay);    
        drawRay(node, startHit); 
        drawRay(node, endHit);                 
        node.draw_line(startHit.location, endHit.location, Settings::rayHitColor);
        break;
    }
    case SectionType::miss: {
        const Ray2D& startRay = std::get<0>(linearSection.startRay);    
        const Ray2D& endRay = std::get<0>(linearSection.endRay);    
        drawRayMissSegment(node, startRay, endRay);
        break;
    }
    default:
        break;
    }
        
}
void drawSection(Node2D& node, const ScatterSection& scatterSection) {
    switch (scatterSection.type) {
    case SectionType::hit: {
        const RayHit2D& startHit = std::get<1>(scatterSection.startRay);    
        const RayHit2D& endHit = std::get<1>(scatterSection.endRay);    
        drawRay(node, startHit); 
        drawRay(node, endHit);                 
        node.draw_line(startHit.location, endHit.location, Settings::rayHitColor);
        break;
    }
    case SectionType::miss: {
        const Ray2D& startRay = std::get<0>(scatterSection.startRay);    
        const Ray2D& endRay = std::get<0>(scatterSection.endRay);    
        drawRayMissSegment(node, startRay, endRay);
        break;
    }
    default:
        break;
    }
}