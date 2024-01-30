#include "Triangulate.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "Colors.h"
#include "Debug/Debug.h"
#include "GeometryUtils.h"
#include "Shader/Constants.h"
#include "Triangulation/TraceTriangulation.h"

// ------ Helper functions ------

void calculateSolidMeshOfType(const Cutout& cutout, std::vector<TextureVertex>& vertices, std::vector<uint32_t>* indices = nullptr) {
    constexpr double defWidthHeight = 50.0f;  // TODO(Jannis): define this in one place (@see getRectangularDimensions)
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
    if (indices) {
        uint32_t offset = (uint32_t)vertices.size();
        for (uint32_t index : poly.indices) {
            indices->push_back(offset + index);
        }
    }
    vertices.insert(vertices.end(), poly.vertices.begin(), poly.vertices.end());
}

void calculateMeshForSlit(const Element& element, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    // Deserialize to get SlitBehaviour
    SlitBehaviour slit = deserializeSlit(element.m_behaviour);

    // Calculate vertices for the beamstop
    calculateSolidMeshOfType(slit.m_beamstopCutout, vertices, &indices);

    // Calculate vertices for the outer slit
    std::vector<TextureVertex> outerSlitVertices;
    calculateSolidMeshOfType(element.m_cutout, outerSlitVertices);

    // Calculate vertices for the opening
    std::vector<TextureVertex> openingVertices;
    calculateSolidMeshOfType(slit.m_openingCutout, openingVertices);

    uint32_t numOuterSlitVertices = (uint32_t)outerSlitVertices.size();
    uint32_t numOpeningVertices = (uint32_t)openingVertices.size();
    if (numOpeningVertices < numOuterSlitVertices) {
        interpolateConvexPolygon(openingVertices, numOuterSlitVertices);
        numOpeningVertices = numOuterSlitVertices;
    } else if (numOpeningVertices > numOuterSlitVertices) {
        interpolateConvexPolygon(outerSlitVertices, numOpeningVertices);
        numOuterSlitVertices = numOpeningVertices;
    }

    auto distanceMatrix = calculateDistanceMatrix(outerSlitVertices, openingVertices);

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

void planarTriangulation(const RAYX::OpticalElement& element, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    // The slit behaviour needs special attention, since it is basically three cutouts (the slit, the beamstop and the opening)
    if (element.m_element.m_behaviour.m_type == BTYPE_SLIT) {
        calculateMeshForSlit(element.m_element, vertices, indices);
    } else {
        calculateSolidMeshOfType(element.m_element.m_cutout, vertices, &indices);
    }
}

bool isPlanar(const QuadricSurface& q) { return (q.m_a11 == 0 && q.m_a22 == 0 && q.m_a33 == 0) && (q.m_a14 != 0 || q.m_a24 != 0 || q.m_a34 != 0); }

// ------ Interface functions ------

/**
 * This function takes optical elements and categorizes them for efficient triangulation.
 */
void triangulateObject(const RAYX::OpticalElement& element, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices) {
    switch (static_cast<int>(element.m_element.m_surface.m_type)) {
        case STYPE_PLANE_XZ: {
            planarTriangulation(element, vertices, indices);
            break;
        }
        case STYPE_QUADRIC: {
            QuadricSurface q = deserializeQuadric(element.m_element.m_surface);
            if (isPlanar(q)) {
                planarTriangulation(element, vertices, indices);
            } else {
                traceTriangulation(element, vertices, indices);
            }
            break;
        }
        case STYPE_TOROID: {
            traceTriangulation(element, vertices, indices);
            break;
        }
        default:
            RAYX_ERR << "Unknown element type: " << element.m_element.m_surface.m_type;
            break;
    }
}
