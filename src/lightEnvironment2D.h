#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>


//std
#include <optional>
#include <variant>
#include <algorithm>
#include <vector>

//own
#include "batchPool.h"

using namespace godot;

struct ShapeAABB {
    Point2 min;
    Point2 max;
};

struct Shape {
    std::size_t shapeId;
    Point2 midPoint;
    ShapeAABB aabb;
    std::vector<Point2> points;
};

template<typename Type>
struct ArrayView {
    Type* data;
    std::size_t size;
};

struct BVH {
    struct Node {
        Node* b1;
        Node* b2;
        ShapeAABB aabb;
        Point2 midPoint;
        ArrayView<Shape> shapes;
    };
    
    BatchPool<Node> nodePool;
    std::vector<Shape> shapes;
    Node* root;

    BVH() = default;
    BVH(const BVH&) = delete;
    BVH& operator=(const BVH&) = delete;
};

struct Ray2D{
    Point2 origin;
    Point2 direction;
};

struct RayHit2D {
    std::size_t shapeId;
    real_t angle;  
    Ray2D ray;
    Point2 location;
};

using RayVariant = std::variant<Ray2D, RayHit2D>;

enum struct SectionType {
    unknown,
    miss,
    hit,
};

struct RadialScanSection {
    SectionType type;
    RayVariant startRay;
    RayVariant endRay;
};

struct LinearScanSection {
    SectionType type;
    RayVariant startRay;
    RayVariant endRay;
};


class LightEnvironment2D : public Node2D {
	GDCLASS(LightEnvironment2D, Node2D);
protected:
	static void _bind_methods();
	bool displayBVH;
	bool displayAABB;
    bool displayPoints;
    bool displayMidpoints;
    bool displayRays;
    bool displayRadialScanSections;
    bool displayLinearScanSections;
    double radialSectionTolerance;
    double linearSectionTolerance;
public:
	BVH bvh;
	std::vector<Shape> shapes;
	std::vector<Point2> points;
    std::vector<Ray2D> rayMisses;
    std::vector<RayHit2D> rayHits;
    std::vector<RadialScanSection> radialScanSections;
    std::vector<LinearScanSection> linearScanSections;
public:

    LightEnvironment2D();
	~LightEnvironment2D();

	void _ready() override;
	void _process(double delta) override;
	void _draw() override;
    void _notification(int pwhat);

	bool get_display_bvh() const;
	void set_display_bvh(const bool);
	bool get_display_aabb() const;
    void set_display_aabb(const bool);
    bool get_display_points() const;
	void set_display_points(const bool);
    bool get_display_midpoints() const;
	void set_display_midpoints(const bool);
    bool get_display_rays() const;
	void set_display_rays(const bool);
    bool get_display_radial_scan_sections() const;
	void set_display_radial_scan_sections(const bool);
    bool get_display_linear_scan_sections() const;
	void set_display_linear_scan_sections(const bool);

    double get_radial_section_tolerance() const;
	void set_radial_section_tolerance(const double);
    double get_linear_section_tolerance() const;
	void set_linear_section_tolerance(const double);
};
