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

        
        WorldUserParams(double alpha, double beta, double chi, double dist, std::vector<double> misalignment, double tangentAngle = 0);
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
        double m_tangentAngle; // Only for ellipsoid

    };

} // namespace RAYX
