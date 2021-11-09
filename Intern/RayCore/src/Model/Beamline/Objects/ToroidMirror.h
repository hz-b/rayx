#pragma once
#include "Model/Surface/Toroid.h"
#include "Model/Beamline/OpticalElement.h"
#include <Data/xml.h>

namespace RAYX
{

    class RAYX_API ToroidMirror : public OpticalElement {

    public:

        ToroidMirror(const char* name, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double incidenceAngle, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const std::vector<double> slopeError);

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