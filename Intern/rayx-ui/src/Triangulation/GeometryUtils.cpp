#include "GeometryUtils.h"

#include "Colors.h"

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
            return {27.3, 27.3};
            break;
        }
    }

    return {width, length};
}

void Outline::calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB) {
    if (widthA <= 0 || widthB <= 0 || lengthA <= 0 || lengthB <= 0) {
        return;
    }
    // Right handed coord system when looking down the negative y axis: x is to the left and z is up
    vertices = {
        TextureVertex(glm::vec4(-widthB / 2.0f, 0, -lengthA / 2.0f, 1.0f), glm::vec2(1.0f, 1.0f)),  // Bottom-right
        TextureVertex(glm::vec4(-widthA / 2.0f, 0, lengthA / 2.0f, 1.0f), glm::vec2(1.0f, 0.0f)),   // Top-right
        TextureVertex(glm::vec4(widthA / 2.0f, 0, lengthB / 2.0f, 1.0f), glm::vec2(0.0f, 0.0f)),    // Top-left
        TextureVertex(glm::vec4(widthB / 2.0f, 0, -lengthB / 2.0f, 1.0f), glm::vec2(0.0f, 1.0f))    // Bottom-left
    };
}

void Outline::calculateForElliptical(double diameterA, double diameterB) {
    if (diameterA <= 0.0 || diameterB <= 0.0) {
        return;
    }
    constexpr uint32_t numVertices = 20;
    vertices.reserve(numVertices);

    // Calculate vertices
    for (uint32_t i = 0; i < numVertices; i++) {
        double angle = 2.0f * PI * i / numVertices;
        glm::vec4 pos = {-diameterA * cos(angle) / 2.0f, 0, diameterB * sin(angle) / 2.0f, 1.0f};
        vertices.emplace_back(pos, OPT_ELEMENT_COLOR);
    }
}
