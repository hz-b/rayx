#include "Triangulate.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Colors.h"
#include "Debug/Debug.h"
#include "Triangulation/MarchingCubes.h"
#include "Triangulation/TraceTriangulation.h"

// ------ Helper functions ------

void calculateVerticesForType(const Element& elem, glm::vec4& topLeft, glm::vec4& topRight, glm::vec4& bottomLeft, glm::vec4& bottomRight) {
    const double defWidthHeight = 100.0f;
    switch (static_cast<int>(elem.m_cutout.m_type)) {
        case CTYPE_TRAPEZOID: {
            TrapezoidCutout trapezoid = deserializeTrapezoid(elem.m_cutout);
            topLeft = {-trapezoid.m_sizeA_x1 / 2.0f, 0, trapezoid.m_size_x2 / 2.0f, 1.0f};
            topRight = {trapezoid.m_sizeA_x1 / 2.0f, 0, trapezoid.m_size_x2 / 2.0f, 1.0f};
            bottomLeft = {-trapezoid.m_sizeB_x1 / 2.0f, 0, -trapezoid.m_size_x2 / 2.0f, 1.0f};
            bottomRight = {trapezoid.m_sizeB_x1 / 2.0f, 0, -trapezoid.m_size_x2 / 2.0f, 1.0f};
            break;
        }
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(elem.m_cutout);
            topLeft = {-rect.m_size_x1 / 2.0f, 0, rect.m_size_x2 / 2.0f, 1.0f};
            topRight = {rect.m_size_x1 / 2.0f, 0, rect.m_size_x2 / 2.0f, 1.0f};
            bottomLeft = {-rect.m_size_x1 / 2.0f, 0, -rect.m_size_x2 / 2.0f, 1.0f};
            bottomRight = {rect.m_size_x1 / 2.0f, 0, -rect.m_size_x2 / 2.0f, 1.0f};
            break;
        }
        case CTYPE_ELLIPTICAL:
        case CTYPE_UNLIMITED:
        default: {
            topLeft = {-defWidthHeight / 2.0f, 0, defWidthHeight / 2.0f, 1.0f};
            topRight = {defWidthHeight / 2.0f, 0, defWidthHeight / 2.0f, 1.0f};
            bottomLeft = {-defWidthHeight / 2.0f, 0, -defWidthHeight / 2.0f, 1.0f};
            bottomRight = {defWidthHeight / 2.0f, 0, -defWidthHeight / 2.0f, 1.0f};
            break;
        }
    }
}

void flipToXYPlane(glm::vec4& topLeft, glm::vec4& topRight, glm::vec4& bottomLeft, glm::vec4& bottomRight) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    topLeft = rotationMatrix * topLeft;
    topRight = rotationMatrix * topRight;
    bottomLeft = rotationMatrix * bottomLeft;
    bottomRight = rotationMatrix * bottomRight;
}

std::vector<RenderObject> planarTriangulation(const std::vector<RAYX::OpticalElement>& elements) {
    std::vector<RenderObject> rObjects;
    static glm::vec4 blue = {0.0f, 0.0f, 1.0f, 1.0f};
    static glm::vec4 darkBlue = {0.0f, 0.0f, 0.4f, 1.0f};
    static glm::vec4 lightBlue = {0.7f, 0.7f, 1.0f, 1.0f};

    for (const auto& element : elements) {
        RenderObject rObject(element.m_element.m_outTrans);

        glm::vec4 topLeft, topRight, bottomLeft, bottomRight;
        calculateVerticesForType(element.m_element, topLeft, topRight, bottomLeft, bottomRight);

        // Some objects are in the XY not the XZ plane
        if (element.m_element.m_behaviour.m_type == BTYPE_SLIT || element.m_element.m_behaviour.m_type == BTYPE_IMAGE_PLANE) {
            flipToXYPlane(topLeft, topRight, bottomLeft, bottomRight);
        }

        Vertex v1 = {topLeft, lightBlue};
        Vertex v2 = {topRight, blue};
        Vertex v3 = {bottomLeft, blue};
        Vertex v4 = {bottomRight, darkBlue};
        rObject.addTriangle({v1, v2, v3});
        rObject.addTriangle({v2, v3, v4});

        rObjects.push_back(rObject);
    }

    return rObjects;
}

// ------ Interface functions ------

/**
 * This function takes optical elements and categorizes them for efficient triangulation.
 * Depending on the type of the surface of the element and the option to use Marching Cubes,
 * different triangulation methods are applied.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, bool useMarchinCubes) {
    std::vector<RAYX::OpticalElement> planarElements;
    std::vector<RAYX::OpticalElement> curvedElements;
    std::vector<RAYX::OpticalElement> quadricElements;

    for (auto element : elements) {
        switch ((int)element.m_element.m_surface.m_type) {
            case STYPE_PLANE_XY:
                planarElements.push_back(element);
                break;
            case STYPE_QUADRIC: {
                QuadricSurface q = deserializeQuadric(element.m_element.m_surface);
                if ((q.m_a11 == 0 && q.m_a22 == 0 && q.m_a33 == 0) && (q.m_a14 != 0 || q.m_a24 != 0 || q.m_a34 != 0)) {  // Plane
                    planarElements.push_back(element);
                } else {
                    if (useMarchinCubes) {
                        quadricElements.push_back(element);
                    } else {
                        curvedElements.push_back(element);
                    }
                }
                break;
            }
            case STYPE_TOROID:
                curvedElements.push_back(element);
                break;
            default:
                // Error
                RAYX_ERR << "Unknown element type: " << element.m_element.m_surface.m_type;
                break;
        }
    }

    std::cout << "Triangulating " << planarElements.size() << " planar elements, " << curvedElements.size() << " curved elements and "
              << quadricElements.size() << " quadric elements" << std::endl;
    std::vector<RenderObject> rObjects = planarTriangulation(planarElements);
    std::vector<RenderObject> traceObjects = traceTriangulation(curvedElements);
    rObjects.insert(rObjects.end(), traceObjects.begin(), traceObjects.end());
    if (useMarchinCubes) {
        std::vector<RenderObject> marchinCubeObjects = marchingCubeTriangulation(quadricElements);
        rObjects.insert(rObjects.end(), marchinCubeObjects.begin(), marchinCubeObjects.end());
    }
    return rObjects;
}

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
                Vertex vertexorange = {worldPos, ORANGE};
                Vertex vertrexred = {worldPos, RED};
                Vertex point = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? vertexorange : vertrexred;

                Line myline = {origin, point};
                rays.push_back(myline);
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

                Line myline = {origin, point};
                rays.push_back(myline);
            }
        }
    }

    return rays;
}