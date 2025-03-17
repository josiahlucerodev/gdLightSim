#include "section2D.h"

//std
#include <algorithm>

//own
#include "settings.h"
#include "util.h"

using namespace godot;

template<typename Section> 
const void addSectionActionsToQueue(SectionActionQueue& queue, const std::vector<Shape2D> shapes,
	const std::vector<Section>& sessions) {
    for(const Section& section : sessions) {
		if (section.type == SectionType::hit) {
			switch (shapes[section.shapeId].type){
			case Shape2DType::mirror:
				queue.push_back(SectionAction{SectionActionType::mirror, section});
				break;
			case Shape2DType::filter:
				queue.push_back(SectionAction{SectionActionType::filter, section});
				break;
			case Shape2DType::lens:
				queue.push_back(SectionAction{SectionActionType::lens, section});
				break;
			default:
				break;
			}
		}
	}
}

void addRadialSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
    const std::vector<RadialSection>& radialSections) {
	addSectionActionsToQueue<RadialSection>(actionQueue, shapes, radialSections);
}
void addLinearSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<LinearSection>& linearSections) {
	addSectionActionsToQueue<LinearSection>(actionQueue, shapes, linearSections);
}
void addScatterSectionActionsToQueue(SectionActionQueue& actionQueue, const std::vector<Shape2D> shapes,
	const std::vector<ScatterSection>& scatterSections) {
	addSectionActionsToQueue<ScatterSection>(actionQueue, shapes, scatterSections);
}

std::vector<Ray2D> generateMissSectionRays(const RadialSection& radialSection) {
	Ray2D startRay = std::get<0>(radialSection.startRay);    
	Ray2D endRay = std::get<0>(radialSection.endRay);  
	
	std::vector<Ray2D> rays;
	rays.push_back(startRay);
	real_t startEndAngle = startRay.direction.angle_to(endRay.direction);
	if(startEndAngle < 0 || startEndAngle > Settings::arcSegmentLimit) {
		real_t fullArc = startEndAngle < 0 ? (Math_PI * 2) - abs(startEndAngle) : startEndAngle;
		std::size_t numberOfArcs = fullArc / Settings::arcSegmentLimit; 
		real_t segmentArc = fullArc / numberOfArcs;
		for(std::size_t i = 0; i < numberOfArcs; i++) {
			Vector2 rayDir = startRay.direction.rotated(segmentArc * (i + 1));
			rays.push_back(Ray2D{startRay.origin, rayDir});   
		}
	}

	rays.push_back(endRay);
	return rays;
}

template<typename Section, typename Predicate>
std::vector<Section> generateSectionsBase(const Color& color, const std::vector<Shape2D>& shapes,
    std::vector<RayVariant>& rays, Predicate&& simplificationPredicate) {
    std::vector<Section> sections;
    size_t i = 0;

	std::optional<RayHit2D> lastHitRay;
    while(i < rays.size()) {
        std::vector<RayHit2D> hitGroup;
        Shape2DType shapeType = Shape2DType::unknown;
        std::size_t shapeId;
        while(i < rays.size()) {
            if(std::holds_alternative<RayHit2D>(rays[i])) {
                RayHit2D rayHit = std::get<1>(rays[i]);
                Shape2DType newShape2DType = shapes[rayHit.shapeId].type;
                if(shapeType == Shape2DType::unknown) {
                    shapeType = newShape2DType;
                    shapeId = rayHit.shapeId;
                } else if(newShape2DType != shapeType) {
                    break;
                }

                hitGroup.push_back(std::get<1>(rays[i])); 
                i++;
                continue;
            }
            break;
        }

        if(hitGroup.size() == 2) {
            sections.push_back(
                Section {
                    SectionType::hit,
                    color,
                    shapeId, 0,
                    hitGroup.front(),
                    hitGroup.back()
                }
            );

			lastHitRay = hitGroup.back();
        } else if(hitGroup.size() > 2) {
            std::vector<RayHit2D> simplifiedHitGroup;
            simplifiedHitGroup.push_back(hitGroup.front());
            for (size_t i = 1; i < hitGroup.size() - 1; i++) {
                RayHit2D& r1 = simplifiedHitGroup.back();
                RayHit2D& r2 = hitGroup[i];
                RayHit2D& r3 = hitGroup[i + 1];
            
                if (simplificationPredicate(r1, r2, r3)) {
                    simplifiedHitGroup.push_back(r2);
                }
            }

            simplifiedHitGroup.push_back(hitGroup.back());

            for(size_t i = 0; i < simplifiedHitGroup.size() - 1; i++) {
                RayHit2D& r1 = simplifiedHitGroup[i];
                RayHit2D& r2 = simplifiedHitGroup[i+1];
                sections.push_back(
                    Section {
                        SectionType::hit,
                        color,
                        shapeId, 0,
                        r1, r2
                    }
                );
            }

			lastHitRay = hitGroup.back();
        }

        std::vector<Ray2D> missGroup;
        while(i < rays.size()) {
            if (std::holds_alternative<Ray2D>(rays[i])) {
                Ray2D ray = std::get<0>(rays[i]);
                missGroup.push_back(ray); 
                i++;
            } else {
                break;
            }
        }

        if(missGroup.size() >= 2) {
			std::optional<RayHit2D> nextHitRay;
			if(i < rays.size()) {
				nextHitRay = std::get<1>(rays[i]);
			}

			const real_t angleEpsolon = 0.01;
			Ray2D startRay = missGroup.front();
			if(lastHitRay.has_value()) {
				//startRay = lastHitRay.value().ray;
			}

			Ray2D endRay = missGroup.back();
			if(nextHitRay.has_value()) {
				//endRay = nextHitRay.value().ray;
			}

            sections.push_back(
                Section {
                    SectionType::miss,
                    color,
                    0, 0,
                    startRay, endRay
                }
            );
        }
    } 
    return sections;
};

std::vector<RadialSection> generateRadialSections(
	const Color& color, const real_t& angle, std::vector<RayVariant>& rays, 
	const std::vector<Shape2D>& shapes, const real_t radialSectionTolerance) {
	Point2 lightMidDir = vectorFromAngle(angle);
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return lightMidDir.angle_to(getRay(lhs).direction) 
				< lightMidDir.angle_to(getRay(rhs).direction);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > radialSectionTolerance;
	};

	return generateSectionsBase<RadialSection>(color, shapes, rays, predicate);
}

std::vector<LinearSection> generateLinearSections(
	const Color& color, const Point2& rightPoint, std::vector<RayVariant>& rays, 
	const std::vector<Shape2D>& shapes, const real_t linearSectionTolerance) {
	
	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			Point2 lhsOrigin = getRay(lhs).origin;
			Point2 rhsOrigin = getRay(rhs).origin;
			return lhsOrigin.distance_to(rightPoint) < rhsOrigin.distance_to(rightPoint);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > linearSectionTolerance;
	};

	return generateSectionsBase<LinearSection>(color, shapes, rays, predicate);
}

std::vector<ScatterSection> generateScatterSections(const Color& color,
	const RayHit2D startRay, const RayHit2D endRay,  std::vector<RayVariant>& rays,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance) {
	const real_t mirrorSlope = calculateSlope(startRay.location, endRay.location);
	const real_t mirrorSlopeAngle = atan(mirrorSlope);
	const Vector2 mirrorSlopeDir = vectorFromAngle(mirrorSlopeAngle);

	std::sort(rays.begin(), rays.end(), 
		[&](const RayVariant& lhs, const RayVariant& rhs) -> bool {
			return startRay.location.distance_to(getRay(lhs).origin) 
				< startRay.location.distance_to(getRay(rhs).origin);
		}
	);

	auto predicate = [&](const RayHit2D& r1, const RayHit2D& r2, const RayHit2D& r3)-> bool {
		real_t slope1 = calculateSlope(r1.location, r2.location);
		real_t slope2 = calculateSlope(r2.location, r3.location);
		return std::abs(slope1 - slope2) > scatterSectionTolerance;
	};

	return generateSectionsBase<ScatterSection>(color, shapes, rays, predicate);
}