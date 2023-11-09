#include "Triangulate.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Colors.h"
#include "Debug/Debug.h"
#include "Shader/Constants.h"
#include "Triangulation/TraceTriangulation.h"

// ------ Helper functions ------

struct Polygon {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB) {
        vertices = {
            Vertex({-widthA / 2.0f, 0, -lengthA / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),  // Bottom-left
            Vertex({-widthB / 2.0f, 0, lengthB / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),   // Top-left
            Vertex({widthB / 2.0f, 0, lengthB / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),    // Top-right
            Vertex({widthA / 2.0f, 0, -lengthA / 2.0f, 1.0f}, OPT_ELEMENT_COLOR)    // Bottom-right
        };
        indices = {0, 1, 2, 2, 3, 0};
    }

    void calculateForElliptical(double diameterA, double diameterB) {
        constexpr uint32_t numVertices = 20;
        vertices.reserve(numVertices);
        indices.reserve(numVertices * 3);

        // Calculate vertices
        for (uint32_t i = 0; i < numVertices; i++) {
            double angle = 2.0f * PI * i / numVertices;
            glm::vec4 pos = {diameterA * cos(angle) / 2.0f, 0, diameterB * sin(angle) / 2.0f, 1.0f};
            glm::vec4 col = glm::mix(OPT_ELEMENT_COLOR, RED, i / (float)numVertices);
            vertices.emplace_back(pos, col);
        }

        // Calculate indices
        for (uint32_t i = 0; i < numVertices - 2; i++) {
            indices.push_back(0);
            indices.push_back(i + 1);
            indices.push_back(i + 2);
        }
        indices.push_back(0);
        indices.push_back(numVertices - 1);
        indices.push_back(1);
    }
};

void calculateSolidMeshOfType(const Cutout& cutout, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    const double defWidthHeight = 50.0f;
    Polygon poly;

    switch (static_cast<int>(cutout.m_type)) {
        case CTYPE_TRAPEZOID: {
            TrapezoidCutout trapezoid = deserializeTrapezoid(cutout);
            poly.calculateForQuadrilateral(trapezoid.m_widthA, trapezoid.m_widthB, trapezoid.m_length, trapezoid.m_length);
            break;
        }
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(cutout);
            poly.calculateForQuadrilateral(rect.m_width, rect.m_width, rect.m_length, rect.m_length);
            break;
        }
        case CTYPE_ELLIPTICAL: {
            EllipticalCutout ellipse = deserializeElliptical(cutout);
            poly.calculateForElliptical(ellipse.m_diameter_x, ellipse.m_diameter_z);
            break;
        }
        case CTYPE_UNLIMITED:
        default: {
            poly.calculateForQuadrilateral(defWidthHeight, defWidthHeight, defWidthHeight, defWidthHeight);
            break;
        }
    }

    // Add vertices and indices to the output vectors. No reserve for now because this might be called in loops
    uint32_t offset = (uint32_t)vertices.size();
    for (uint32_t index : poly.indices) {
        indices.push_back(offset + index);
    }
    vertices.insert(vertices.end(), poly.vertices.begin(), poly.vertices.end());
}

/**
 * This function takes a polygon and interpolates it to have the specified number of vertices.
 * The polygon is assumed to be convex.
 */
void interpolateConvexPolygon(std::vector<Vertex>& polyVertices, uint32_t targetNumber) {
    if (polyVertices.size() == targetNumber || polyVertices.empty()) {
        return;  // No interpolation needed if counts are the same or polygon is empty
    }

    std::vector<Vertex> interpolatedVertices;
    interpolatedVertices.reserve(targetNumber);

    size_t originalCount = polyVertices.size();
    double step = static_cast<double>(originalCount) / targetNumber;

    for (uint32_t i = 0; i < targetNumber; ++i) {
        double exactIndex = i * step;
        size_t lowerIndex = static_cast<size_t>(exactIndex);
        size_t upperIndex = (lowerIndex + 1) % originalCount;  // Wrap around for closed loop

        double fraction = exactIndex - lowerIndex;

        glm::vec4 interpolatedPosition = glm::mix(polyVertices[lowerIndex].pos, polyVertices[upperIndex].pos, fraction);
        glm::vec4 green = {0.0f, 1.0f, 0.0f, 1.0f};
        glm::vec4 interpolatedColor = glm::mix(polyVertices[lowerIndex].color, green, i / (float)targetNumber);

        interpolatedVertices.push_back({interpolatedPosition, interpolatedColor});
    }

    // Replace the original vertices with the interpolated ones
    polyVertices = std::move(interpolatedVertices);
}

void calculateMeshForSlit(const Element& element, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    // Deserialize to get SlitBehaviour
    SlitBehaviour slit = deserializeSlit(element.m_behaviour);

    // Calculate vertices for the beamstop
    calculateSolidMeshOfType(slit.m_beamstopCutout, vertices, indices);

    // Calculate vertices for the outer slit
    std::vector<Vertex> outerSlitVertices;
    std::vector<uint32_t> outerSlitIndices;
    calculateSolidMeshOfType(element.m_cutout, outerSlitVertices, outerSlitIndices);  // TODO: This is a bit wasteful, since we don't need the indices

    // Calculate vertices for the opening
    std::vector<Vertex> openingVertices;
    std::vector<uint32_t> openingIndices;
    calculateSolidMeshOfType(slit.m_openingCutout, openingVertices, openingIndices);  // TODO: This is a bit wasteful, since we don't need the indices

    // Connecting the outer slit to the opening
    uint32_t numOuterSlitVertices = (uint32_t)outerSlitVertices.size();
    uint32_t numOpeningVertices = (uint32_t)openingVertices.size();

    if (numOpeningVertices < numOuterSlitVertices) {
        interpolateConvexPolygon(openingVertices, numOuterSlitVertices);
        numOpeningVertices = numOuterSlitVertices;
    } else if (numOpeningVertices > numOuterSlitVertices) {
        interpolateConvexPolygon(outerSlitVertices, numOpeningVertices);
        numOuterSlitVertices = numOpeningVertices;
    } else {
        // Do nothing; the number of vertices is the same
    }

    // Create the Distance Matrix
    std::vector<std::vector<double>> distanceMatrix(numOuterSlitVertices, std::vector<double>(numOpeningVertices, 0.0));
    for (uint32_t i = 0; i < numOuterSlitVertices; ++i) {
        for (uint32_t j = 0; j < numOpeningVertices; ++j) {
            distanceMatrix[i][j] = glm::distance(outerSlitVertices[i].pos, openingVertices[j].pos);
        }
    }

    // Pair vertices based on distance, considering order to minimize crossings
    std::vector<uint32_t> pairings(numOuterSlitVertices);
    std::vector<bool> used(numOpeningVertices, false);
    for (uint32_t i = 0; i < numOuterSlitVertices; ++i) {
        double minDistance = std::numeric_limits<double>::max();
        uint32_t pairedIndex = 0;
        for (uint32_t j = 0; j < numOpeningVertices; ++j) {
            if (!used[j] && distanceMatrix[i][j] < minDistance) {
                minDistance = distanceMatrix[i][j];
                pairedIndex = j;
            }
        }
        pairings[i] = pairedIndex;
        used[pairedIndex] = true;  // Mark this opening vertex as used
    }

    // Add the vertices of the outer slit and the opening to the main vertex list
    uint32_t outerSlitOffset = static_cast<uint32_t>(vertices.size());
    vertices.insert(vertices.end(), outerSlitVertices.begin(), outerSlitVertices.end());

    uint32_t openingOffset = static_cast<uint32_t>(vertices.size());
    vertices.insert(vertices.end(), openingVertices.begin(), openingVertices.end());

    // Triangulate to form a continuous strip
    for (uint32_t i = 0; i < numOuterSlitVertices; ++i) {
        uint32_t outerIndex = i;
        uint32_t outerNextIndex = (i + 1) % numOuterSlitVertices;
        uint32_t openingIndex = pairings[i];
        uint32_t openingNextIndex = pairings[outerNextIndex];

        // Triangulate using the paired vertices
        indices.push_back(outerSlitOffset + outerIndex);
        indices.push_back(outerSlitOffset + outerNextIndex);
        indices.push_back(openingOffset + openingIndex);

        indices.push_back(outerSlitOffset + outerNextIndex);
        indices.push_back(openingOffset + openingIndex);
        indices.push_back(openingOffset + openingNextIndex);
    }
}

RenderObject planarTriangulation(const RAYX::OpticalElement& element, Device& device) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    // The slit behaviour needs special attention, since it is basically three cutouts (the slit, the beamstop and the opening)
    if (element.m_element.m_behaviour.m_type == BTYPE_SLIT) {
        calculateMeshForSlit(element.m_element, vertices, indices);
    } else {
        calculateSolidMeshOfType(element.m_element.m_cutout, vertices, indices);
    }

    RenderObject renderObj(element.m_name, device, element.m_element.m_outTrans, vertices, indices);
    return renderObj;
}

bool isPlanar(const QuadricSurface& q) { return (q.m_a11 == 0 && q.m_a22 == 0 && q.m_a33 == 0) && (q.m_a14 != 0 || q.m_a24 != 0 || q.m_a34 != 0); }

// ------ Interface functions ------

/**
 * This function takes optical elements and categorizes them for efficient triangulation.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, Device& device) {
    std::vector<RenderObject> rObjects;

    for (const RAYX::OpticalElement& element : elements) {
        switch (static_cast<int>(element.m_element.m_surface.m_type)) {
            case STYPE_PLANE_XZ: {
                RenderObject ro = planarTriangulation(element, device);  // Assume this returns a RenderObject
                rObjects.emplace_back(std::move(ro));
                break;
            }
            case STYPE_QUADRIC: {
                QuadricSurface q = deserializeQuadric(element.m_element.m_surface);
                if (isPlanar(q)) {  // Replace with your condition for planarity
                    RenderObject ro = planarTriangulation(element, device);
                    rObjects.emplace_back(std::move(ro));
                } else {
                    RenderObject ro = traceTriangulation(element, device);  // Assume this returns a RenderObject
                    rObjects.emplace_back(std::move(ro));
                }
                break;
            }
            case STYPE_TOROID: {
                RenderObject ro = traceTriangulation(element, device);  // Assume this returns a RenderObject
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

    for (const RAYX::RayHistory& rayHist : bundleHist) {
        glm::vec3 rayLastPos = {0.0f, 0.0f, 0.0f};
        for (const RAYX::Event& event : rayHist) {
            if (event.m_eventType == ETYPE_JUST_HIT_ELEM || event.m_eventType == ETYPE_ABSORBED) {
                // Events where rays hit objects are in element coordinates
                // We need to convert them to world coordinates
                glm::vec4 worldPos = elements[(size_t)event.m_lastElement].m_element.m_outTrans * glm::vec4(event.m_position, 1.0f);

                Vertex origin = {{rayLastPos.x, rayLastPos.y, rayLastPos.z, 1.0f}, YELLOW};
                Vertex point = (event.m_eventType == ETYPE_JUST_HIT_ELEM) ? Vertex(worldPos, ORANGE) : Vertex(worldPos, RED);

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
