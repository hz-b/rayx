#pragma once

#include <Material/Material.h>

#include <memory>
#include <vector>

#include "Beamline/LightSource.h"
#include "Beamline/OpticalElement.h"
#include "Core.h"
#include "Shared/Ray.h"

namespace RAYX {
struct OpticalElement;
class LightSource;

/*
 * The Beamline class is a container for OpticalElements and LightSources.
 * It represents a central structure in our simulation process.
 */
class RAYX_API Beamline {
  public:
    Beamline();
    ~Beamline();

    std::vector<Ray> getInputRays() const;

    /**
     * @brief Quality-of-life function to calculate the smallest possible
     * MaterialTables which cover all materials from this beamline
     *
     * @return MaterialTables
     */
    MaterialTables calcMinimalMaterialTables() const;

    std::vector<OpticalElement> m_OpticalElements;
    std::vector<std::shared_ptr<LightSource>> m_LightSources;
};

}  // namespace RAYX
