#pragma once
#include "Model/Surface/Quadric.h"
#include "Model/Beamline/OpticalElement.h"
#include <Data/xml.h>

namespace RAYX
{

    class RAYX_API SphereMirror : public OpticalElement {

    public:

        // SphereMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double distanceToPreceedingElement, const double entranceArmLength, const double exitArmLength, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);

        // calculate radius in this class
        SphereMirror(const char* name, const int geometricalShape, const double width, const double height, const double grazingIncidenceAngle, glm::dvec4 position, glm::dmat4x4 orientation, const double entranceArmLength, const double exitArmLength, const std::vector<double> slopeError);
        // radius is precalculated and given as a parameter
        SphereMirror(const char* name, const int geometricalShape, const double width, const double height, double radius, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);
        SphereMirror();
        ~SphereMirror();

        static std::shared_ptr<SphereMirror> createFromXML(rapidxml::xml_node<>*);

        void calcRadius();
        double getRadius() const;
        double getEntranceArmLength() const;
        double getExitArmLength() const;

    private:
        double m_radius;
        double m_entranceArmLength;
        double m_exitArmLength;
        // grazing incidence, in rad
        double m_grazingIncidenceAngle;

    };

} // namespace RAYX