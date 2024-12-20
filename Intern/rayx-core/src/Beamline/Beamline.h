#pragma once

#include <memory>
#include <vector>

#include "Beamline/LightSource.h"
#include "Core.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Design/Group.h"
#include "Material/Material.h"
#include "Shader/Ray.h"

namespace RAYX {

/*
 * The Beamline class is a container for a hierarchical tree of OpticalElements and LightSources.
 * It represents a central structure in our simulation process.
 */
class RAYX_API Beamline {
  public:
    Beamline();

    // Iterates over the m_DesignSources, and collects the rays they emit.
    std::vector<Ray> getInputRays(int thread_count = 1) const;

    /**
     * @brief Quality-of-life function to calculate the smallest possible
     * MaterialTables which cover all materials from this beamline
     *
     * @return MaterialTables
     */
    MaterialTables calcMinimalMaterialTables() const;

    /**
     * @brief Adds a node to the root Group of the beamline hierarchy.
     * @param node The BeamlineNode to add (can be a DesignElement, DesignSource, or nested Group).
     */
    void addNodeToRoot(
        BeamlineNode&& node);  // TODO: Adding only to root note seems good practice but means we need to create nested structures during parsing

  private:
    // Root node of the beamline hierarchy. This group will always be positioned at 0,0,0 with an identity matrix for the orientation
    Group m_RootGroup;  // TODO: Should the group just be the new beamline? Both are the same thing conceptually
};

}  // namespace RAYX