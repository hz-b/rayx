#pragma once

#include "Beamline/LightSource.h"

namespace RAYX {

class RAYX_API DipoleSource : public LightSource {
  public:
    DipoleSource() = default;
    DipoleSource(const DesignObject&);
    virtual ~DipoleSource() = default;

    std::vector<Ray> getRays() const override;

  private:
    // Geometric Params
    double m_bendingRadius;
    ElectronEnergyOrientation m_electronEnergyOrientation;
    SourcePulseType m_sourcePulseType;
    double m_photonFlux;
    SpreadType m_energySpreadType;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
