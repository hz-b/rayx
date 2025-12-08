# How to use Doxygen

## Prerequisites

- Doxygen: http://www.doxygen.nl/download.html#srcbin
- (GraphViz: https://graphviz.gitlab.io/download/) \\(\leftarrow\\) at the moment not needed

Note GraphViz installation: On windows you can install graphviz via MSYS2 since you might already have it to build the project. Simply call: pacman -S mingw-w64-x86_64-graphviz in the MSYS2 commandline window. The DOT_PATH muust then be set to {MSYS2}/mingw64/bin

## Configuration

1.  Open the Doxyfile in Intern/rayx-core/doc/ with Doxywizard
2.  Under Wizard->Diagrams check "Use dot tool from the GraphViz package"
3.  Change the DOT_PATH in "Expert \\(\rightarrow\\) Dot" to the bin-directory in your GraphViz-Folder (where you installed it). This is usually the doxygen/bin folder.
4.  Click on the "Run"-tab and click "Run doxygen" (this will take a while, depending on your cpu performance)
5.  After completion click "Show HTML output" in the Doxywizard or open the Index.html in Intern/rayx-core/doc/html/

## Class Diagram

A generated Class Diagram can be found under "Classes->Class Hierarchy"

## Generating documentation with GPT-4 (3.5 not recommended)

Use this prompt and replace the header and cpp code in the lower part with your code.
Note: The first code is example code for ChatGPT so it knows the style. Do not replace it. Just replace the lower code after "Create a documentation for this header file:" and "Create a documentation for this cpp file:"

### Important:

Read and check all generated content. ChatGPT can easily make errors and assumptions. It is only a tool to save time writing text. The information needs to be approved by the developer.

Prompt:

```
I need to create a doxygen documentation for some code I wrote. You will help me with that. The comments in the header file should include the information what the function does/is for. In the cpp file, it's how the function is implemented. This is a good example how it should be:

.h
/**
 * @brief Triangulates optical elements for rendering.
 * @param elements A vector of optical elements to be triangulated.
 * @param useMarchinCubes Flag to determine if Marching Cubes triangulation should be used.
 * @return A vector of RenderObject, which are the triangulated version of the input elements.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, bool useMarchinCubes = false);

/**
 * @brief Generates visual representations of rays based on bundle history and optical elements.
 * @param bundleHist RAYX-Core type, providing details of ray interactions in the beamline.
 * @param elements A vector of optical elements used for coordinate conversions.
 * @return A vector of lines, which visually represents the paths of rays in the beamline.
 */
std::vector<Line> getRays(const RAYX::BundleHistory& bundleHist, const std::vector<RAYX::OpticalElement>& elements);

.cpp
/**
 * This function processes the BundleHistory and determines the ray's path in the beamline.
 * Depending on the event type associated with the ray, the function produces visual lines that represent
 * ray segments, colored based on the event type.
 */
std::vector<Line> getRays(const RAYX::BundleHistory& bundleHist, const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<Line> rays;

    for (const auto& rayHist : bundleHist) {
        glm::vec3 rayLastPos = {0.0f, 0.0f, 0.0f};
        for (const auto& event : rayHist) {
            if (event.m_eventType == ETYPE_JUST_HIT_ELEM || event.m_eventType == ETYPE_ABSORBED) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, YELLOW};
                Vertex point = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? Vertex(worldPos, ORANGE) : Vertex(worldPos, RED);

                rays.push_back(Line(origin, point));
                rayLastPos = point.pos;
            } else if (event.m_eventType == ETYPE_FLY_OFF) {
                // Fly off events are in world coordinates
                // The origin here is the position of the event
                // The point is defined by the direction of the ray (default length)

                glm::vec4 eventPos = glm::vec4(event.m_position, 1.0f);
                glm::vec4 eventDir = glm::vec4(event.m_direction, 0.0f);
                glm::vec4 pointPos = eventPos + eventDir * 1000.0f;

                Vertex origin = {eventPos, GREY};
                Vertex point = {pointPos, GREY};

                rays.push_back(Line(origin, point));
            }
        }
    }

    return rays;
}

Create a documentation for this header file:

// Marching Cubes
std::vector<RenderObject> marchingCubeTriangulation(const std::vector<RAYX::OpticalElement>& elements);
std::vector<Triangle> trianglesFromQuadric(const double* quadric, Cutout cutout);  // TODO: make nicer

glm::vec3 getPositionAtCorner(int cornerIndex);
Vertex interpolateVertex(int edgeIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ,
                         glm::vec3 scale);
double evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos);
int determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z);
std::vector<Triangle> lookupTrianglesForCase(int caseIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY,
                                             int offsetZ, glm::vec3 scale);
Vertex getVertexFromEdge(int edgeIndex);
double getScalarValueAtCorner(int x, int y, int z, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]);

// Marching Cubes
const int edgeTable[256] = {...}
const int triTable[256][16] = {...}

Create a documentation for this cpp file:
#include "MarchingCubes.h"

#include "Colors.h"
#include "Debug/Debug.h"

namespace RAYX {
namespace CPU_TRACER {
bool RAYX_API inCutout(Cutout cutout, double x1, double x2);
}  // namespace CPU_TRACER
}  // namespace RAYX

std::vector<RenderObject> marchingCubeTriangulation(const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<RenderObject> objects;

    for (RAYX::OpticalElement element : elements) {
        auto quadric = element.m_element.m_surface.m_params;
        std::vector<Triangle> triangles = trianglesFromQuadric(quadric, element.m_element.m_cutout);
        RenderObject object(glm::mat4(element.m_element.m_outTrans));
        for (Triangle triangle : triangles) {
            object.addTriangle(triangle);
        }
        objects.push_back(object);
    }

    return objects;
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
```
