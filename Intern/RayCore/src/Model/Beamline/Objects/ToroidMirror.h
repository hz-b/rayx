#pragma once
#include "Model/Surface/Toroid.h"
#include "Model/Beamline/OpticalElement.h"
#include <Data/xml.h>

namespace RAYX
{

    class RAYX_API ToroidMirror : public OpticalElement {

    public:

        // new shortened constructor with position and orientation in world coordinates pre calculated
        ToroidMirror(const char* name, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double incidenceAngle, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> slopeError);
        // old, long constructor that uses angles and distance to derive transformation matrices
        //ToroidMirror(const char* name, const int geometricalShape, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        ToroidMirror();
        ~ToroidMirror();

        static std::shared_ptr<ToroidMirror> createFromXML(rapidxml::xml_node<>*);

        void calcRadius(double incidenceAngle);
        double getRadius() const;
        double getSagittalEntranceArmLength() const;
        double getSagittalExitArmLength() const;
        double getMeridionalEntranceArmLength() const;
        double getMeridionalExitArmLength() const;

    private:
        double m_sagittalEntranceArmLength;
        double m_sagittalExitArmLength;
        double m_meridionalEntranceArmLength;
        double m_meridionalExitArmLength;
        double m_longRadius;
        double m_shortRadius;
    };

} // namespace RAYX