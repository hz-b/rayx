#pragma once

#include <glm/glm.hpp>

#include "Beamline/OpticalElement.h"
#include "RenderObject.h"
#include "Vertex.h"

#define GRIDSIZE 10

/**
 * @brief Triangulates optical elements using the Marching Cubes algorithm.
 * @param elements A vector of optical elements to be triangulated.
 * @return A vector of RenderObject, which are the triangulated version of the input elements.
 */
RenderObject marchingCubeTriangulation(const RAYX::OpticalElement& element, Device& device);

// ------ Helper functions ------
/**
 * @brief Generates triangles from a given quadric surface and cutout.
 * @param quadric The coefficients of the quadric surface (double[16]).
 * @param cutout The cutout object that defines the region of interest.
 * @return A vector of Triangle, representing the triangulated portion of the quadric surface.
 */
std::vector<Triangle> trianglesFromQuadric(const double* quadric, Cutout cutout);  // TODO: make nicer

/**
 * @brief Computes the position of a vertex at a given corner index.
 * @param cornerIndex The index of the corner.
 * @return The 3D position of the vertex.
 */
glm::vec3 getPositionAtCorner(int cornerIndex);

/**
 * @brief Interpolates a vertex on an edge given a scalar grid and offsets.
 * @param edgeIndex The index of the edge.
 * @param scalarGrid The scalar grid defining the volume.
 * @param offsetX The x offset for the position.
 * @param offsetY The y offset for the position.
 * @param offsetZ The z offset for the position.
 * @param scale The scaling factor for the position.
 * @return The interpolated Vertex object.
 */
Vertex interpolateVertex(int edgeIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ,
                         glm::vec3 scale);

/**
 * @brief Evaluates a quadric surface at a given position.
 * @param surface The coefficients of the quadric surface.
 * @param pos The 4D position at which to evaluate the quadric.
 * @return The evaluated scalar value.
 */
double evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos);

/**
 * @brief Determines the Marching Cubes case for a given grid cell.
 * @param scalarGrid The scalar grid defining the volume.
 * @param x The x coordinate of the cell.
 * @param y The y coordinate of the cell.
 * @param z The z coordinate of the cell.
 * @return The case index for the Marching Cubes algorithm.
 */
int determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z);

/**
 * @brief Looks up and constructs triangles for a given Marching Cubes case.
 * @param caseIndex The case index for the Marching Cubes algorithm.
 * @param scalarGrid The scalar grid defining the volume.
 * @param offsetX The x offset for the position.
 * @param offsetY The y offset for the position.
 * @param offsetZ The z offset for the position.
 * @param scale The scaling factor for the position.
 * @return A vector of Triangle, representing the triangulated geometry for the given case.
 */
std::vector<Triangle> lookupTrianglesForCase(int caseIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY,
                                             int offsetZ, glm::vec3 scale);

/**
 * @brief Retrieves a vertex from a given edge index.
 * @param edgeIndex The index of the edge.
 * @return The Vertex object associated with the edge index.
 */
Vertex getVertexFromEdge(int edgeIndex);

/**
 * @brief Retrieves the scalar value at a given grid corner.
 * @param x The x coordinate of the corner.
 * @param y The y coordinate of the corner.
 * @param z The z coordinate of the corner.
 * @param scalarGrid The scalar grid defining the volume.
 * @return The scalar value at the specified corner.
 */
double getScalarValueAtCorner(int x, int y, int z, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]);
