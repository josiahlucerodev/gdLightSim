#pragma once

//own
#include "meshData.h"

void generateQuadMeshSection(MeshData& meshData, const Color& color, const Point2 p1, const Point2 p2, const Point2 p3, const Point2 p4);
void generateTriangleMeshSection(MeshData& meshData, const Color& color, const Point2 p1, const Point2 p2, const Point2 p3);