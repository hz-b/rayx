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
void triangulateObject(const Element compiled, std::vector<TextureVertex>& vertices, std::vector<uint32_t>& indices);
