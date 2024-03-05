#pragma once

#include <array>
#include <glm.hpp>
#include <optional>

#include "Core.h"
#include "Data/xml.h"
#include "DesignElement/DesignElement.h"
#include "Shader/Constants.h"
#include "angle.h"

namespace RAYX {

enum class GratingMount { Deviation, Incidence };
enum class ParaboloidType { Focussing, Collimate };
//enum class CurvatureType { Plane, Toroidal, Spherical, Cubic, Cone, Cylinder, Ellipsoid, Paraboloid};
enum class BehaviourType { Mirror, Grating, Slit, Rzp, ImapePlane};
enum class FigureRotation { Yes, Plane, A11 };

struct DesignElement; //TODO Fanny see where the forward declaration has to go

// This is the current "high-level" representation of an OpticalElement.
// As you can see, it is precisely the same as the shader-internal representation `Element` in addition with a name.
// Hence, this is not suitable for work with the GUI, as it does not contain DesignParameters.
// Originally, we had a more rich structure of higher-level OpticalElements.
// Although we decided to remove it due to other flaws, it might still make sense to have a look at it.
// You can find the old implementation at e625e4a4 in the git history.
struct RAYX_API OpticalElement {
    Element m_element;
    std::string m_name;
};

// calculations the transformation matrices that will then be given into the shader.
// if plane == DesignPlane::XY, it will additionally swap the y and z coordinates to make the shader-internal code consistent.
// if `calcInMatrix`, this returns the "in-matrix", and otherwise the "out-matrix".
RAYX_API glm::dmat4 calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, bool calcInMatrix, DesignPlane plane);

// convenience wrapper around calcTransformationMatrices.
glm::dmat4 defaultInMatrix(const DesignObject& dobj, DesignPlane plane);
glm::dmat4 defaultOutMatrix(const DesignObject& dobj, DesignPlane plane);

// convenience wrapper around dobj.parseMaterial().
double defaultMaterial(const DesignObject& dobj);

// constructs an Element given all of its components. Some information that is not explicitly given, will be parsed from the `dobj`.
Element makeElement(const DesignElement& dele, Behaviour behaviour, Surface surface, std::optional<Cutout> cutout = {}, DesignPlane plane=DesignPlane::XZ);

Element makeExperts(const DesignElement& dobj);
Element makeExpertsCubic(const DesignElement& dobj);


// creates a Grating Behaviour from the parameters given in `dobj`.
Behaviour makeGrating(const DesignElement& dele);

}  // namespace RAYX
