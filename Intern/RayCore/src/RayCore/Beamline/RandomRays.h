#pragma once
#include <list>
#include "LightSource.h"

namespace RAY
{

    class RAY_API RandomRays : public LightSource
    {
    public:
        
        RandomRays(int n, int low, int high);
        
        RandomRays();
        ~RandomRays();

        std::vector<Ray> getRays();
        void compareRays(std::vector<Ray *> input, std::vector<double> output);
        
    private:
        int m_low;
        int m_high;
    };

} // namespace RAY