#pragma once

#include <memory>
#include <vector>

#include "Core.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Material/Material.h"
#include "Shader/Ray.h"

namespace RAYX {

/*
 * The Beamline class is a container for OpticalElements and LightSources.
 * It represents a central structure in our simulation process.
 */
class RAYX_API Beamline {
  public:
    Beamline();
    ~Beamline();

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
