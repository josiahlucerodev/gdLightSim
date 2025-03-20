#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

//own
#include "ray2D.h"
#include "section2D.h"
#include "bvh2D.h"

class Lens2D : public Node2D {
	GDCLASS(Lens2D, Node2D);
protected:
    static void _bind_methods();
private:
	bool drawDebug;
    real_t lensWidth;
	real_t focalLength;
public:
    Lens2D();
    ~Lens2D();

    void _ready() override;
    void _process(double delta) override;
    void _draw() override;

    bool get_draw_debug() const;
	void set_draw_debug(const bool debugDraw);
    real_t get_lens_width() const;
	void set_lens_width(const real_t);
    real_t get_focal_length() const;
	void set_focal_length(const real_t focalLength);
};

Shape2D constructShape2D(Lens2D& lens, ShapeId shapeId);

ShotScatterReturn shotLensRadialSection(
	const RadialSection& lensRadialSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread);
GenerateScatterSectionsReturn generateLensScatterSectionsFromRadial(
	const RadialSection& lensRadialSection,  std::vector<RayVariant>& rays, const ScatterSectionBehavior& behavior,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);
	
ShotScatterReturn shotLensLinearSection(
	const LinearSection& lensLinearSection, const std::vector<Shape2D>& shapes,
	const BVH2D& bvh, const real_t scatterRaySpread);
GenerateScatterSectionsReturn generateLensScatterSectionsFromLinear(
	const LinearSection& lensLinearSection, std::vector<RayVariant>& rays, const ScatterSectionBehavior& behavior,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);
				
ShotScatterReturn shotLensScatterSection(
	const ScatterSection& lensScatterSection, const std::vector<Shape2D>& shapes, 
	const BVH2D& bvh, const real_t scatterRaySpread);
GenerateScatterSectionsReturn generateLensScatterSectionsFromScatter(
	const ScatterSection& lensScatterSection, std::vector<RayVariant>& rays, const ScatterSectionBehavior& behavior,
	const std::vector<Shape2D>& shapes, const real_t scatterSectionTolerance);