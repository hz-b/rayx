#include "TracerInterface.h"
#include "VulkanTracer.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
        m_Beamline = Beamline();
        Ray* ray1 = new Ray(glm::vec3(0,-1,0), glm::vec3(1,1,1), 1);
        m_RayList.push_back(ray1);
    }

    TracerInterface::~TracerInterface()
    {
    }

    bool TracerInterface::run()
    {
        VulkanTracer tracer;
        for(int i=0; i<4; i++){
            tracer.addRay(0,m_RayList[0]->m_position.y,m_RayList[0]->m_position.z,m_RayList[0]->m_direction.x,m_RayList[0]->m_direction.y,m_RayList[0]->m_direction.z, 1);
        }
        tracer.run();
	    std::cout << "run succeeded" << std::endl;
        std::vector<double> outputRays = tracer.getRays();
        for(int i = 0; i<outputRays.size(); i++){
            std::cout << "outputRay["<<i<<"]: "  << outputRays[i] << std::endl;

        }
	    tracer.cleanup();
        return true;
    }
} // namespace RAY
