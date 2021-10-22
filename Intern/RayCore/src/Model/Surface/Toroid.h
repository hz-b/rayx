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
        Toroid(double longRadius, double shortRadius);
        // dtor
        ~Toroid();


        void setParameters(std::vector<double> inputPoints);

        std::vector<double> getParams() const;

    private:
        // Quadric parameters; will be turned into 4x4 matrix
        std::vector<double> m_parameters;


    };
}