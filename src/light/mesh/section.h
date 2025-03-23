#pragma once

//godotcpp
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
using namespace godot;

//own
#include "meshData.h"
#include "../ops/section2D.h"

struct SectionDrawSettings {
    bool drawColor;
};

void setEmptyMesh(MeshInstance2D& node, Ref<ArrayMesh>& arrayMesh);
void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const RadialSection& radialSection);
void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const LinearSection& linearSection);
void generateMeshSection(MeshData& meshData, const SectionDrawSettings& drawSettings, const ScatterSection& scatterSection);