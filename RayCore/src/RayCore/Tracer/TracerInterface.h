#pragma once

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Ray.h"

#include <string>
#include <vector>
#include <set>

namespace RAY
{
    class RAY_API TracerInterface
    {
        struct RayVector {
            std::vector<Ray> rayVector;
        };
    public:
        enum m_dataType { RayType, QuadricType };

        TracerInterface();
        ~TracerInterface();
        void addLightSource(LightSource* newSource);
        void generateRays();
        void writeToFile(std::list<double> outputRays);
        //void readFromFile(std::string path, m_dataType dataType);
        //void addRayToRayList(Ray inputRay);
        void addRayVector(void* location);


        bool run();
    private:
        std::vector<LightSource*> m_LightSources;
        Beamline& m_Beamline;
        std::list<std::vector<Ray>> m_RayList;
    };
} // namespace RAY