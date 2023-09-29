#include "MarchingCubes.h"

#include "Colors.h"
#include "Debug/Debug.h"
#include "MarchingCubeConstans.h"

namespace RAYX {
namespace CPU_TRACER {
bool RAYX_API inCutout(Cutout cutout, double x1, double x2);
}  // namespace CPU_TRACER
}  // namespace RAYX

RenderObject marchingCubeTriangulation(const RAYX::OpticalElement& element, Device& device) {
    auto quadric = element.m_element.m_surface.m_params;
    std::vector<Triangle> triangles = trianglesFromQuadric(quadric, element.m_element.m_cutout);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < triangles.size(); i++) {
        vertices.push_back(triangles[i].v1);
        vertices.push_back(triangles[i].v2);
        vertices.push_back(triangles[i].v3);
        indices.push_back(i * 3);
        indices.push_back(i * 3 + 1);
        indices.push_back(i * 3 + 2);
    }

    return RenderObject(device, element.m_element.m_outTrans, vertices, indices);
}

std::vector<Triangle> trianglesFromQuadric(const double* quadric, Cutout cutout) {
    // Define the size and resolution of the grid
    double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE];
    if (cutout.m_type == CTYPE_UNLIMITED) {
        RAYX_ERR << "Unlimited cutout not supported by marching cubes";
    }
    RAYX_LOG << "cutout: " << cutout.m_params[0] << ", " << cutout.m_params[1];

    const double SCALE = 1;  // Define your desired scaling factor here

    const glm::vec3 bounding_box = glm::vec3(cutout.m_params[0], cutout.m_params[1], 1.0f);
    const glm::vec3 scale = glm::vec3(cutout.m_params[0] * SCALE / GRIDSIZE, SCALE * 1.0f, cutout.m_params[1] * SCALE / GRIDSIZE);

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                // Convert grid coordinate to centered & scaled space coordinate

                double realX = ((double(x) / GRIDSIZE) - 0.5) * bounding_box.x;
                double realY = ((double(y) / GRIDSIZE) - 0.5) * bounding_box.y;
                double realZ = ((double(z) / GRIDSIZE) - 0.5) * bounding_box.z;

                glm::vec4 pos(realX, realY, realZ, 1);
                double value = evaluateQuadricAtPosition(quadric, pos);
                scalarGrid[x][y][z] = value;
            }
        }
    }

    // 2. March through each voxel
    std::vector<Triangle> triangles;
    for (int x = 0; x < GRIDSIZE - 1; x++) {
        for (int y = 0; y < GRIDSIZE - 1; y++) {
            for (int z = 0; z < GRIDSIZE - 1; z++) {
                double realX = ((double(x) / GRIDSIZE) - 0.5) * bounding_box.x;
                double realZ = ((double(z) / GRIDSIZE) - 0.5) * bounding_box.y;
                if (RAYX::CPU_TRACER::inCutout(cutout, realX, realZ)) {
                    int caseIndex = determineMarchingCubesCase(scalarGrid, x, y, z);
                    std::vector<Triangle> voxelTriangles = lookupTrianglesForCase(caseIndex, scalarGrid, x, y, z, scale);
                    triangles.insert(triangles.end(), voxelTriangles.begin(), voxelTriangles.end());
                }
            }
        }
    }

    return triangles;
}

double evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos) {
    double icurv = surface[0];
    double a11 = surface[1];
    double a12 = surface[2];
    double a13 = surface[3];
    double a14 = surface[4];
    double a22 = surface[5];
    double a23 = surface[6];
    double a24 = surface[7];
    double a33 = surface[8];
    double a34 = surface[9];
    double a44 = surface[10];
    double result = a11 * pos.x * pos.x + a22 * pos.y * pos.y + a33 * pos.z * pos.z + a12 * pos.x * pos.y + a13 * pos.x * pos.z +
                    a23 * pos.y * pos.z + a14 * pos.x + a24 * pos.y + a34 * pos.z + a44;

    return result;
}

int determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z) {
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

std::vector<Triangle> lookupTrianglesForCase(int caseIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY,
                                             int offsetZ, glm::vec3 scale) {
    // Using the triTable to generate the triangles for the voxel.

    std::vector<Triangle> triangles;

    // triTable[caseIndex] provides the edges to be connected for the triangles.
    // Every 3 indices in the table make up a triangle.
    for (int i = 0; triTable[caseIndex][i] != -1; i += 3) {
        Triangle triangle;

        // Convert edge indices to vertices
        triangle.v1 = interpolateVertex(triTable[caseIndex][i], scalarGrid, offsetX, offsetY, offsetZ, scale);
        triangle.v2 = interpolateVertex(triTable[caseIndex][i + 1], scalarGrid, offsetX, offsetY, offsetZ, scale);
        triangle.v3 = interpolateVertex(triTable[caseIndex][i + 2], scalarGrid, offsetX, offsetY, offsetZ, scale);
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
Vertex interpolateVertex(int edgeIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ,
                         glm::vec3 scale) {
    int edgeToVertex[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    // Define the corner-to-voxel mapping
    int cornerToVoxel[8][3] = {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}};

    int v0Index = edgeToVertex[edgeIndex][0];
    int v1Index = edgeToVertex[edgeIndex][1];

    double value0 = getScalarValueAtCorner(cornerToVoxel[v0Index][0] + offsetX, cornerToVoxel[v0Index][1] + offsetY,
                                           cornerToVoxel[v0Index][2] + offsetZ, scalarGrid);
    double value1 = getScalarValueAtCorner(cornerToVoxel[v1Index][0] + offsetX, cornerToVoxel[v1Index][1] + offsetY,
                                           cornerToVoxel[v1Index][2] + offsetZ, scalarGrid);
    // Check for divide by zero

    double t;
    if (fabs(value1 - value0) < 1e-6) {
        t = 0.5;  // or choose a reasonable default
        RAYX_LOG << "Divide by zero";
    } else {
        t = (0 - value0) / (value1 - value0);
    }
    double move = (GRIDSIZE / 2.0);
    Vertex v;
    v.pos = glm::vec4((glm::mix(getPositionAtCorner(v0Index), getPositionAtCorner(v1Index), t) + glm::vec3(offsetX, offsetY, offsetZ) -
                       glm::vec3(move, move, move)) *
                          scale,
                      1.0f);

    return v;
}

double getScalarValueAtCorner(int x, int y, int z, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]) {
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE || z < 0 || z >= GRIDSIZE) {
        throw std::out_of_range("Invalid corner index");
    }
    return scalarGrid[x][y][z];
}