#pragma once

#include "Core.h"
#include "Surface.h"
#include "utils.h"

#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAYX {
class RAYX_API Quadric : public Surface {
  public:
    // ctor
    Quadric();
    Quadric(const std::array<double, 4*4> inputPoints);
    // dtor
    ~Quadric();

    void setAnchorPoints(std::array<double, 4*4> inputPoints);

    std::array<double, 4*4> getParams() const;

  private:
    // Quadric parameters; will be turned into 4x4 matrix
    std::array<double, 4*4> m_parameters;
};
}  // namespace RAYX