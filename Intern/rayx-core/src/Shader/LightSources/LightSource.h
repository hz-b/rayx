#pragma once

#include <array>
#include <glm.hpp>
#include <string>
#include <vector>

#include "Beamline/Misalignment.h"
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

class RAYX_API ModelLightSource {
  public:
    ModelLightSource(const DesignSource&);

    /** yields the average energy of the energy distribution
     * m_EnergyDistribution */
    RAYX_FN_ACC static glm::dvec3 getDirectionFromAngles(double phi, double psi);

    /** the energy distribution used when deciding the energies of the rays. */
    // TODO: enable
    // EnergyDistribution m_EnergyDistribution;
    // double selectEnergy() const;

    int32_t m_numberOfRays;

    glm::dmat4x4 m_orientation = glm::dmat4x4();
    glm::dvec4 m_position      = glm::dvec4();

    Misalignment m_misalignmentParams;  // x, y, psi, phi
};

}  // namespace RAYX
