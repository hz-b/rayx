#include "TracerInterface.h"
#include "VulkanTracer.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
        m_Beamline = Beamline();
        Ray* ray1 = new Ray(glm::vec3(0,-1,0), glm::vec3(1,1,1));
        m_RayList.push_back(ray1);
    }

    TracerInterface::~TracerInterface()
    {
    }

    bool TracerInterface::run()
    {
        VulkanTracer tracer;
        tracer.run();
        return true;
    }
} // namespace RAY
