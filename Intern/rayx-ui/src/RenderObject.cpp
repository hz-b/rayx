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
    } else if (m_Surface.m_type == STYPE_PLANE_XY) {  // TODO: Own case for quadric
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

    } else if (m_Surface.m_type == STYPE_QUADRIC) {
        auto tris = trianglesFromQuadric();
        for (auto tri : tris) {
            tri.vertices[0].color = m_darkerBlue;
            tri.vertices[1].color = m_blue;
            tri.vertices[2].color = m_lighterBlue;
            m_vertices.push_back(tri.vertices[0]);
            m_vertices.push_back(tri.vertices[1]);
            m_vertices.push_back(tri.vertices[2]);
        }
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

    const double SCALE = 5.0 * 10.0 / GRIDSIZE;  // Define your desired scaling factor here
    double move = (GRIDSIZE / 2.0);

    // 1. Sample the 3D space
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            for (int z = 0; z < GRIDSIZE; z++) {
                // Convert grid coordinate to centered & scaled space coordinate

                double realX = (x - (GRIDSIZE / 2.0)) * SCALE;
                double realY = (y - (GRIDSIZE / 2.0)) * SCALE;
                double realZ = (z - (GRIDSIZE / 2.0)) * SCALE;

                glm::vec4 pos(realX, realY, realZ, 1);
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
                std::vector<Triangle> voxelTriangles = lookupTrianglesForCase(caseIndex, scalarGrid, x, y, z, move, SCALE);
                triangles.insert(triangles.end(), voxelTriangles.begin(), voxelTriangles.end());
            }
        }
    }

    return triangles;
}

double RenderObject::evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos) {
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

int RenderObject::determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z) {
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

std::vector<Triangle> RenderObject::lookupTrianglesForCase(int caseIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX,
                                                           int offsetY, int offsetZ, double move, double scale) {
    // Using the triTable to generate the triangles for the voxel.

    std::vector<Triangle> triangles;

    // triTable[caseIndex] provides the edges to be connected for the triangles.
    // Every 3 indices in the table make up a triangle.
    for (int i = 0; triTable[caseIndex][i] != -1; i += 3) {
        Triangle triangle;

        // Convert edge indices to vertices
        triangle.vertices[0] = interpolateVertex(triTable[caseIndex][i], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.vertices[1] = interpolateVertex(triTable[caseIndex][i + 1], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.vertices[2] = interpolateVertex(triTable[caseIndex][i + 2], scalarGrid, offsetX, offsetY, offsetZ, move, scale);
        triangle.vertices[0].color = m_darkerBlue;
        triangle.vertices[1].color = m_blue;
        triangle.vertices[2].color = m_lighterBlue;

        triangles.push_back(triangle);
    }

    return triangles;
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
Vertex RenderObject::interpolateVertex(int edgeIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ,
                                       double move, double scale) {
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

    Vertex v;
    v.pos = (glm::mix(getPositionAtCorner(v0Index), getPositionAtCorner(v1Index), t) + glm::vec3(offsetX, offsetY, offsetZ) -
             glm::vec3(move, move, move)) *
            glm::vec3(scale, scale, scale);

    return v;
}

double RenderObject::getScalarValueAtCorner(int x, int y, int z, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]) {
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE || z < 0 || z >= GRIDSIZE) {
        throw std::out_of_range("Invalid corner index");
    }
    return scalarGrid[x][y][z];
}
