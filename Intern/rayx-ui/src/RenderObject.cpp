#include "RenderObject.h"

#include "Beamline/OpticalElement.h"
#include "Debug/Debug.h"

std::vector<RenderObject> RenderObject::getRenderData(const std::filesystem::path& filename) {
    std::vector<RenderObject> data;
    std::vector<RAYX::OpticalElement> elements;
    std::vector<std::shared_ptr<RAYX::LightSource>> sources;
    {
        auto beamline = RAYX::importBeamline(filename);
        elements = beamline.m_OpticalElements;
        sources = beamline.m_LightSources;
    }
    for (auto element : elements) {
        RenderObject rendObj(element);
        data.push_back(rendObj);
    }
    return data;
}

RenderObject::RenderObject(const RAYX::OpticalElement& element)
    : m_name(element.m_name),
      m_Surface(element.m_element.m_surface),
      m_Cutout(element.m_element.m_cutout),
      m_Behaviour(element.m_element.m_behaviour) {
    m_position = glm::vec4(element.m_element.m_outTrans[3][0], element.m_element.m_outTrans[3][1], element.m_element.m_outTrans[3][2], 1.0);
    m_orientation[0] = glm::vec4(element.m_element.m_outTrans[0][0], element.m_element.m_outTrans[0][1], element.m_element.m_outTrans[0][2], 0.0);
    m_orientation[1] = glm::vec4(element.m_element.m_outTrans[1][0], element.m_element.m_outTrans[1][1], element.m_element.m_outTrans[1][2], 0.0);
    m_orientation[2] = glm::vec4(element.m_element.m_outTrans[2][0], element.m_element.m_outTrans[2][1], element.m_element.m_outTrans[2][2], 0.0);
    this->triangulate();
}

void RenderObject::triangulate() {
    // Create triangle representation
    if (m_Surface.m_type == STYPE_QUADRIC) {
        //
    } else if (m_Surface.m_type == STYPE_TOROID) {
        RAYX_ERR << "Toroid not implemented yet";
    } else if (m_Surface.m_type == STYPE_PLANE_XY) {
        //
    } else {
        RAYX_ERR << "Unknown surface type";
    }
}

std::vector<Triangle> RenderObject::trianglesFromQuadric(const RenderObject& renderObject) {
    // Define the size and resolution of the grid
    double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE];

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                glm::vec4 pos(x, y, z, 1);
                double value = evaluateQuadricAtPosition(pos);
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

double RenderObject::evaluateQuadricAtPosition(const glm::vec4& pos) {
    // Convert the surface array to a glm::mat4.
    glm::mat4 surfaceMatrix(m_Surface.m_params[0], m_Surface.m_params[1], m_Surface.m_params[2], m_Surface.m_params[3], m_Surface.m_params[4],
                            m_Surface.m_params[5], m_Surface.m_params[6], m_Surface.m_params[7], m_Surface.m_params[8], m_Surface.m_params[9],
                            m_Surface.m_params[10], m_Surface.m_params[11], m_Surface.m_params[12], m_Surface.m_params[13], m_Surface.m_params[14],
                            m_Surface.m_params[15]);

    glm::vec4 resultVec = pos * surfaceMatrix;
    double result = glm::dot(resultVec, pos);

    return result;
}

int RenderObject::determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z) {
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

std::vector<Triangle> RenderObject::lookupTrianglesForCase(int caseIndex) {
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

Vertex RenderObject::interpolateVertex() {
    // Here, we'd interpolate between the two voxel corners that the edge connects.
    // This function should return the vertex position on the edge where the surface intersects.
    // The exact interpolation might depend on the scalar values at the voxel corners.
    // This is a placeholder function. The actual function would involve more logic to determine the exact position.
    Vertex v;
    v.pos = glm::vec3(0, 0, 0);  // Placeholder value
    return v;
}