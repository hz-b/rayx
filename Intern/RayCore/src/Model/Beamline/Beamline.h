#pragma once

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

    void addOpticalElement(const std::shared_ptr<OpticalElement> q);
    void addOpticalElement(const char* name,
                           const std::array<double, 4*4>& inputPoints,
                           std::array<double, 4 * 4> inputInMatrix,
                           std::array<double, 4 * 4> inputOutMatrix,
                           std::array<double, 4 * 4> OParameters,
                           std::array<double, 4 * 4> EParameters);
    void addOpticalElement(const char* name, std::array<double, 4*4>&& inputPoints,
                           std::array<double, 4 * 4>&& inputInMatrix,
                           std::array<double, 4 * 4>&& inputOutMatrix,
                           std::array<double, 4 * 4>&& OParameters,
                           std::array<double, 4 * 4>&& EParameters);

    std::vector<std::shared_ptr<OpticalElement>> m_OpticalElements;
    std::vector<std::shared_ptr<LightSource>> m_LightSources;
};

}  // namespace RAYX