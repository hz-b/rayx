#pragma once

#include <glm.h>

#include <optional>

#include "Behaviour.h"
#include "Core.h"
#include "Cutout.h"
#include "Rml/xml.h"
#include "Shader/SlopeError.h"
#include "Surface.h"

namespace RAYX {

struct DesignElement;

/**
 * @brief Structure to represent an element in the ray tracing simulation.
 */
struct OpticalElement {
    glm::dmat4 m_inTrans;     ///< In-transformation matrix: Converts a point from world coordinates to element coordinates.
    glm::dmat4 m_outTrans;    ///< Out-transformation matrix: Converts a point from element coordinates back to world coordinates.
    Behaviour m_behaviour;    ///< Describes what happens to a ray once it collides with this OpticalElement.
    Surface m_surface;        ///< Describes how the OpticalElement's surface is curved.
    Cutout m_cutout;          ///< Limits the Surface to the dimensions of the actual OpticalElement.
    SlopeError m_slopeError;  ///< Describes a random noise in the normal vector of a particular surface point.
    double m_azimuthalAngle;  ///< Azimuthal angle at which this element is rotated around the "main-beam".
    double m_material;        ///< The material that this object is made of (see `enum class Material` from Material.h).
};

// Ensure OpticalElement does not introduce cost on copy or default construction.
static_assert(std::is_trivially_copyable_v<OpticalElement>);

RAYX_API glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix, DesignPlane plane);

// constructs an OpticalElement given all of its components. Some information that is not explicitly given, will be parsed from the ` dele`.
OpticalElement makeElement(const DesignElement& dele, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout = {},
                    DesignPlane plane = DesignPlane::XZ);

}  // namespace RAYX
