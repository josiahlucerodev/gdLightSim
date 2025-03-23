#pragma once

#include "../data/bvh2D.h"
#include "../data/shape2D.h"
#include "../ops/section2D.h"
#include "../data/raySettings.h"
#include "../ops/shotLight.h"

struct LightInteractorShotInfo {
    const std::vector<Shape2D>& shapes;
    BVH2D& bvh;
    RaySettings raySettings;

    ShotInfo generateBaseInfo(std::vector<Point2>& points) {
        return ShotInfo{points, bvh, raySettings};
    }
};


template<typename InteractorType>
struct LightInteractor;