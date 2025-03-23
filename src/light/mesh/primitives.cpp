#include "primitives.h"

void generateQuadMeshSection(MeshData& meshData, const Color& color, const Point2 p1, const Point2 p2, const Point2 p3, const Point2 p4) {

    meshData.vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    meshData.vertices.push_back(Vector3(p2.x, p2.y, 0)); //p2
    meshData.vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3

    meshData.vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    meshData.vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3
    meshData.vertices.push_back(Vector3(p4.x, p4.y, 0)); //p4

    meshData.uvs.push_back(Vector2(0, 0)); //p1
    meshData.uvs.push_back(Vector2(1, 0)); //p2
    meshData.uvs.push_back(Vector2(1, 1)); //p3

    meshData.uvs.push_back(Vector2(0, 0)); //p1
    meshData.uvs.push_back(Vector2(1, 1)); //p3
    meshData.uvs.push_back(Vector2(0, 1)); //p4

    meshData.colors.push_back(color); //p1
    meshData.colors.push_back(color); //p2
    meshData.colors.push_back(color); //p3

    meshData.colors.push_back(color); //p1
    meshData.colors.push_back(color); //p3
    meshData.colors.push_back(color); //p4
}
void generateTriangleMeshSection(MeshData& meshData, const Color& color, const Point2 p1, const Point2 p2, const Point2 p3) {
    meshData.vertices.push_back(Vector3(p1.x, p1.y, 0)); //p1
    meshData.vertices.push_back(Vector3(p2.x, p2.y, 0)); //p2
    meshData.vertices.push_back(Vector3(p3.x, p3.y, 0)); //p3

    meshData.uvs.push_back(Vector2(0, 0)); //p1
    meshData.uvs.push_back(Vector2(1, 0)); //p2
    meshData.uvs.push_back(Vector2(1, 1)); //p3

    meshData.colors.push_back(color); //p1
    meshData.colors.push_back(color); //p2
    meshData.colors.push_back(color); //p3
}