#pragma once 

//std
#include <vector>

//own
#include "data/aabb2D.h"
#include "data/bvh2D.h"
#include "data/shape2D.h"
#include "ops/ray2D.h"
#include "ops/section2D.h"

struct LightEnviroment2DData {
    BVH2D bvh;
	std::vector<Shape2D> shapes;
	std::vector<Point2> points;
    std::vector<RayVariant> allShotRays;
    inline void reset() {
        resetBVH2D(bvh);
        shapes.clear();
        points.clear();
        allShotRays.clear();
    }
};
