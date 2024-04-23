#pragma once

#include <Shader/Cutout.h>

#include "Vertex.h"

struct Vertex;

struct Polygon {
    std::vector<TextureVertex> vertices;
    std::vector<uint32_t> indices;

    void calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB);
    void calculateForElliptical(double diameterA, double diameterB);
};

/**
 * This function takes a polygon and interpolates it to have the specified number of vertices.
 * The polygon is assumed to be convex.
 */
void interpolateConvexPolygon(std::vector<TextureVertex>& polyVertices, uint32_t targetNumber);

std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<TextureVertex>& outerSlitVertices,
                                                         const std::vector<TextureVertex>& openingVertices);

/**
 * @brief Returns the dimensions (width and length) for various types of cutouts.
 * @param cutout Reference to the Cutout object.
 * @return A pair containing width and length as double values.
 */
std::pair<double, double> getRectangularDimensions(const RAYX::Cutout& cutout);
