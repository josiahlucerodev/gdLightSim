#include "section.h"

//own
#include "primitives.h"
#include "../settings.h"

void setEmptyMesh(MeshInstance2D& node, Ref<ArrayMesh>& arrayMesh) {
    arrayMesh->reset_state();
    node.set_mesh(nullptr);
}

Color pickColor(const SectionDrawSettings& drawSettings, const Color& color) {
    return drawSettings.drawColor ? 
        Color(color.r, color.g, color.b, 0.8) :
        Color(Settings::debugFilledLightColor.r, Settings::debugFilledLightColor.g, Settings::debugFilledLightColor.b, 0.8); 
};


void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const RadialSection& radialSection) {
    switch (radialSection.type) {
        case SectionType::hit: {
            const RayHit2D& startHit = std::get<1>(radialSection.startRay);    
            const RayHit2D& endHit = std::get<1>(radialSection.endRay); 
            
            generateTriangleMeshSection(meshData, pickColor(drawSettings, radialSection.color),
                startHit.ray.origin, startHit.location, endHit.location);
            break;
        }
        case SectionType::miss: {
            const std::vector<Ray2D> rays = generateMissSectionRays(radialSection);

            for(std::size_t i = 0; i < rays.size() - 1; i++) {
                Ray2D ray1 = rays[i];
                Ray2D ray2 = rays[i + 1];
                Point2 dis1 = ray1.origin + (ray1.direction * Settings::debugDistance);
                Point2 dis2 = ray2.origin + (ray2.direction * Settings::debugDistance);
                generateTriangleMeshSection(meshData, pickColor(drawSettings, radialSection.color), ray1.origin, dis1, dis2);
            }
            break;
        }
        default:
            break;
        }
}
void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const LinearSection& linearSection) {
    switch (linearSection.type) {
    case SectionType::hit: {
        const RayHit2D& startHit = std::get<1>(linearSection.startRay);    
        const RayHit2D& endHit = std::get<1>(linearSection.endRay);    
        generateQuadMeshSection(meshData, pickColor(drawSettings, linearSection.color),
            startHit.ray.origin, startHit.location, 
            endHit.location, endHit.ray.origin);
        break;
    }
    case SectionType::miss: {
        const Ray2D& startRay = std::get<0>(linearSection.startRay);    
        const Ray2D& endRay = std::get<0>(linearSection.endRay);    
        Point2 startDis = startRay.origin + (startRay.direction * Settings::debugDistance);
        Point2 endDis = endRay.origin + (endRay.direction * Settings::debugDistance);
        generateQuadMeshSection(meshData, pickColor(drawSettings, linearSection.color),
            startRay.origin, startDis, endDis, endRay.origin);
        break;
    }
    default:
        break;
    }
}
void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const ScatterSection& scatterSection) {
    switch (scatterSection.type) {
    case SectionType::hit: {
        const RayHit2D& startHit = std::get<1>(scatterSection.startRay);    
        const RayHit2D& endHit = std::get<1>(scatterSection.endRay);    
        if(scatterSection.intersectionPoint.has_value()) {
            generateTriangleMeshSection(meshData, pickColor(drawSettings, scatterSection.color),
                startHit.ray.origin, endHit.ray.origin, scatterSection.intersectionPoint.value());
        } else {
            generateQuadMeshSection(meshData, pickColor(drawSettings, scatterSection.color),
                startHit.ray.origin, startHit.location, 
                endHit.location, endHit.ray.origin);
        }
        break;
    }
    case SectionType::miss: {
        const Ray2D& startRay = std::get<0>(scatterSection.startRay);    
        const Ray2D& endRay = std::get<0>(scatterSection.endRay);    

        if(scatterSection.intersectionPoint.has_value()) {
            generateTriangleMeshSection(meshData, pickColor(drawSettings, scatterSection.color),
                startRay.origin, endRay.origin, scatterSection.intersectionPoint.value());
        } else {
            Point2 startDis = startRay.origin + (startRay.direction * Settings::debugDistance);
            Point2 endDis = endRay.origin + (endRay.direction * Settings::debugDistance);
            generateQuadMeshSection(meshData, pickColor(drawSettings, scatterSection.color),
                startRay.origin, startDis, endDis, endRay.origin);
        }
        break;
    }
    default:
        break;
    }
}