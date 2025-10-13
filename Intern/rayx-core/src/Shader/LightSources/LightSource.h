#pragma once

#include <array>
#include <glm.hpp>
#include <string>
#include <vector>

#include "Core.h"
#include "Element/Element.h"
#include "EnergyDistributions/EnergyDistribution.h"
#include "Rml/xml.h"
#include "Shader/Ray.h"

namespace RAYX {

enum class SourceDist { Uniform, Gaussian, Thirds, Circle };  // SourceDist::Thirds represents PixelSource Footprint
enum class ElectronEnergyOrientation { Clockwise, Counterclockwise };
enum class EnergySpreadUnit { EU_PERCENT, EU_eV };
enum class SigmaType { ST_STANDARD, ST_ACCURATE };
enum class SourcePulseType { None };

class DesignSource;

class RAYX_API LightSourceBase {
  protected:
    LightSourceBase(const DesignSource&);

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    RAYX_FN_ACC static glm::dvec3 getDirectionFromAngles(double phi, double psi);

    int32_t m_numberOfRays;
};

}  // namespace RAYX
