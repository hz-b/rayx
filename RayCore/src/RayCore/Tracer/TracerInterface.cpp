#include "Debug.h"
#include "TracerInterface.h"
#include "VulkanTracer.h"
#include <fstream>
#include <chrono>
#include <sstream>

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
        int number_of_rays = 25;
        MatrixSource m = MatrixSource(0, "Matrix source 1", number_of_rays, 0.065, 0.04, 0.0, 0.001, 0.001);
        //PointSource m = PointSource(0, "Point source 1", number_of_rays, 0.065, 0.04, 1.0, 0.001, 0.001);
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
        // plane mirror with RAY-UI default values
        PlaneMirror plM = PlaneMirror(50, 200, 10, 0, 10000, {0,0,0,0,0,0}); // {1,2,3,0.01,0.02,0.03}
        // plane grating with default values
        PlaneGrating plG = PlaneGrating(50, 200, 10, 0.0, 0.0, 10000, 100, 1000, 1, 2, {1,2,3,0.001,0.002,0.003}); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        // spherical grating with defalult values. SphereGrating(int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams);
        SphereGrating s = SphereGrating(0, 50, 200, 10, 0.0, 0.0, 10000, 10000, 1000,  100, 1000, 1, {0,0,0,0,0,0});
        // std::cout << s.getRadius() << std::endl;
        // SphereMirror sM = SphereMirror(50, 200, 10, 0.0, 10000, 10000, 1000, {0,0,0,0,0,0});
        
        for(int i=0; i<1; i++){
            //m_Beamline.addQuadric(s.getAnchorPoints(), s.getInMatrix(), s.getOutMatrix(), s.getMisalignmentMatrix(), s.getInverseMisalignmentMatrix());
            m_Beamline.addQuadric(plM.getAnchorPoints(), plM.getInMatrix(), plM.getOutMatrix(), plM.getMisalignmentMatrix(), plM.getInverseMisalignmentMatrix());
            //m_Beamline.addQuadric(plG.getAnchorPoints(), plG.getInMatrix(), plG.getOutMatrix(), plG.getMisalignmentMatrix(), plG.getInverseMisalignmentMatrix());
        }

        //add beamline to tracer
        std::vector<RAY::Quadric> Quadrics = m_Beamline.getObjects();
        for(int i = 0; i<Quadrics.size(); i++){
            tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix(), Quadrics[i].getMisalignmentMatrix(), Quadrics[i].getInverseMisalignmentMatrix());//, Quadrics[i].getInverseMisalignmentMatrix()
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
        outputFile.open("../../io/output.csv");
        char sep = ','; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
        outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep<<"Zloc"<<sep<<"Weight"<<sep<<"Xdir"<<sep<<"Ydir"<<sep<<"Zdir" << std::endl;
        // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;
        size_t counter = 0;
        for (std::list<double>::iterator i=outputRays.begin(); i != outputRays.end(); i++){
            if(counter%8 == 0){
                outputFile << counter/VULKANTRACER_RAY_DOUBLE_AMOUNT;
            }
            outputFile << sep << *i ;
            if(counter%8 == 7){
                outputFile << std::endl;
            }
            counter++;
            std::cout << *i<< std::endl;
        }
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
                uint32_t numberOfRays = 0;
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
            case TracerInterface::QuadricType:{
                int i=1;
            }
        }

    } 

}    
    // namespace RAY
