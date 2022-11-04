#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

enum class CentralBeamstop { None, Rectangle, Elliptical };  ///< central beamstop shape

class RAYX_API Slit : public OpticalElement {
  public:
    Slit(const DesignObject&);

    CentralBeamstop getCentralBeamstop() const;
    double getBeamstopWidth() const;
    double getBeamstopHeight() const;
    glm::dmat4x4 getElementParameters() const;

  private:
    CentralBeamstop m_centralBeamstop;
    double m_beamstopWidth;
    double m_beamstopHeight;
};

}  // namespace RAYX
