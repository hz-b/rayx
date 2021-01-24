#pragma once

#include "Beamline/Beamline.h"
#include "Beamline/MatrixSource.h"
#include "Core.h"
#include "Ray.h"

#include <string>
#include <vector>

namespace RAY
{
    class RAY_API TracerInterface
    {
    public:
        enum m_dataType {RayType, QuadricType};

        TracerInterface();
        ~TracerInterface();
        void addLightSource(LightSource* newSource);
        void generateRays();
        void writeToFile(std::vector<double> outputRays);
        void readFromFile(std::string path, m_dataType dataType, void* data);

        bool run();
    private:
        std::vector<LightSource *> m_LightSources;
        Beamline m_Beamline;
        std::vector<Ray *> m_RayList;
    };
} // namespace RAY