#include "Debug.h"
#include "TracerInterface.h"
#include "VulkanTracer.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
        /*for (int i = 0; i < 64; i++)
        {
            m_RayList.emplace_back(new Ray(glm::vec3(0, 20, -20), glm::vec3(0, -1, 1), 1));
            // m_RayList.emplace_back(new Ray(glm::vec3(10, 5, 10), glm::vec3(-1, 1, 1), 1));
        }*/
        std::cout.precision(15); // show 16 decimals
        MatrixSource m = MatrixSource(0, "Matrix source 1", 20, 0.65, 0.4, 0.0, 0.01, 0.02);
        
        std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;
        m_RayList = m.getRays();
        
        std::cout.precision(15); // show 16 decimals
        // same rays as with old RAY-UI
        for(int i = 0; i<m.getNumberOfRays(); i++) {
            std::cout << "weight: " << m_RayList[i]->m_weight << " pos: (" << m_RayList[i]->m_position[0] << "," << m_RayList[i]->m_position[1] << "," << m_RayList[i]->m_position[2] << ") dir: (" << m_RayList[i]->m_direction[0] << "," << m_RayList[i]->m_direction[1] << "," << m_RayList[i]->m_direction[2]  << ")" << std::endl;
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
        // this defines the sphere that was previously hardcoded in the shader. 
        std::vector<double> sphere{1,0,0,0, 0,1,0,-3, 0,0,1,0, 0,0,0,0};
        BeamLineObject b = BeamLineObject(sphere, 10, 5, 10, 0);
        
        for(int i=0; i<1; i++){
            m_Beamline.addBeamlineObject(b.getAnchorPoints(), b.getInMatrix(), b.getOutMatrix());
            // m_Beamline.addBeamlineObject(beamlineObjectPlaceholder, beamlineObjectPlaceholder, beamlineObjectPlaceholder);
        }

        //add beamline to tracer
        auto beamLineObjects = m_Beamline.getObjects();
        for(int i = 0; i<beamLineObjects.size(); i++){
            //for(int j=0; j<beamLineObjects[i].getAnchorPoints().size(); j++)
            //    std::cout << beamLineObjects[i].getAnchorPoints()[j] << std::endl;
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
