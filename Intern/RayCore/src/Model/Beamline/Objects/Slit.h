#pragma once
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

// TODO CentralBeamstop could also be stored as a cutout, might be good for code re-use
enum class CentralBeamstop { None, Rectangle, Elliptical };  ///< central beamstop shape

class RAYX_API Slit : public OpticalElement {
  public:
    Slit(const DesignObject&);

    inline int getElementType() const { return TYPE_SLIT; }

    CentralBeamstop getCentralBeamstop() const;
    double getBeamstopWidth() const;
    double getBeamstopHeight() const;
    std::array<double, 16> getElementParams() const;

  private:
    CentralBeamstop m_centralBeamstop;
    double m_beamstopWidth;
    double m_beamstopHeight;

    // the cutout for a slit is always CTYPE_UNLIMITED (as rays should only be able to bypass a slit by going through it - not around)
    // Hence the cutout of the gap (i.e. the "hole", the part of the Slit where rays actually go through) needs to be stored separately:
    Cutout m_gapCutout;
};

}  // namespace RAYX
