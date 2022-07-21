#pragma once

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
    Toroid(const std::array<double, 4 * 4> inputPoints);
    Toroid(double longRadius, double shortRadius, double elementType,
           Material mat);
    // dtor
    ~Toroid();

    [[maybe_unused]] void setParameters(std::array<double, 4 * 4> inputPoints);

    std::array<double, 4 * 4> getParams() const;

  private:
    // 16 values that store the 2 surface params for the toroid and also some
    // settings for the shader to know how to interpret the input
    std::array<double, 4 * 4> m_parameters;
    double m_longRadius;
    double m_shortRadius;
};
}  // namespace RAYX