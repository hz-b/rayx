#pragma once
#include <Data/xml.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

enum class CentralBeamstop {
    None,
    Rectangle,
    Elliptical
};  ///< central beamstop shape

class RAYX_API Slit : public OpticalElement {
  public:
    Slit(const char* name, OpticalElement::GeometricalShape geometricalShape,
         CentralBeamstop beamstop, double width, double height,
         glm::dvec4 position, glm::dmat4x4 orientation, double beamstopWidth,
         double beamstopHeight);

    static std::shared_ptr<Slit> createFromXML(xml::Parser);

    CentralBeamstop getCentralBeamstop() const;
    double getBeamstopWidth() const;
    double getBeamstopHeight() const;

  private:
    CentralBeamstop m_centralBeamstop;
    double m_beamstopWidth;
    double m_beamstopHeight;
};

}  // namespace RAYX