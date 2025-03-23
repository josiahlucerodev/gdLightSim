#include "meshData.h"

//godot
#include <godot_cpp/classes/array_mesh.hpp>
using namespace godot;

Array getSurfaceArrays(MeshData& meshData) {
    Array surfaceArrays;
    surfaceArrays.resize(ArrayMesh::ARRAY_MAX);
    surfaceArrays[ArrayMesh::ARRAY_VERTEX] = meshData.vertices;
    surfaceArrays[ArrayMesh::ARRAY_TEX_UV] = meshData.uvs;
    surfaceArrays[ArrayMesh::ARRAY_COLOR] = meshData.colors;
    return surfaceArrays;
}