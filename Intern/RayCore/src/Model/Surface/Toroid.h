#pragma once

#include "Core.h"
#include "Surface.h"
#include "utils.h"

#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAYX {
class RAYX_API Toroid : public Surface {
  public:
    // ctor
    Toroid();
    Toroid(const std::vector<double> inputPoints);
    Toroid(double longRadius, double shortRadius, double elementType);
    // dtor
    ~Toroid();

    void setParameters(std::vector<double> inputPoints);

    std::vector<double> getParams() const;

  private:
    // 16 values that store the 2 surface params for the toroid and also some settings for the shader to know how to interpret the input
    std::vector<double> m_parameters;
    double m_longRadius;
    double m_shortRadius;
};
}  // namespace RAYX