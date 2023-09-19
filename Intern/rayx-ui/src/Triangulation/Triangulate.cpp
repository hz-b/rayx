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
            topLeft = {-trapezoid.m_widthA / 2.0f, 0, trapezoid.m_length / 2.0f, 1.0f};
            topRight = {trapezoid.m_widthA / 2.0f, 0, trapezoid.m_length / 2.0f, 1.0f};
            bottomLeft = {-trapezoid.m_widthB / 2.0f, 0, -trapezoid.m_length / 2.0f, 1.0f};
            bottomRight = {trapezoid.m_widthB / 2.0f, 0, -trapezoid.m_length / 2.0f, 1.0f};
            break;
        }
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(elem.m_cutout);
            topLeft = {-rect.m_width / 2.0f, 0, rect.m_length / 2.0f, 1.0f};
            topRight = {rect.m_width / 2.0f, 0, rect.m_length / 2.0f, 1.0f};
            bottomLeft = {-rect.m_width / 2.0f, 0, -rect.m_length / 2.0f, 1.0f};
            bottomRight = {rect.m_width / 2.0f, 0, -rect.m_length / 2.0f, 1.0f};
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

RenderObject planarTriangulation(const RAYX::OpticalElement& element, Device& device) {
    static glm::vec4 blue = {0.0f, 0.0f, 1.0f, 1.0f};
    static glm::vec4 darkBlue = {0.0f, 0.0f, 0.4f, 1.0f};
    static glm::vec4 lightBlue = {0.7f, 0.7f, 1.0f, 1.0f};

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
    std::vector<Vertex> vertices = {v1, v2, v3, v4};
    std::vector<uint32_t> indices = {0, 1, 2, 2, 1, 3};

    RenderObject renderObj(device, element.m_element.m_outTrans, vertices, indices);
    return renderObj;
}

bool isPlanar(const QuadricSurface& q) { return (q.m_a11 == 0 && q.m_a22 == 0 && q.m_a33 == 0) && (q.m_a14 != 0 || q.m_a24 != 0 || q.m_a34 != 0); }

// ------ Interface functions ------

/**
 * This function takes optical elements and categorizes them for efficient triangulation.
 * Depending on the type of the surface of the element and the option to use Marching Cubes,
 * different triangulation methods are applied.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, Device& device, bool useMarchingCubes) {
    std::vector<RenderObject> rObjects;

    for (const auto& element : elements) {
        switch (static_cast<int>(element.m_element.m_surface.m_type)) {
            case STYPE_PLANE_XZ: {
                auto ro = planarTriangulation(element, device);  // Assume this returns a RenderObject
                rObjects.emplace_back(std::move(ro));
                break;
            }
            case STYPE_QUADRIC: {
                QuadricSurface q = deserializeQuadric(element.m_element.m_surface);
                if (isPlanar(q)) {  // Replace with your condition for planarity
                    auto ro = planarTriangulation(element, device);
                    rObjects.emplace_back(std::move(ro));
                } else {
                    if (useMarchingCubes) {
                        auto ro = marchingCubeTriangulation(element, device);  // Assume this returns a RenderObject
                        rObjects.emplace_back(std::move(ro));
                    } else {
                        auto ro = traceTriangulation(element, device);  // Assume this returns a RenderObject
                        rObjects.emplace_back(std::move(ro));
                    }
                }
                break;
            }
            case STYPE_TOROID: {
                auto ro = traceTriangulation(element, device);  // Assume this returns a RenderObject
                rObjects.emplace_back(std::move(ro));
                break;
            }
            default:
                RAYX_ERR << "Unknown element type: " << element.m_element.m_surface.m_type;
                break;
        }
    }

    std::cout << "Triangulation complete" << std::endl;
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

                rays.push_back(Line(origin, point));
            } else if (event.m_eventType == ETYPE_NOT_ENOUGH_BOUNCES) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                const glm::vec4 white = {1.0f, 1.0f, 1.0f, 0.7f};
                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, white};
                Vertex point = Vertex(worldPos, white);

                rays.push_back(Line(origin, point));
                rayLastPos = point.pos;
            }
        }
    }

    return rays;
}
