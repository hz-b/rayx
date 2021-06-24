#pragma once

#include "Beamline/Beamline.h"
#include "Core.h"

#include <string>
#include <vector>
#include <set>

class VulkanTracer;

namespace RAY
{
    class Ray;

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
        void generateRays(VulkanTracer* tracer, LightSource* source);
        void writeToFile(const std::vector<double>& outputRays, std::ofstream& file, int index) const;


        bool run(double translationXerror, double translationYerror, double translationZerror);
    private:
        std::vector<LightSource*> m_LightSources;
        Beamline& m_Beamline;
    };
} // namespace RAY