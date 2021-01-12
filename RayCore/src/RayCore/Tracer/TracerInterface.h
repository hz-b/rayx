#pragma once

#include "Beamline/Beamline.h"
#include "Beamline/MatrixSource.h"
#include "Core.h"
#include "Ray.h"

#include <vector>

namespace RAY
{
    class RAY_API TracerInterface
    {
    public:
        TracerInterface();
        ~TracerInterface();
        void addLightSource(LightSource* newSource);
        void generateRays();

        bool run();

    private:
        std::vector<LightSource *> m_LightSources;
        Beamline m_Beamline;
        std::vector<Ray *> m_RayList;
    };
} // namespace RAY