#pragma once
#include <Data/xml.h>
#include <Material/Material.h>
#include <string.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {
enum class FigureRotation { Yes, Plane, A11 };

// TODO(Jannis): rename or turn into surface
class RAYX_API Ellipsoid : public OpticalElement {
  public:
    Ellipsoid(const char* name, OpticalElement::GeometricalShape geometricalShape, const double width, const double height,
              const double azimuthalAngle, glm::dvec4 position, const double LongHalfAxisA, const double ShortHalfAxisB,
              const double DesignAngle, glm::dmat4x4 orientation, const double grazingIncidence, const double entranceArmLength,
              const double exitArmLength, FigureRotation figRot, const double a_11, const std::array<double, 7> slopeError, Material mat);

    void calcHalfAxes();
    void calculateCenterFromHalfAxes(double angle);
    double getRadius() const;
    double getExitArmLength() const;
    double getEntranceArmLength() const;
    double getY0() const;  // center of ellipsoid
    double getZ0() const;  // -"-
    double getIncidenceAngle() const;
    double getShortHalfAxisB() const;  // b
    double getLongHalfAxisA() const;   // a
    double getOffsetY0() const;        // always = 0?
    double getAlpha1() const;          // from tangent angle and grazing incidence???
    // derived params
    double getTangentAngle() const;
    double getA34() const;
    double getA33() const;
    double getA44() const;
    double getHalfAxisC() const;

    glm::dmat4x4 getElementParameters() const;

    static std::shared_ptr<Ellipsoid> createFromXML(const xml::Parser&);

  private:
    // user parameters:
    // double m_radius;
    // grazing incidence, in rad
    double m_incidence;
    double m_entranceArmLength;
    double m_exitArmLength;
    FigureRotation m_figureRotation;
    double m_a11;  // param for quadric funciton, given by user

    // derived params, needed on shader
    double m_tangentAngle;  // == alpha1 (= x-rotational offset to the ellipsoid
                            // coordianate system
    double m_a34;           // paramters for quadric equation
    double m_a33;
    double m_a44;
    double m_a22;
    double m_a23;
    double m_a24;
    double m_y0;              // offset to center of ellipsoid
    double m_z0;              // -"-
    double m_shortHalfAxisB;  // b
    // double m_A;
    // double m_B;
    // double m_a0;
    double m_longHalfAxisA;  // a
    double m_offsetY0;       // always = 0?
    double m_halfAxisC;      // derived from figure_rotation, a_11 and half axes a, b
    double m_designGrazingAngle;
};

}  // namespace RAYX
