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
#include "shape2D.h"

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

struct ScatterSection {
    SectionType type;
    Color color;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
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

void addRadialSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
    const std::vector<RadialSection>& radialSections);
void addLinearSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<LinearSection>& linearSections);
void addScatterSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<ScatterSection>& scatterSections);

std::vector<Ray2D> generateMissSectionRays(const RadialSection& radialSection);

std::vector<RadialSection> generateRadialSections(
	const Color& color, const real_t& angle, std::vector<RayVariant>& rays, 
	const std::vector<Shape2D>& shapes, const real_t radialSectionTolerance);

std::vector<LinearSection> generateLinearSections(
	const Color& color, const Point2& rightPoint, std::vector<RayVariant>& rays, 
	const std::vector<Shape2D>& shapes, const real_t linearSectionTolerance);

std::vector<ScatterSection> generateScatterSections(const Color& color,
	const RayHit2D startRay, const RayHit2D endRay,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);