#pragma once

#include <vector>

#include "Beamline/OpticalElement.h"
#include "RenderObject.h"
#include "Tracer/Tracer.h"

/**
 * @brief Triangulates optical elements for rendering.
 * @param elements A vector of optical elements to be triangulated.
 * @return A vector of RenderObject, which are the triangulated version of the input elements.
 */
std::vector<RenderObject> triangulateObjects(const std::vector<RAYX::OpticalElement>& elements, Device& device);
