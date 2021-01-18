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
        MatrixSource m = MatrixSource(0, "Matrix source 1", 20, 0.065, 0.04, 0.0, 0.001, 0.001);
        std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        addLightSource(&m);
        generateRays();

        //add rays to tracer
        for (int i = 0; i < m_RayList.size(); i++)
        {
            tracer.addRay(m_RayList[i]->m_position.x, m_RayList[i]->m_position.y, m_RayList[i]->m_position.z, m_RayList[i]->m_direction.x, m_RayList[i]->m_direction.y, m_RayList[i]->m_direction.z, 1);
        }

        // simple plane
        std::vector<double> plane{0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        // this defines the sphere that was previously hardcoded in the shader. 
        std::vector<double> sphere{1,0,0,0, 0,1,0,-3, 0,0,1,0, 0,0,0,0};
        // Quadric b = Quadric(sphere, 0, 0, 0, 10000);
        Quadric b = Quadric(plane, 10, 0, 10, 10000);
        
        for(int i=0; i<1; i++){
            m_Beamline.addQuadric(b.getAnchorPoints(), b.getInMatrix(), b.getOutMatrix());
            // m_Beamline.addQuadric(QuadricPlaceholder, QuadricPlaceholder, QuadricPlaceholder);
        }

        //add beamline to tracer
        auto Quadrics = m_Beamline.getObjects();
        for(int i = 0; i<Quadrics.size(); i++){
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
        outputFile.precision(16);
        outputFile.open("../../output/output.csv");
        outputFile << "Index,X,Y,Z,Weight" << std::endl;
        for (int i=0; i<outputRays.size(); i+=4){
            outputFile << i/4 << "," << outputRays[i] << "," << outputRays[i+1] << "," << outputRays[i+2] << "," << outputRays[i+3] << std::endl;

        }
        outputFile.close();
    }
} // namespace RAY
