#include "RenderObject.h"

#include "Beamline/OpticalElement.h"
#include "Debug/Debug.h"

std::vector<RenderObject> RenderObject::createRenderObjects(const std::filesystem::path& filename) {
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
    m_translation = glm::vec4(element.m_element.m_outTrans[3][0], element.m_element.m_outTrans[3][1], element.m_element.m_outTrans[3][2], 1.0);
    m_rotation = glm::mat4(element.m_element.m_outTrans);
    this->triangulate();
}

void RenderObject::triangulate() {
    // Create triangle representation
    if (m_Surface.m_type == STYPE_TOROID) {
        RAYX_ERR << "Toroid not implemented yet";
    } else if (m_Surface.m_type == STYPE_PLANE_XY || m_Surface.m_type == STYPE_QUADRIC) {  // TODO: Own case for quadric
        Vertex topLeft, topRight, bottomLeft, bottomRight;
        if (m_Cutout.m_type == 2) {  // trapzoid
            TrapezoidCutout trapez = deserializeTrapezoid(m_Cutout);

            topLeft = Vertex(glm::vec4(-trapez.m_sizeA_x1 / 2.0f, 0, trapez.m_size_x2 / 2.0f, 1.0f), m_darkerBlue);
            topRight = Vertex(glm::vec4(trapez.m_sizeA_x1 / 2.0f, 0, trapez.m_size_x2 / 2.0f, 1.0f), m_blue);
            bottomLeft = Vertex(glm::vec4(-trapez.m_sizeB_x1 / 2.0f, 0, -trapez.m_size_x2 / 2.0f, 1.0f), m_lighterBlue);
            bottomRight = Vertex(glm::vec4(trapez.m_sizeB_x1 / 2.0f, 0, -trapez.m_size_x2 / 2.0f, 1.0f), m_blue);
        } else {  // rectangle, unlimited, elliptical (treat all as rectangles)

            double width, height;
            if (m_Cutout.m_type == CTYPE_UNLIMITED || m_Cutout.m_type == CTYPE_ELLIPTICAL) {
                width = 100.0f;
                height = 100.0f;
            } else {  // rectangle
                RectCutout rect = deserializeRect(m_Cutout);
                width = rect.m_size_x1;
                height = rect.m_size_x2;
            }

            // Calculate two triangle from the position, orientation and size of the render object
            // Assume the rectangle is in the XZ plane, and its center is at position.
            // First, calculate the corners of the rectangle in the model's local space
            topLeft = Vertex(glm::vec4(-width / 2.0f, 0, height / 2.0f, 1.0f), m_darkerBlue);
            topRight = Vertex(glm::vec4(width / 2.0f, 0, height / 2.0f, 1.0f), m_blue);
            bottomLeft = Vertex(glm::vec4(-width / 2.0f, 0, -height / 2.0f, 1.0f), m_lighterBlue);
            bottomRight = Vertex(glm::vec4(width / 2.0f, 0, -height / 2.0f, 1.0f), m_blue);
        }
        // Check if the current object is an image plane.
        if (m_Behaviour.m_type == BTYPE_IMAGE_PLANE || m_Behaviour.m_type == BTYPE_SLIT) {
            // Rotate by 90 degrees around the X-axis.
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            topLeft.pos = glm::vec3(rotationMatrix * glm::vec4(topLeft.pos, 1.0f));
            topRight.pos = glm::vec3(rotationMatrix * glm::vec4(topRight.pos, 1.0f));
            bottomLeft.pos = glm::vec3(rotationMatrix * glm::vec4(bottomLeft.pos, 1.0f));
            bottomRight.pos = glm::vec3(rotationMatrix * glm::vec4(bottomRight.pos, 1.0f));
        }

        m_vertices.push_back(topLeft);
        m_vertices.push_back(topRight);
        m_vertices.push_back(bottomLeft);
        m_vertices.push_back(topRight);
        m_vertices.push_back(bottomRight);
        m_vertices.push_back(bottomLeft);

    } else {
        RAYX_ERR << "Unknown surface type";
    }
}

std::vector<Vertex> RenderObject::getWorldVertices() const {
    std::vector<Vertex> vertices;
    for (auto vertex : m_vertices) {
        vertex.pos = glm::vec3(m_rotation * glm::vec4(vertex.pos, 1.0f) + m_translation);
        vertices.push_back(vertex);
    }
    return vertices;
}

std::vector<Triangle> RenderObject::trianglesFromQuadric() {
    // Define the size and resolution of the grid
    double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE];

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                glm::vec4 pos(x, y, z, 1);
                double value = evaluateQuadricAtPosition(this->m_Surface.m_params, pos);
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

double RenderObject::evaluateQuadricAtPosition(const double sur[16], const glm::vec4& pos) {
    double surface[11];
    surface[0] = -10.0;
    surface[1] = 1.0;
    surface[2] = 1.0;
    surface[3] = 0.0;
    surface[4] = 0.0;
    surface[5] = 0.0;
    surface[6] = 0.0;
    surface[7] = 0.0;
    surface[8] = 0.0;
    surface[9] = 0.0;
    surface[10] = -1.0;
    double result = surface[1] * pos.x * pos.x + surface[2] * pos.y * pos.y + surface[3] * pos.z * pos.z + surface[4] * pos.x * pos.y +
                    surface[5] * pos.x * pos.z + surface[6] * pos.y * pos.z + surface[7] * pos.x + surface[8] * pos.y + surface[9] * pos.z +
                    surface[10];

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
        triangle.vertices[0] = getVertexFromEdge(triTable[caseIndex][i]);
        triangle.vertices[1] = getVertexFromEdge(triTable[caseIndex][i + 1]);
        triangle.vertices[2] = getVertexFromEdge(triTable[caseIndex][i + 2]);

        triangles.push_back(triangle);
    }

    return triangles;
}

Vertex RenderObject::getVertexFromEdge(int edgeIndex) {
    // Define edge-to-vertex mapping. This is typical for Marching Cubes.
    int edgeToVertex[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    // For simplicity, we'll just use the first vertex of the edge.
    int vertexIndex = edgeToVertex[edgeIndex][0];
    int vertexIndex2 = edgeToVertex[edgeIndex][1];

    Vertex v;
    auto x = getPositionAtCorner(vertexIndex);
    auto y = getPositionAtCorner(vertexIndex2);
    v.pos = glm::mix(x, y, 0.5f);

    return v;
}

glm::vec3 RenderObject::getPositionAtCorner(int cornerIndex) {
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
// Vertex RenderObject::interpolateVertex(int edgeIndex) {
//     // Define edge-to-vertex mapping. This is typical for Marching Cubes.
//     int edgeToVertex[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

//     // Get the voxel corner indices for the edge
//     int v0Index = edgeToVertex[edgeIndex][0];
//     int v1Index = edgeToVertex[edgeIndex][1];

//     // Get the scalar values at the voxel corners
//     float value0 = getScalarValueAtCorner(v0Index);
//     float value1 = getScalarValueAtCorner(v1Index);

//     // Interpolate the position. This is a simple linear interpolation.
//     // In practice, you might want to use the actual positions of the voxel corners and interpolate based on the scalar values.
//     float t = (0 - value0) / (value1 - value0);  // Assuming we want the position where the scalar value is 0

//     Vertex v;
//     v.pos = glm::mix(getPositionAtCorner(v0Index), getPositionAtCorner(v1Index), t);

//     return v;
// }
// float Scene::getScalarValueAtCorner(int cornerIndex) {
//     // Check for valid index
//     if (cornerIndex < 0 || cornerIndex >= scalarValues.size()) {
//         throw std::out_of_range("Invalid corner index");
//     }
//     return scalarValues[cornerIndex];
// }