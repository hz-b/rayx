#pragma once

#include "Core.h"
#include "Surface.h"
#include "utils.h"
// Unused Macro
#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAYX {
class RAYX_API Quadric : public Surface {
  public:
    // ctor
    Quadric();
    Quadric(const glm::dmat4x4 inputPoints);
    // dtor
    ~Quadric();

    [[maybe_unused]] [[maybe_unused]] void setAnchorPoints(
        glm::dmat4x4 inputPoints);
    glm::dmat4x4 getParams() const;

  private:
    // Quadric parameters; will be turned into 4x4 matrix
    glm::dmat4x4 m_parameters;
};
}  // namespace RAYX