#pragma once

#include "Core.h"
#include "Surface.h"
#include "utils.h"

#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;

namespace RAY {
    class RAY_API Quadric : public Surface {
    public:
        Quadric(const std::vector<double> inputPoints);
        
        
        void setAnchorPoints(std::vector<double> inputPoints);

        std::vector<double> getParams() const;

        Quadric();
        ~Quadric();
    private:
        
        std::vector<double> m_anchorPoints;
        

    };
}