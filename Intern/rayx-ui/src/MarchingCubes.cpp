#include "MarchingCubes.h"

#include "Colors.h"
#include "Debug/Debug.h"

std::vector<RenderObject> marchingCubeTriangulation(const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<RenderObject> objects;

    for (RAYX::OpticalElement element : elements) {
        auto quadric = element.m_element.m_surface.m_params;
        std::vector<Triangle> triangles = trianglesFromQuadric(quadric);
        RenderObject object(glm::mat4(element.m_element.m_outTrans));
        for (Triangle triangle : triangles) {
            object.addTriangle(triangle);
        }
        objects.push_back(object);
    }

    return objects;
}

std::vector<Triangle> trianglesFromQuadric(const float* quadric) {
    // Define the size and resolution of the grid
    float scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE];

    const float SCALE = 5.0 * 10.0 / GRIDSIZE;  // Define your desired scaling factor here
    float move = (GRIDSIZE / 2.0);

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                // Convert grid coordinate to centered & scaled space coordinate

                float realX = (x - (GRIDSIZE / 2.0)) * SCALE;
                float realY = (y - (GRIDSIZE / 2.0)) * SCALE;
                float realZ = (z - (GRIDSIZE / 2.0)) * SCALE;

                glm::vec4 pos(realX, realY, realZ, 1);
                float value = evaluateQuadricAtPosition(quadric, pos);
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
                std::vector<Triangle> voxelTriangles = lookupTrianglesForCase(caseIndex, scalarGrid, x, y, z, move, SCALE);
                triangles.insert(triangles.end(), voxelTriangles.begin(), voxelTriangles.end());
            }
        }
    }

    return triangles;
}

float evaluateQuadricAtPosition(const float surface[16], const glm::vec4& pos) {
    float icurv = surface[0];
    float a11 = surface[1];
    float a12 = surface[2];
    float a13 = surface[3];
    float a14 = surface[4];
    float a22 = surface[5];
    float a23 = surface[6];
    float a24 = surface[7];
    float a33 = surface[8];
    float a34 = surface[9];
    float a44 = surface[10];
    float result = a11 * pos.x * pos.x + a22 * pos.y * pos.y + a33 * pos.z * pos.z + a12 * pos.x * pos.y + a13 * pos.x * pos.z +
                    a23 * pos.y * pos.z + a14 * pos.x + a24 * pos.y + a34 * pos.z + a44;

    return result;
}

int determineMarchingCubesCase(const float scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z) {
    // Based on the scalar values at the voxel corners, determine the index for the lookup tables.
    int cubeIndex = 0;

    auto test = scalarGrid[x][y][z];
    if (scalarGrid[x][y][z] < 0) cubeIndex |= 1;
    auto test2 = scalarGrid[x + 1][y][z];
    if (scalarGrid[x + 1][y][z] < 0) cubeIndex |= 2;
    if (scalarGrid[x + 1][y][z + 1] < 0) cubeIndex |= 4;
    if (scalarGrid[x][y][z + 1] < 0) cubeIndex |= 8;
    if (scalarGrid[x][y + 1][z] < 0) cubeIndex |= 16;
    if (scalarGrid[x + 1][y + 1][z] < 0) cubeIndex |= 32;
    if (scalarGrid[x + 1][y + 1][z + 1] < 0) cubeIndex |= 64;
    if (scalarGrid[x][y + 1][z + 1] < 0) cubeIndex |= 128;

    return cubeIndex;
}

std::vector<Triangle> lookupTrianglesForCase(int caseIndex, const float scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY,
                                             int offsetZ, float move, float scale) {
    // Using the triTable to generate the triangles for the voxel.

    std::vector<Triangle> triangles;

    // triTable[caseIndex] provides the edges to be connected for the triangles.
    // Every 3 indices in the table make up a triangle.
    for (int i = 0; triTable[caseIndex][i] != -1; i += 3) {
        Triangle triangle;

        // Convert edge indices to vertices
        triangle.v1 = interpolateVertex(triTable[caseIndex][i], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.v2 = interpolateVertex(triTable[caseIndex][i + 1], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.v3 = interpolateVertex(triTable[caseIndex][i + 2], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.v1.color = DARKER_BLUE;
        triangle.v2.color = BLUE;
        triangle.v3.color = LIGHTER_BLUE;

        triangles.push_back(triangle);
    }

    return triangles;
}

glm::vec3 getPositionAtCorner(int cornerIndex) {
    glm::vec3 cornerPositions[8] = {
        glm::vec3(0, 0, 0),  // 0
        glm::vec3(1, 0, 0),  // 1
        glm::vec3(1, 0, 1),  // 2
        glm::vec3(0, 0, 1),  // 3
        glm::vec3(0, 1, 0),  // 4
        glm::vec3(1, 1, 0),  // 5
        glm::vec3(1, 1, 1),  // 6
        glm::vec3(0, 1, 1)   // 7
    };
    // Check for valid index
    if (cornerIndex < 0 || cornerIndex >= 8) {
        throw std::out_of_range("Invalid corner index");
    }
    return cornerPositions[cornerIndex];
}
Vertex interpolateVertex(int edgeIndex, const float scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ, float move,
                         float scale) {
    int edgeToVertex[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    // Define the corner-to-voxel mapping
    int cornerToVoxel[8][3] = {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}};

    int v0Index = edgeToVertex[edgeIndex][0];
    int v1Index = edgeToVertex[edgeIndex][1];

    float value0 = getScalarValueAtCorner(cornerToVoxel[v0Index][0] + offsetX, cornerToVoxel[v0Index][1] + offsetY,
                                           cornerToVoxel[v0Index][2] + offsetZ, scalarGrid);
    float value1 = getScalarValueAtCorner(cornerToVoxel[v1Index][0] + offsetX, cornerToVoxel[v1Index][1] + offsetY,
                                           cornerToVoxel[v1Index][2] + offsetZ, scalarGrid);
    // Check for divide by zero

    float t;
    if (fabs(value1 - value0) < 1e-6) {
        t = 0.5;  // or choose a reasonable default
        RAYX_LOG << "Divide by zero";
    } else {
        t = (0 - value0) / (value1 - value0);
    }

    Vertex v;
    v.pos = glm::vec4((glm::mix(getPositionAtCorner(v0Index), getPositionAtCorner(v1Index), t) + glm::vec3(offsetX, offsetY, offsetZ) -
                       glm::vec3(move, move, move)) *
                          glm::vec3(scale, scale, scale),
                      1.0f);

    return v;
}

float getScalarValueAtCorner(int x, int y, int z, const float scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]) {
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE || z < 0 || z >= GRIDSIZE) {
        throw std::out_of_range("Invalid corner index");
    }
    return scalarGrid[x][y][z];
}