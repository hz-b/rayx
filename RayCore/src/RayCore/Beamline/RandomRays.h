#pragma once

#include <list>
#include "LightSource.h"

namespace RAY
{

    class RAY_API RandomRays : public LightSource
    {
    public:
        
        RandomRays(int n);
        
        RandomRays();
        ~RandomRays();

        std::vector<Ray> getRays();
        void compareRays(std::vector<Ray *> input, std::vector<double> output);
        
    private:
        
    };

} // namespace RAY