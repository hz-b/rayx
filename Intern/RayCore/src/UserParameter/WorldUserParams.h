#pragma once

#include <math.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Core.h"
#include "Ray.h"
#include "Model/Beamline/OpticalElement.h"
#include "utils.h"
#include <memory>
#include <glm.hpp>

namespace RAYX
{
    class RAYX_API WorldUserParams {

    public:

        // old constructor with several user parameters to calculate incidence and exit angle (alpha, beta) has been moved to somewhere else
        // PlaneGrating(const char* name, const int mount, const double width, const double height, const double deviation, const double normalIncidence, const double azimuthal, const double distanceToPreceedingElement, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const double fixFocusConstantCFF, const int additional_zero_order, const std::vector<double> misalignmentParams, const std::vector<double> vls, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);

        WorldUserParams(double alpha, double beta, double chi, double dist, std::vector<double> misalignment);
        WorldUserParams();
        ~WorldUserParams();

        glm::dmat4x4 calcE2B();
        glm::dmat4x4 getMisalignmentOrientation();

        glm::dvec4 calcPosition(WorldUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or);
        glm::dvec4 calcPosition();
        glm::dmat4x4 calcOrientation(WorldUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or);
        glm::dmat4x4 calcOrientation();

        std::vector<double> getMisalignment();


    private:
        double m_incidenceAngle;
        double m_exitAngle;
        double m_azimuthalAngle;
        double m_dist;
        std::vector<double> m_misalignment;

    };

} // namespace RAYX
