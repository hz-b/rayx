#include "TracerInterface.h"
#include "VulkanTracer.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
        m_Beamline = Beamline();
        for(int i=0; i<1048576; i++){
            m_RayList.emplace_back(new Ray(glm::vec3(0,20,-20), glm::vec3(0,-1,1), 1));
            m_RayList.emplace_back(new Ray(glm::vec3(10,5,10), glm::vec3(-1,1,1), 1));
        }
        std::cout << "Creating TracerInterface..." << std::endl;
    }

    TracerInterface::~TracerInterface()
    {
        std::cout << "Deleting TracerInterface..." << std::endl;
    }

    bool TracerInterface::run()
    {
        VulkanTracer tracer;
        for(int i=0; i<m_RayList.size(); i++){
            tracer.addRay(m_RayList[i]->m_position.x,m_RayList[i]->m_position.y,m_RayList[i]->m_position.z,m_RayList[i]->m_direction.x,m_RayList[i]->m_direction.y,m_RayList[i]->m_direction.z, 1);
        }
        std::vector<double> beamlineObjectPlaceholder(16, 1);
        m_Beamline.addBeamlineObject(*(new BeamLineObject(beamlineObjectPlaceholder, beamlineObjectPlaceholder, beamlineObjectPlaceholder)));
        auto beamLineObjects = m_Beamline.getObjects();
        for(auto object = beamLineObjects.begin(); object != beamLineObjects.end(); object++){
            tracer.addBeamLineObject((*object).getData());   
        }
        tracer.run();
	    std::cout << "run succeeded" << std::endl;
        std::vector<double> outputRays = tracer.getRays();
        for(int i=0; i<32; i++){
            std::cout << "output: "<<outputRays[i] << std::endl;
        }
	    std::cout << "read data succeeded" << std::endl;
	    tracer.cleanup();
        return true;
    }
} // namespace RAY
