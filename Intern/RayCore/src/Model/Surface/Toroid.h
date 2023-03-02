#pragma once

#include <glm.hpp>

#include "Core.h"
#include "Material/Material.h"
#include "Surface.h"
#include "utils.h"

#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAYX {
class RAYX_API Toroid : public Surface {
  public:
    // ctor
    Toroid();
    Toroid(double longRadius, double shortRadius);

    int getSurfaceType() const;

    // dtor
    ~Toroid();

    glm::dmat4x4 getParams() const;

  private:
    // 16 values that store the 2 surface params for the toroid and also some
    // settings for the shader to know how to interpret the input
    glm::dmat4x4 m_parameters;
};
}  // namespace RAYX
