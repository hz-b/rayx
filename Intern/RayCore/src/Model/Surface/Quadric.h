#pragma once

#include <glm.hpp>

#include "Core.h"
#include "Surface.h"
// Unused Macro
#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAYX {
class RAYX_API Quadric : public Surface {
  public:
    // ctor
    Quadric();
    Quadric(const std::array<double, 16> inputPoints);

    int getSurfaceType() const;

    // dtor
    ~Quadric();

    [[maybe_unused]] [[maybe_unused]] void setAnchorPoints(std::array<double, 16> inputPoints);
    std::array<double, 16> getParams() const;

  private:
    // Quadric parameters; will be turned into 4x4 matrix
    std::array<double, 16> m_parameters;
};
}  // namespace RAYX
