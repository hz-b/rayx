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
        void writeToFile(std::vector<double> outputRays);

        bool run();

    private:
        std::vector<LightSource *> m_LightSources;
        Beamline m_Beamline;
        std::vector<Ray *> m_RayList;
    };
} // namespace RAY