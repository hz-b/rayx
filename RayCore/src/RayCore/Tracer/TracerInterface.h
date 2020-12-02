#pragma once

#include "BeamLine/Beamline.h"

#include "Core.h"
#include "Ray.h"

#include <vector>

namespace RAY
{
    class TracerInterface
    {
    public:
        TracerInterface();
        ~TracerInterface();

        bool run();
        
    private:
        Beamline m_Beamline;
        std::vector<Ray*> m_RayList;
         
    };
} // namespace RAY