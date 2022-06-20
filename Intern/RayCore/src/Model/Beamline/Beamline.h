#pragma once

#include <Material/Material.h>
#include <Tracer/RayList.h>

#include <array>
#include <memory>
#include <vector>

#include "Core.h"
#include "glm.hpp"

namespace RAYX {
class OpticalElement;
class LightSource;

class RAYX_API Beamline {
  public:
    Beamline();
    ~Beamline();

    RayList getInputRays() const;

    // quality-of-life function to calculate the smallest possible
    // MaterialTables which cover all materials from this beamline
    MaterialTables calcMinimalMaterialTables() const;

    std::vector<std::shared_ptr<OpticalElement>> m_OpticalElements;
    std::vector<std::shared_ptr<LightSource>> m_LightSources;

    // a mostly-empty vector, containing input rays to be used in addition to
    // those from the light sources. relevant for testing!
    std::vector<Ray> m_extraRays;
};

}  // namespace RAYX
