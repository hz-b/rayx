#pragma once

#include "Vertex.h"

struct Vertex;

struct Polygon {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void calculateForQuadrilateral(double widthA, double widthB, double lengthA, double lengthB);
    void calculateForElliptical(double diameterA, double diameterB);
};

/**
 * This function takes a polygon and interpolates it to have the specified number of vertices.
 * The polygon is assumed to be convex.
 */
void interpolateConvexPolygon(std::vector<Vertex>& polyVertices, uint32_t targetNumber);

std::vector<std::vector<double>> calculateDistanceMatrix(const std::vector<Vertex>& outerSlitVertices, const std::vector<Vertex>& openingVertices);