#include "Debug.h"
#include "TracerInterface.h"
#include "VulkanTracer.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
        for (int i = 0; i < 1048576/16; i++)
        {
            m_RayList.emplace_back(new Ray(glm::vec3(0, 20, -20), glm::vec3(0, -1, 1), 1));
            m_RayList.emplace_back(new Ray(glm::vec3(10, 5, 10), glm::vec3(-1, 1, 1), 1));
        }
        DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    bool TracerInterface::run()
    {
        //create tracer instance
        VulkanTracer tracer;

        //add rays to tracer
        for (int i = 0; i < m_RayList.size(); i++)
        {
            tracer.addRay(m_RayList[i]->m_position.x, m_RayList[i]->m_position.y, m_RayList[i]->m_position.z, m_RayList[i]->m_direction.x, m_RayList[i]->m_direction.y, m_RayList[i]->m_direction.z, 1);
        }

        //fill beamline (this is a placeholder)
        std::vector<double> beamlineObjectPlaceholder(16, 1);
        for(int i=0; i<2; i++){
            m_Beamline.addBeamlineObject(*(new BeamLineObject(beamlineObjectPlaceholder, beamlineObjectPlaceholder, beamlineObjectPlaceholder)));
        }

        //add beamline to tracer
        auto beamLineObjects = m_Beamline.getObjects();
        for(int i = 0; i<beamLineObjects.size(); i++){
            tracer.addBeamLineObject(beamLineObjects[i].getAnchorPoints(), beamLineObjects[i].getInMatrix(), beamLineObjects[i].getOutMatrix());   
        }

        //run tracer
        tracer.run();
        std::cout << "run succeeded" << std::endl;

        //get rays from tracer
        std::vector<double> outputRays = tracer.getRays();
        for (int i = 0; i < 32; i++)
        {
            std::cout << "output: " << outputRays[i] << std::endl;
        }
        std::cout << "read data succeeded" << std::endl;

        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        return true;
    }
} // namespace RAY
