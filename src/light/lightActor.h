#pragma once

//std
#include <vector>

//own
#include "emitter/lightEmitter2D.h"
#include "interactor/lightInteractor2D.h"
#include "ops/section2D.h"
#include "misc/descendants.h"
#include "lightEnvironmentData.h"

template<typename LightEmitterType>
std::vector<LightEmitterType*> getLightEmitter(Node& node, const std::string_view& name) {
    return getDescendantsOfType<LightEmitterType>(node, String(name.data()));;
}

template<typename LightInteractorType>
std::vector<LightInteractorType*> getLightInteractor(Node& node, const std::string_view& name) {
    return getDescendantsOfType<LightInteractorType>(node, String(name.data()));
}

template<typename LightInteractorType>
void constructLightInteractorShapes(std::vector<Shape2D>& shapes, std::vector<LightInteractorType*>& lightInteractors) {
    for(LightInteractorType* lightInteractor : lightInteractors) {
        shapes.push_back(LightInteractor<LightInteractorType>::constructShape(lightInteractor->getInfo(), shapes.size()));
    }
}

template<typename ShotResult>
void handleShotResult(std::vector<RayVariant>& allShotRays, ShotResult& shotResult) {
    using decayed_shot_result = std::decay_t<ShotResult>;
    if constexpr(std::is_same_v<ShotResult, std::vector<RayVariant>>) {
        allShotRays.insert(allShotRays.end(), shotResult.begin(), shotResult.end());
    } else if constexpr(std::is_same_v<ShotResult, ShotScatterResult>) {
        allShotRays.insert(allShotRays.end(), shotResult.rays.begin(), shotResult.rays.end());
    }
} 

template<typename ShotResult>
SectionGenerationInfo getSectionGenerationInfo(const std::vector<Shape2D>& shapes, const RaySettings& raySettings, ShotResult& shotResult) {
    using decayed_shot_result = std::decay_t<ShotResult>;
    if constexpr(std::is_same_v<ShotResult, ShotScatterResult>) {
        return SectionGenerationInfo{shapes, raySettings, shotResult.behavior};
    } else {
        return SectionGenerationInfo{shapes, raySettings, ScatterSectionBehavior::unknown};
    }
}

template<typename GenerationResult>
void handleGenerationResult(LightEnviromentData& envData, SectionsInfo& sectionsInfo, const RaySettings& raySettings, GenerationResult& generationResult) {
    using decayed_generation_result = std::decay_t<GenerationResult>;

    if constexpr(std::is_same_v<decayed_generation_result, std::vector<RadialSection>>) {
        sectionsInfo.radialSections.insert(sectionsInfo.radialSections.end(), generationResult.begin(), generationResult.end());
        addRadialSectionActionsToQueue(sectionsInfo.actionQueue, envData.shapes, generationResult);
    } else if constexpr(std::is_same_v<decayed_generation_result, std::vector<LinearSection>>) {
        sectionsInfo.linearSections.insert(sectionsInfo.linearSections.end(), generationResult.begin(), generationResult.end());
        addLinearSectionActionsToQueue(sectionsInfo.actionQueue, envData.shapes, generationResult);
    } else if constexpr(std::is_same_v<decayed_generation_result, GenerateScatterSectionResult>) {
        sectionsInfo.scatterSections.insert(sectionsInfo.scatterSections.end(), generationResult.sections.begin(), generationResult.sections.end());
        addScatterSectionActionsToQueue(sectionsInfo.actionQueue, envData.shapes, generationResult.sections);

        std::vector<ScatterSection> actionScatterSections;
        for(ScatterSection& scatterSection : generationResult.sections) {
            if(!scatterSection.intersectionPoint.has_value()) {
                actionScatterSections.push_back(scatterSection);
            }
        }
        addScatterSectionActionsToQueue(sectionsInfo.actionQueue, envData.shapes, actionScatterSections);
    
        for(RadialSection& postIntersectionSection : generationResult.postIntersectionSections) {
            const Ray2D startRay = getRay(postIntersectionSection.startRay);
            const Ray2D endRay = getRay(postIntersectionSection.endRay);
            ShotInfo shotInfo{envData.points, envData.bvh, raySettings};
            std::vector<RayVariant> shotResult = shotScatterPostIntersectionLight(shotInfo, postIntersectionSection.shapeId, startRay, endRay);
            handleShotResult(envData.allShotRays, shotResult);

            SectionGenerationInfo generationInfo = getSectionGenerationInfo(envData.shapes, raySettings, shotResult);
            std::vector<RadialSection> postGenerationResult = generatePostIntersectionSections(generationInfo, postIntersectionSection.color, startRay, endRay, shotResult);
            handleGenerationResult(envData, sectionsInfo, raySettings, postGenerationResult);
        }
    }
} 

template<typename LightEmitterType> 
void preformEmitterAction(LightEnviromentData& envData, SectionsInfo& sectionsInfo, const RaySettings& raySettings, LightEmitterType& lightEmitter) {
    using light_emitter = LightEmitter<LightEmitterType>;

    typename light_emitter::source_info source = lightEmitter.getInfo();
    EmitterShotInfo shotInfo{envData.points, envData.bvh, raySettings};
    typename light_emitter::shot_result shotResult = light_emitter::shot(source, shotInfo);
    handleShotResult(envData.allShotRays, shotResult);

    SectionGenerationInfo generationInfo = getSectionGenerationInfo(envData.shapes, raySettings, shotResult);
    typename light_emitter::generation_result generationResult = light_emitter::generate(source, shotResult, generationInfo);
    handleGenerationResult(envData, sectionsInfo, raySettings, generationResult);
}

template<typename LightInteractorType, typename Session>
void preformInteractorActionOnSection(LightEnviromentData& envData, SectionsInfo& sectionsInfo, const RaySettings& raySettings, Session& section) {
    using light_interactor = LightInteractor<LightInteractorType>;

    LightInteractorShotInfo shotInfo{envData.shapes, envData.bvh, raySettings};
    auto shotResult = light_interactor::shot(section, shotInfo);
    handleShotResult(envData.allShotRays, shotResult);

    SectionGenerationInfo generationInfo = getSectionGenerationInfo(envData.shapes, raySettings, shotResult);
    auto generationResult = light_interactor::generate(section, shotResult, generationInfo);
    handleGenerationResult(envData, sectionsInfo, raySettings, generationResult);
}

template<typename LightInteractorType>
void preformInteractorAction(LightEnviromentData& envData, SectionsInfo& sectionsInfo, const RaySettings& raySettings, SectionVariant& section) {
    std::visit([&](auto&& section) -> void {
        preformInteractorActionOnSection<LightInteractorType, decltype(section)>(envData, sectionsInfo, raySettings, section);
    }, section);
}