#pragma once

#include "Shader/Cutout.h"
#include "Vertex.h"

struct Vertex;

struct Outline {
    std::vector<TextureVertex> vertices;

    void calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB);
    void calculateForElliptical(double diameterA, double diameterB);
};

/**
 * @brief Returns the dimensions (width and length) for various types of cutouts.
 * @param cutout Reference to the Cutout object.
 * @return A pair containing width and length as double values.
 */
std::pair<double, double> getRectangularDimensions(const RAYX::Cutout& cutout);
