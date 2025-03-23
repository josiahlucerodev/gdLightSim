#pragma once

//std
#include <vector>
#include <variant>
#include <deque>

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "ray2D.h"
#include "../data/shape2D.h"
#include "../data/raySettings.h"

enum struct SectionType {
    unknown,
    miss,
    hit,
};

struct RadialSection {
    SectionType type;
    Color color;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
};

struct LinearSection {
    SectionType type;
    Color color;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
};

enum struct ScatterSectionBehavior {
    unknown,
    parallel,
    converges,
    diverges
};

struct ScatterSection {
    SectionType type;
    Color color;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
    std::optional<Point2> intersectionPoint;
    ScatterSectionBehavior behavior;
};

enum struct SectionActionType {
    mirror,
    filter,
    lens,
};

using SectionVariant = std::variant<RadialSection, LinearSection, ScatterSection>;

struct SectionAction {
    SectionActionType type;
    SectionVariant section;
};

using SectionActionQueue = std::deque<SectionAction>;

struct SectionsInfo {
    std::vector<RadialSection> radialSections;
    std::vector<LinearSection> linearSections;
    std::vector<ScatterSection> scatterSections;
    SectionActionQueue actionQueue;
    void reset();
    bool hasSections() const;
};

void addRadialSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
    const std::vector<RadialSection>& radialSections);
void addLinearSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<LinearSection>& linearSections);
void addScatterSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<ScatterSection>& scatterSections);

std::vector<Ray2D> generateMissSectionRays(const RadialSection& radialSection);

struct SectionGenerationInfo {
    const std::vector<Shape2D>& shapes;
    const RaySettings& raySettings;
    ScatterSectionBehavior behavior;
};

std::vector<RadialSection> generateRadialSections(SectionGenerationInfo generationInfo,
	const Color& color, const real_t& angle, std::vector<RayVariant>& rays);

std::vector<LinearSection> generateLinearSections(SectionGenerationInfo generationInfo,
	const Color& color, const Point2& rightPoint, std::vector<RayVariant>& rays);

struct GenerateScatterSectionResult {
    std::vector<ScatterSection> sections;
    std::vector<RadialSection> postIntersectionSections; 
    //post Intersection Sections occur when a section when the section cross and a radial section 
    //needs to be shot from that convergent point forward 
};

struct ShotScatterResult{
    ScatterSectionBehavior behavior;
    std::vector<RayVariant> rays;
};

GenerateScatterSectionResult generateScatterSections(SectionGenerationInfo generationInfo, 
    const Color& color, const RayHit2D startRay, const RayHit2D endRay, 
    ShotScatterResult& shotResult);

std::vector<RadialSection> generatePostIntersectionSections(SectionGenerationInfo generationInfo, 
    const Color& color, const Ray2D startRay, const Ray2D endRay, std::vector<RayVariant>& rays);

