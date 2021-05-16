#include "Debug.h"
#include "TracerInterface.h"
#include "VulkanTracer.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include <cmath>

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
        m_RayList.push_back((*m_LightSources[0]).getRays());
    }
    /*
    void TracerInterface::addRayToRayList(Ray inputRay){
        size_t lastSetIndex = m_RayList.size()-1;
        if(lastSetIndex == -1){
            std::vector<Ray> newRayVector;
            newRayVector.resize(67108864);
            m_RayList.insert(newRayVector);
        }
        size_t vectorIndex = m_RayList[lastSetIndex].size();
        if(vectorIndex < 67108863){
            m_RayList[lastSetIndex].push_back(inputRay);
        }
        else{
            std::vector<Ray> newRayVector;
            newRayVector.resize(67108864);
            m_RayList.insert(newRayVector);
        }
    }
    */
    bool TracerInterface::run()
    {
        //create tracer instance
        VulkanTracer tracer;
        // readFromFile("../../io/input.csv", RayType);


        //add source to tracer
        //initialize matrix light source with default params
        
        //RandomRays m = RandomRays(1000000); // produces random values for position, direction and weight to test cosinus and atan implementation
        int number_of_rays = 20000;
        //PointSource p = PointSource(0, "name", number_of_rays, 10.0, 20, 10, 0.01, 0.01, 1, 1, 1, 1);
        MatrixSource m = MatrixSource(0, "Matrix20", number_of_rays, 0.065, 0.04, 0.0, 0.001, 0.001, {0,0,0,0});
        //PointSource m = PointSource(0, "Point source 1", number_of_rays, 0.065, 0.04, 1.0, 0.001, 0.001, 0, 0, 0, 0, {0,0,0,0});
        //std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        addLightSource(&m);
        generateRays();
        

        //add rays to tracer
        std::cout<<"start add rays to tracer"<<std::endl;
        for (auto i = m_RayList.begin(); i != m_RayList.end(); i++)
        {
            std::cout<<"i.size= " << (*i).size()<<std::endl;
            std::cout<<&((*i)[0])<<std::endl;
            tracer.addRayVector(&((*i)[0]), (*i).size());
        }
        std::cout<<"add rays to tracer done"<<std::endl;



        std::cout.precision (17);
        ReflectionZonePlate p1 = ReflectionZonePlate("ReflectionZonePlate1", 1, 0, 50, 200, 170, 1, 10, 1000, 100, 100, -1, -1, 1, 1, 100, 500,100,500, 0, 0, 0, {0,0,0, 0,0,0}, NULL); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        ReflectionZonePlate p2 = ReflectionZonePlate("ReflectionZonePlate2", 1, 0, 50, 200, 170, 3, 20, 2000, 100, 100, -1, -1, 1, 1, 100, 500,100,500, 0, 0, 0, {0,0,0, 0,0,0}, &p1); 
        ReflectionZonePlate p3 = ReflectionZonePlate("ReflectionZonePlate3", 1, 0, 50, 200, 170, 5, 15, 1000, 100, 100, -1, -1, 1, 1, 100, 500,100,500, 0, 0, 0, {0,0,0, 0,0,0}, &p2); 
        ReflectionZonePlate p4 = ReflectionZonePlate("ReflectionZonePlate4", 1, 0, 50, 200, 170, 7, 7, 2000, 100, 100, -1, -1, 1, 1, 100, 500,100,500, 0, 0, 0, {0,0,0, 0,0,0}, &p3);
        // RAY::ReflectionZonePlate p = RAY::ReflectionZonePlate("ReflectionZonePlateMis", 1, 0, 50, 200, 170, 0, 20, 0, 100, 100, -1, -1, 1, 1, 100, 500,100,500, 0, 0, 0, {0,0,0, 0,0,0}); // dx,dy,dz, dpsi,dphi,dchi // 
        // plane mirror with RAY-UI default values
        /*PlaneMirror p1 = PlaneMirror("PlaneMirror1", 50, 200, 10, 7, 10000, {0,0,0, 0,0,0}, NULL); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p2 = PlaneMirror("PlaneMirror2", 50, 200, 15, 4, 10000, {1,2,3, 0.001,0.002,0.003}, &p1); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p3 = PlaneMirror("PlaneMirror3", 50, 200, 7, 10, 10000, {0,0,0, 0,0,0}, &p2); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p4 = PlaneMirror("PlaneMirror4", 50, 200, 22, 17, 10000, {0,0,0, 0,0,0}, &p3); // {1,2,3,0.01,0.02,0.03}
        */

        p4.setOutMatrix({1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}); // for testing, compare resulting rays in object coordinate system of last object
         
        for(int i=0; i<1; i++){
            //m_Beamline.addQuadric(s.getName(), s.getAnchorPoints(), s.getInMatrix(), s.getOutMatrix(), s.getTempMisalignmentMatrix(), s.getInverseTempMisalignmentMatrix(), p.getParameters());
            //m_Beamline.addQuadric(plM.getName(), plM.getAnchorPoints(), plM.getInMatrix(), plM.getOutMatrix(), plM.getTempMisalignmentMatrix(), plM.getInverseTempMisalignmentMatrix(), p.getParameters());
            m_Beamline.addQuadric(p1);
            m_Beamline.addQuadric(p2);
            m_Beamline.addQuadric(p3);
            m_Beamline.addQuadric(p4);
        }

        //add beamline to tracer
        std::vector<RAY::Quadric> Quadrics = m_Beamline.getObjects();
        for(int i = 0; i<int(Quadrics.size()); i++){
            tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix(), Quadrics[i].getTempMisalignmentMatrix(), Quadrics[i].getInverseTempMisalignmentMatrix(), Quadrics[i].getParameters());//, Quadrics[i].getInverseMisalignmentMatrix()
        }

        const clock_t begin_time = clock();
        tracer.run(); //run tracer
        std::cout << "tracer run time: " << float( clock () - begin_time ) << " ms" << std::endl;
        
        std::cout << "run succeeded" << std::endl;

        //get rays from tracer
        std::list<double> outputRays = tracer.getRays();
        
        std::cout << "tracer run incl load rays time: " << float( clock () - begin_time ) << " ms" << std::endl;
        
        // m.compareRays(m_RayList, outputRays); // for comparing accuracy of cos and atan approximation with "source" RandomRays
        std::cout << "read data succeeded" << std::endl;
        std::cout << outputRays.size() << std::endl;
        writeToFile(outputRays);
        //for(auto iter = outputRays.begin(); iter != outputRays.end(); ++iter){
            //std::cout << *iter << ", ";
        //}
        std::cout << std::endl;
        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        return true;
    }

    //writes rays to file
    void TracerInterface::writeToFile(std::list<double> outputRays)
    {
        std::cout << "writing to file..." << std::endl;
        std::ofstream outputFile;
        outputFile.precision(17);
        std::cout.precision (17);
        std::string filename = "../../output/output.csv";
        outputFile.open(filename);
        char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
        outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep<<"Zloc"<<sep<<"Weight"<<sep<<"Xdir"<<sep<<"Ydir"<<sep<<"Zdir" << std::endl;
        // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;
        
        size_t counter = 0;
        int print = 0;
        for (std::list<double>::iterator i=outputRays.begin(); i != outputRays.end(); i++){
            if(counter%8 == 0){
                outputFile << counter/VULKANTRACER_RAY_DOUBLE_AMOUNT;
                if(print==1) std::cout << ")" << std::endl;
                if(print==1) std::cout << "(";
            }
            if(counter%8 == 7){
                outputFile << std::endl;
                counter++;
                continue;
            }
            outputFile << sep << *i ;
            if(counter%8 == 3) {
                if(print==1) std::cout << ") ";
            }else if(counter%8 == 4) {
                if(print==1) std::cout << " (";
            }else if(counter %8 != 0){
                if(print==1) std::cout <<", ";
            }
            if(print==1) std::cout << *i;
            counter++;
        }
        if(print==1) std::cout << ")" << std::endl;
        outputFile.close();
        std::cout << "done!" << std::endl;
    }
    //reads from file. datatype (RayType, QuadricType) needs to be set
    //pretty ugly, should be rewritten later
    void TracerInterface::readFromFile(std::string path, m_dataType dataType)
    {
        std::ifstream inputFile;
        inputFile.open(path);
        switch(dataType){
            case TracerInterface::RayType: {
                std::vector<Ray> newRayVector;
                newRayVector.resize(1048576);
                std::string line;
                std::getline(inputFile, line);
                //std::cout<<line<<std::endl;
                uint32_t numberOfRays = 1;
                while(!inputFile.eof()){
                    std::getline(inputFile, line);
                    if(line[0] == '\0'){
                        break;
                    }
                    int i=0;
                    char currentNumber[32];
                    Ray newRay(glm::dvec3(0,0,0),glm::dvec3(0,0,0),0);
                    std::vector<double> newDoubles;
                    for(int k=0; k<VULKANTRACER_RAY_DOUBLE_AMOUNT; k++){
                        int j=0;
                        while((line[i] != ',') && (line[i] != '\0')){
                            currentNumber[j] = line[i];
                            j++;
                            i++;
                        }
                        i++;
                        currentNumber[j] = '\0';
                        newDoubles.emplace_back(std::stof(currentNumber));
                    }
                    newRay.m_position.x = newDoubles[1];
                    newRay.m_position.y = newDoubles[2];
                    newRay.m_position.z = newDoubles[3];
                    newRay.m_weight = newDoubles[4];
                    newRay.m_direction.x = newDoubles[5];
                    newRay.m_direction.y = newDoubles[6];
                    newRay.m_direction.z = newDoubles[7];
                    //std::cout<<newDoubles[0]<<newDoubles[1]<<newDoubles[2]<<newDoubles[3]<<newDoubles[4]<<newDoubles[5]<<newRay.m_direction.y<<newRay.m_direction.z<<"test"<<std::endl;
                    newRayVector.push_back(newRay);
                    numberOfRays++;
                    if(numberOfRays>1048576){
                        m_RayList.push_back(newRayVector);
                        numberOfRays = 0;
                        newRayVector.clear();
                        newRayVector.resize(1048576);
                    }

                }
            }
            /*case TracerInterface::QuadricType:{
                int i=1;
            }*/
        }

    } 

}    
    // namespace RAY
