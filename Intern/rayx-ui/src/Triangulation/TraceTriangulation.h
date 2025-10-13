#pragma once

#include <vector>

#include "Element/Element.h"
#include "RenderObject.h"
#include "BundleHistory.h"

/**
 * @brief Traces the path of rays through an OpticalElement using a grid-based approach, and returns a RenderObject for visualization.
 * @param element The optical element through which rays are traced. It contains details about its surface,
 *                cutout, and behavior.
 * @param device  The Device object used for rendering the final output.
 *
 * @return RenderObject containing the vertices and indices needed for rendering the triangulated rays.
 *
 * @note This function uses the RAYX::MegaKernelTracer for ray tracing and relies on a grid-based approach for tracing rays.
 *
 * Example usage:
 * @code{cpp}
 *   RAYX::OpticalElement element = ...;
 *   Device device = ...;
 *   RenderObject renderObj = traceTriangulation(element, device);
 * @endcode
 */
void traceTriangulation(const RAYX::OpticalElement compiled, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices);

// ------ Helper functions ------

/**
 * @brief Creates a 2D grid of rays to be used for ray tracing.
 * @param size Grid size along one dimension.
 * @param width Total grid width.
 * @param length Total grid length.
 * @return 2D vector of Ray objects.
 */
std::vector<std::vector<Ray>> createRayGrid(size_t size, double width, double length);
