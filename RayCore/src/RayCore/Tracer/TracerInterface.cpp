#include "Debug.h"
#include "TracerInterface.h"
#include "VulkanTracer.h"
#include <fstream>

namespace RAY
{
    TracerInterface::TracerInterface()
    {
           
        DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    void TracerInterface::addLightSource(LightSource* newSource){
        m_LightSources.push_back(newSource);
    }
    void TracerInterface::generateRays(){
        //only one Source for now
        m_RayList = (*m_LightSources[0]).getRays();
    }


    bool TracerInterface::run()
    {
        //create tracer instance
        VulkanTracer tracer;

        //add source to tracer
        //initialize matrix light source
        MatrixSource m = MatrixSource(0, "Matrix source 1", 20000, 0.65, 0.4, 0.0, 0.01, 0.02);
        std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        addLightSource(&m);
        generateRays();

        // same rays as with old RAY-UI
        /*
        for(int i = 0; i<m.getNumberOfRays(); i++) {
            std::cout << "weight: " << m_RayList[i]->m_weight << " pos: (" << m_RayList[i]->m_position[0] << "," << m_RayList[i]->m_position[1] << "," << m_RayList[i]->m_position[2] << ") dir: (" << m_RayList[i]->m_direction[0] << "," << m_RayList[i]->m_direction[1] << "," << m_RayList[i]->m_direction[2]  << ")" << std::endl;
        }
        */
        //add rays to tracer
        for (int i = 0; i < m_RayList.size(); i++)
        {
            tracer.addRay(m_RayList[i]->m_position.x, m_RayList[i]->m_position.y, m_RayList[i]->m_position.z, m_RayList[i]->m_direction.x, m_RayList[i]->m_direction.y, m_RayList[i]->m_direction.z, 1);
        }

        //fill beamline (this is a placeholder)
        // this defines the sphere that was previously hardcoded in the shader. 
        std::vector<double> sphere{1,0,0,0, 0,1,0,-3, 0,0,1,0, 0,0,0,0};
        Quadric b = Quadric(sphere, 10, 5, 10, 0);
        
        for(int i=0; i<1; i++){
            m_Beamline.addQuadric(b.getAnchorPoints(), b.getInMatrix(), b.getOutMatrix());
            // m_Beamline.addQuadric(QuadricPlaceholder, QuadricPlaceholder, QuadricPlaceholder);
        }

        //add beamline to tracer
        auto Quadrics = m_Beamline.getObjects();
        for(int i = 0; i<Quadrics.size(); i++){
            //for(int j=0; j<Quadrics[i].getAnchorPoints().size(); j++)
            //    std::cout << Quadrics[i].getAnchorPoints()[j] << std::endl;
            tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix());   
        }

        //run tracer
        tracer.run();
        std::cout << "run succeeded" << std::endl;

        //get rays from tracer
        std::vector<double> outputRays = tracer.getRays();
        std::cout << "read data succeeded" << std::endl;
        std::cout << "writing to file..." << std::endl;
        writeToFile(outputRays);
        std::cout << "done!" << std::endl;

        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        return true;
    }

    void TracerInterface::writeToFile(std::vector<double> outputRays){
        std::ofstream outputFile;
        outputFile.open("../../output/output.csv");
        outputFile << "Index,X,Y,Z,Weight" << std::endl;
        for (int i=0; i<outputRays.size(); i+=4){
            outputFile << i/4 << "," << outputRays[i] << "," << outputRays[i+1] << "," << outputRays[i+2] << "," << outputRays[i+3] << std::endl;

        }
        outputFile.close();
    }
} // namespace RAY
