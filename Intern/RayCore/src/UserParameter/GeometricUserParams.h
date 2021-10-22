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

    class RAYX_API GeometricUserParams {

    public:
        // constructor for mirrors
        GeometricUserParams(double incidenceAngle);
        // constructor for gratings
        GeometricUserParams(int mount, double deviation, double normalIncidence, double lineDensity, double designEnergy, double additionalOrder, int orderOfDiffraction);
        // constructor for rzp
        GeometricUserParams(int mount, int imageType, double deviationAngle, double grazingIncidence, double grazingExitAngle, double sourceEnergy, double designEnergy, double orderOfDiffraction, double designOrderOfDiffraction,
            double designAlphaAngle, double designBetaAngle, double mEntrance, double mExit, double sEntrance, double sExit);
        // constructor for EllipsoidMirror
        GeometricUserParams(double incidence, double entranceArmLength, double exitArmLength);
        GeometricUserParams();
        ~GeometricUserParams();

        void focus(double angle, double designEnergy, double lineDensity, double orderOfDiffraction);
        double calcDz00(int imageType, double designWavelength, double designAlphaAngle, double designBetaAngle, double designOrderOfDiffraction, double sEntrance, double sExit, double mEntrance, double mExit);
        double calcFresnelZOffset(double designAlphaAngle, double designBetaAngle, double sEntrance, double sExit);
        double rzpLineDensityDZ(int imageType, glm::dvec3 intersection, glm::dvec3 normal, double designWavelength, double designAlphaAngle, double designBetaAngle, double designOrderOfDiffraction, double sEntrance, double sExit, double mEntrance, double mExit);
        double calcTangentAngle(double incidence, double entranceArmLength, double exitArmLength);
        
        void calcMirrorRadius(double entranceArmLength, double exitArmLength);
        void calcGratingRadius(int mount, double deviation, double entranceArmLength, double exitArmLength);
        void calcTorusRadius(double incidenceAngle, double sEntrance, double sExit, double mEntrance, double mExit);

        double getAlpha();
        double getBeta();
        double getRadius();
        double getShortRadius();

        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE };
    private:
        double m_alpha;
        double m_beta;
        double m_radius; // only for spheres and toroid (long radius)
        double m_shortRadius; // only for toroid
    };
}