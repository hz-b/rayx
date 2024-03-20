#pragma once

#include <Material/Material.h>

#include <memory>
#include <vector>

#include "Beamline/LightSource.h"
#include "Beamline/OpticalElement.h"
#include "Core.h"
#include "Shader/Ray.h"
#include "DesignElement/DesignElement.h"
#include "DesignElement/DesignSource.h"

namespace RAYX {
class LightSource;
struct OpticalElement;
struct DesignElement; //TODO Fanny see where the forward declaration has to go
struct DesignSource; //TODO Fanny see where the forward declaration has to go


/*
 * The Beamline class is a container for OpticalElements and LightSources.
 * It represents a central structure in our simulation process.
 */
class RAYX_API Beamline {
  public:
    Beamline();
    ~Beamline();

    // iterates over the m_LightSources, and collects the rays they emit.
    std::vector<Ray> getInputRays(int thread_count = 1) const;

    /**
     * @brief Quality-of-life function to calculate the smallest possible
     * MaterialTables which cover all materials from this beamline
     *
     * @return MaterialTables
     */
    MaterialTables calcMinimalMaterialTables() const;

    std::vector<DesignElement> m_DesignElements;
    std::vector<DesignSource> m_DesignSources;
};

}  // namespace RAYX
