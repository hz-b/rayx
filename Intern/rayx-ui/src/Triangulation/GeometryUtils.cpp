#include "GeometryUtils.h"

#include "Colors.h"

void Polygon::calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB) {
    vertices = {
        Vertex({-widthB / 2.0f, 0, -lengthA / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),  // Bottom-left
        Vertex({-widthA / 2.0f, 0, lengthB / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),   // Top-left
        Vertex({widthA / 2.0f, 0, lengthB / 2.0f, 1.0f}, OPT_ELEMENT_COLOR),    // Top-right
        Vertex({widthB / 2.0f, 0, -lengthA / 2.0f, 1.0f}, OPT_ELEMENT_COLOR)    // Bottom-right
    };
    indices = {0, 1, 2, 2, 3, 0};
}

void Polygon::calculateForElliptical(double diameterA, double diameterB) {
    constexpr uint32_t numVertices = 20;
    vertices.reserve(numVertices);
    indices.reserve(numVertices * 3);

    // Calculate vertices
    for (uint32_t i = 0; i < numVertices; i++) {
        double angle = 2.0f * PI * i / numVertices;
        glm::vec4 pos = {diameterA * cos(angle) / 2.0f, 0, diameterB * sin(angle) / 2.0f, 1.0f};
        vertices.emplace_back(pos, OPT_ELEMENT_COLOR);
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
        glm::vec4 interpolatedColor = glm::mix(polyVertices[lowerIndex].color, polyVertices[upperIndex].color, fraction);

        interpolatedVertices.push_back({interpolatedPosition, interpolatedColor});
    }

    // Replace the original vertices with the interpolated ones
    polyVertices = std::move(interpolatedVertices);
}

std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<Vertex>& outerSlitVertices, const std::vector<Vertex>& openingVertices) {
    uint32_t numOuterSlitVertices = (uint32_t)outerSlitVertices.size();
    uint32_t numOpeningVertices = (uint32_t)openingVertices.size();

    std::vector<std::vector<double>> distanceMatrix(numOuterSlitVertices, std::vector<double>(numOpeningVertices, 0.0));

    for (uint32_t i = 0; i < numOuterSlitVertices; ++i) {
        for (uint32_t j = 0; j < numOpeningVertices; ++j) {
            distanceMatrix[i][j] = glm::distance(outerSlitVertices[i].pos, openingVertices[j].pos);
        }
    }

    return distanceMatrix;
}

/**
 * Given a Cutout object, this function calculates and returns the width and
 * length depending on the cutout's type (rectangle, ellipse, trapezoid, etc.).
 */
std::pair<double, double> getRectangularDimensions(const Cutout& cutout) {
    double width = 0.0;
    double length = 0.0;

    switch (static_cast<int>(cutout.m_type)) {
        case CTYPE_RECT: {
            RectCutout rect = deserializeRect(cutout);
            width = rect.m_width;
            length = rect.m_length;
            break;
        }
        case CTYPE_ELLIPTICAL: {
            EllipticalCutout ell = deserializeElliptical(cutout);
            width = ell.m_diameter_x;   // Diameter is essentially the max width
            length = ell.m_diameter_z;  // Diameter is the max length
            break;
        }
        case CTYPE_TRAPEZOID: {
            TrapezoidCutout trap = deserializeTrapezoid(cutout);
            width = std::max(trap.m_widthA, trap.m_widthB);  // max of the two sides
            length = trap.m_length;
            break;
        }
        default: {  // CTYPE_UNLIMITED and unknown types
            return {50.0, 50.0};
            break;
        }
    }

    return {width, length};
}