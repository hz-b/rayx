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

            topLeft = Vertex(glm::vec4(-trapez.m_sizeA_x1 / 2.0f, 0, trapez.m_size_x2 / 2.0f, 1.0f), m_darkerGreen);
            topRight = Vertex(glm::vec4(trapez.m_sizeA_x1 / 2.0f, 0, trapez.m_size_x2 / 2.0f, 1.0f), m_greenBase);
            bottomLeft = Vertex(glm::vec4(-trapez.m_sizeB_x1 / 2.0f, 0, -trapez.m_size_x2 / 2.0f, 1.0f), m_lighterGreen);
            bottomRight = Vertex(glm::vec4(trapez.m_sizeB_x1 / 2.0f, 0, -trapez.m_size_x2 / 2.0f, 1.0f), m_greenBase);
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
            topLeft = Vertex(glm::vec4(-width / 2.0f, 0, height / 2.0f, 1.0f), m_darkerGreen);
            topRight = Vertex(glm::vec4(width / 2.0f, 0, height / 2.0f, 1.0f), m_greenBase);
            bottomLeft = Vertex(glm::vec4(-width / 2.0f, 0, -height / 2.0f, 1.0f), m_lighterGreen);
            bottomRight = Vertex(glm::vec4(width / 2.0f, 0, -height / 2.0f, 1.0f), m_greenBase);
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