#pragma once

#include <vector>

#include "Beamline/OpticalElement.h"
#include "RenderObject.h"
#include "Tracer/Tracer.h"

// TODO: documentation
// TODO: its a bit weird to pass device here

/**
 * @brief Triangulates optical elements for rendering.
 * @param elements A vector of optical elements to be triangulated.
 * @param useMarchinCubes Flag to determine if Marching Cubes triangulation should be used.
 * @return A vector of RenderObject, which are the triangulated version of the input elements.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, Device& device, bool useMarchinCubes = false);

/**
 * @brief Generates visual representations of rays based on bundle history and optical elements.
 * @param bundleHist RAYX-Core type, providing details of ray interactions in the beamline.
 * @param elements A vector of optical elements used for coordinate conversions.
 * @return A vector of lines, which visually represents the paths of rays in the beamline.
 */
std::vector<Line> getRays(const RAYX::BundleHistory& bundleHist, const std::vector<RAYX::OpticalElement>& elements);
