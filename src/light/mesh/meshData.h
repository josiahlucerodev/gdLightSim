#pragma once

//godotcpp
#include <godot_cpp/classes/packed_data_container.hpp>
using namespace godot;

struct MeshData {
    PackedVector3Array vertices;
    PackedVector2Array uvs;
    PackedColorArray colors;
};

Array getSurfaceArrays(MeshData& meshData);
