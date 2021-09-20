#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API PlaneMirror : public OpticalElement {

    public:

        PlaneMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        PlaneMirror(const char* name, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);
        PlaneMirror();
        ~PlaneMirror();

    };

} // namespace RAYX