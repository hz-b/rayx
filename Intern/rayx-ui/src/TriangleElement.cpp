#include "TriangleElement.h"

std::vector<Triangle> TriangleElement::trianglesFromQuadric(const RenderObject& renderObject) {
    // Define the size and resolution of the grid
    double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE];

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                glm::vec4 pos(x, y, z, 1);
                double value = evaluateQuadricAtPosition(renderObject.surface, pos);
                scalarGrid[x][y][z] = value;
            }
        }
    }

    // 2. March through each voxel
    std::vector<Triangle> triangles;
    for (int x = 0; x < GRIDSIZE - 1; x++) {
        for (int y = 0; y < GRIDSIZE - 1; y++) {
            for (int z = 0; z < GRIDSIZE - 1; z++) {
                int caseIndex = determineMarchingCubesCase(scalarGrid, x, y, z);
                std::vector<Triangle> voxelTriangles = lookupTrianglesForCase(caseIndex);
                triangles.insert(triangles.end(), voxelTriangles.begin(), voxelTriangles.end());
            }
        }
    }

    return triangles;
}

double TriangleElement::evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos) {
    // Convert the surface array to a glm::mat4.
    glm::mat4 surfaceMatrix(surface[0], surface[1], surface[2], surface[3], surface[4], surface[5], surface[6], surface[7], surface[8], surface[9],
                            surface[10], surface[11], surface[12], surface[13], surface[14], surface[15]);

    glm::vec4 resultVec = pos * surfaceMatrix;
    double result = glm::dot(resultVec, pos);

    return result;
}

int TriangleElement::determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z) {
    // Based on the scalar values at the voxel corners, determine the index for the lookup tables.
    int cubeIndex = 0;
    if (scalarGrid[x][y][z] < 0) cubeIndex |= 1;
    if (scalarGrid[x + 1][y][z] < 0) cubeIndex |= 2;
    if (scalarGrid[x + 1][y][z + 1] < 0) cubeIndex |= 4;
    if (scalarGrid[x][y][z + 1] < 0) cubeIndex |= 8;
    if (scalarGrid[x][y + 1][z] < 0) cubeIndex |= 16;
    if (scalarGrid[x + 1][y + 1][z] < 0) cubeIndex |= 32;
    if (scalarGrid[x + 1][y + 1][z + 1] < 0) cubeIndex |= 64;
    if (scalarGrid[x][y + 1][z + 1] < 0) cubeIndex |= 128;

    return cubeIndex;
}

std::vector<Triangle> TriangleElement::lookupTrianglesForCase(int caseIndex) {
    // Using the triTable to generate the triangles for the voxel.

    std::vector<Triangle> triangles;

    // triTable[caseIndex] provides the edges to be connected for the triangles.
    // Every 3 indices in the table make up a triangle.
    for (int i = 0; triTable[caseIndex][i] != -1; i += 3) {
        Triangle triangle;

        // Convert edge indices to vertices
        triangle.vertices[0] = interpolateVertex(/*triTable[caseIndex][i]*/);
        triangle.vertices[1] = interpolateVertex(/*triTable[caseIndex][i + 1]*/);
        triangle.vertices[2] = interpolateVertex(/*triTable[caseIndex][i + 2]*/);

        triangles.push_back(triangle);
    }

    return triangles;
}

Vertex TriangleElement::interpolateVertex() {
    // Here, we'd interpolate between the two voxel corners that the edge connects.
    // This function should return the vertex position on the edge where the surface intersects.
    // The exact interpolation might depend on the scalar values at the voxel corners.
    // This is a placeholder function. The actual function would involve more logic to determine the exact position.
    Vertex v;
    v.pos = glm::vec3(0, 0, 0);  // Placeholder value
    return v;
}