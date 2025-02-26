#pragma once

//std
#include <vector>

//own
#include "ray2D.h"
#include "shape2D.h"

enum struct SectionType {
    unknown,
    miss,
    hit,
};

struct RadialScanSection {
    SectionType type;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
};

struct LinearScanSection {
    SectionType type;
    std::size_t shapeId;
    std::size_t bounceIndex;
    RayVariant startRay;
    RayVariant endRay;
};

template<typename Section, typename Predicate>
std::vector<Section> generateSectionsBase(const std::vector<Shape2D>& shapes,
    std::vector<RayVariant>& rays, Predicate&& simplificationPredicate) {
    RayVariant front = rays.front();
    rays.push_back(front);

    std::vector<Section> sections;
    size_t i = 0;
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
                    shapeId, 0,
                    hitGroup.front(),
                    hitGroup.back()
                }
            );
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
                        shapeId, 0,
                        r1, r2
                    }
                );
            }
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
            sections.push_back(
                Section {
                    SectionType::miss,
                    0, 0,
                    missGroup.front(),
                    missGroup.back()
                }
            );
        }
    } 
    return sections;
};