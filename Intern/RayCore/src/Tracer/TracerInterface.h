#pragma once

#include "Model/Beamline/Beamline.h"
#include "Core.h"
#include "VulkanTracer.h"

#include <string>
#include <vector>
#include <set>
#include <memory>

namespace RAYX
{
    class Ray;

    class RAYX_API TracerInterface
    {
    public:
        TracerInterface();
        TracerInterface(int numElements, int numRays);
        ~TracerInterface();
        void generateRays(std::shared_ptr<LightSource> source);
        void setBeamlineParameters();
        void addOpticalElementToTracer(std::shared_ptr<OpticalElement> element);
        void writeToFile(const std::vector<double>& outputRays, std::ofstream& file, int index) const;


        bool run();

        enum DATA_TYPE { DT_RAY_TYPE, DT_QUADRIC_TYPE };

    private:
        int m_numElements;
        int m_numRays;
        // TODO(Jannis): should be generic Tracer for interchangeability of tracers
        VulkanTracer m_RayTracer;
    };


} // namespace RAYX