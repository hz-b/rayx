#include "GeometryUtils.h"

#include <vector>

#include "Colors.h"
#include "Shader/Constants.h"
#include "Shader/Variant.h"

/**
 * Given a Cutout object, this function calculates and returns the width and
 * length depending on the cutout's type (rectangle, ellipse, trapezoid, etc.).
 */
std::pair<double, double> getRectangularDimensions(const RAYX::Cutout& cutout) {
    double width = 0.0;
    double length = 0.0;

    RAYX::variant::visit(
        [&]<typename T>(const T& arg) {
            if constexpr (std::is_same_v<T, RAYX::Cutout::Rect>) {
                width = arg.m_width;
                length = arg.m_length;
            } else if constexpr (std::is_same_v<T, RAYX::Cutout::Elliptical>) {
                width = arg.m_diameter_x;
                length = arg.m_diameter_z;
            } else if constexpr (std::is_same_v<T, RAYX::Cutout::Trapezoid>) {
                width = std::max(arg.m_widthA, arg.m_widthB);
                length = arg.m_length;
            } else {
                // For Unlimited and any other unknown types, return default values
                width = 50.0;
                length = 50.0;
            }
        },
        cutout.m_variant);

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
        double angle = 2.0f * RAYX::PI * i / numVertices;
        glm::vec4 pos = {-diameterA * cos(angle) / 2.0f, 0, diameterB * sin(angle) / 2.0f, 1.0f};
        vertices.emplace_back(pos, OPT_ELEMENT_COLOR);
    }
}
