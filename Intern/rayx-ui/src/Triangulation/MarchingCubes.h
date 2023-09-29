#pragma once

#include <glm/glm.hpp>

#include "Beamline/OpticalElement.h"
#include "RenderObject.h"
#include "Vertex.h"

#define GRIDSIZE 10

// Marching Cubes
RenderObject marchingCubeTriangulation(const RAYX::OpticalElement& element, Device& device);

// ------ Helper functions ------
std::vector<Triangle> trianglesFromQuadric(const double* quadric, Cutout cutout);  // TODO: make nicer

glm::vec3 getPositionAtCorner(int cornerIndex);
Vertex interpolateVertex(int edgeIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY, int offsetZ,
                         glm::vec3 scale);
double evaluateQuadricAtPosition(const double surface[16], const glm::vec4& pos);
int determineMarchingCubesCase(const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int x, int y, int z);
std::vector<Triangle> lookupTrianglesForCase(int caseIndex, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE], int offsetX, int offsetY,
                                             int offsetZ, glm::vec3 scale);
Vertex getVertexFromEdge(int edgeIndex);
double getScalarValueAtCorner(int x, int y, int z, const double scalarGrid[GRIDSIZE][GRIDSIZE][GRIDSIZE]);
