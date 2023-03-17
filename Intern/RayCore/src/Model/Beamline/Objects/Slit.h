#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

enum class CentralBeamstop { None, Rectangle, Elliptical };  ///< central beamstop shape

class RAYX_API Slit : public OpticalElement {
  public:
    Slit(const DesignObject&);

    inline int getElementType() const { return TY_SLIT; }

    CentralBeamstop getCentralBeamstop() const;
    double getBeamstopWidth() const;
    double getBeamstopHeight() const;
    std::array<double, 16> getElementParams() const;

  private:
    CentralBeamstop m_centralBeamstop;
    double m_beamstopWidth;
    double m_beamstopHeight;

    // the width, height for a slit are infinite (as no rays should be able to bypass a slit by being out of it's hitbox)
    // Hence the width, height of the gap (i.e. the part of the Slit that does not absorb rays) needs to be stored separately:
    // TODO this should be gapCutoutType and gapCutoutParams later!
    double m_gapWidthA;
    double m_gapWidthB;
    double m_gapHeight;
};

}  // namespace RAYX
