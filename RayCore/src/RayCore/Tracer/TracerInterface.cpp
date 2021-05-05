#include "Beamline/MatrixSource.h"
#include "Beamline/PointSource.h"
#include "Beamline/ReflectionZonePlate.h"
#include "Debug.h"
#include "TracerInterface.h"

#include <fstream>
#include <chrono>
#include <sstream>
#include <cmath>

namespace RAY
{
    TracerInterface::TracerInterface() :
        m_Beamline(Beamline::get())
    {

        DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    void TracerInterface::addLightSource(LightSource* newSource) {
        m_LightSources.push_back(newSource);
    }

    void TracerInterface::generateRays(VulkanTracer tracer, LightSource* source) {
        //only one Source for now
        std::vector<RAY::Ray> rays = (*source).getRays();
        tracer.addRayVector(&rays, rays.size());
    }
    bool TracerInterface::run()
    {

        const clock_t all_begin_time = clock();
        //create tracer instance
        VulkanTracer tracer;
        // readFromFile("../../io/input.csv", RayType);


        //add source to tracer
        //initialize matrix light source with default params
        int beamlinesSimultaneously = 1;
        //RandomRays m = RandomRays(1000000); // produces random values for position, direction and weight to test cosinus and atan implementation
        int number_of_rays = 1 << 17;
        MatrixSource m = MatrixSource(0, "Matrix20", number_of_rays, 0.065, 0.04, 0.0, 0.001, 0.001, { 0,0,0,0,0,0 });
        //PointSource m = PointSource(0, "Point source 1", number_of_rays, 0.065, 0.04, 1.0, 0.001, 0.001);
        //std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        addLightSource(&m);
        generateRays(tracer, m_LightSources[0]);

        std::cout << "add rays to tracer done" << std::endl;



        std::cout.precision(17);

        PointSource ptSource(0, "Point Source", 2000000, 0.005, 0.005, 0, 20, 60, 0, 0, 0, 0, { 0,0,0,0,0,0 }); // TODO: widthlength, heighlength, horLength, verLength

        ReflectionZonePlate reflZonePlate("Reflection Zoneplate", 1, ReflectionZonePlate::CURVATURE_TYPE::CT_PLANE, 4, 60, 170, 2.2, 0, 90, 640, 640, -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 0, { 0,0,0,0,0,0 });

        m_Beamline.addQuadric(reflZonePlate.getName(), reflZonePlate.getAnchorPoints(), reflZonePlate.getInMatrix(), reflZonePlate.getOutMatrix(), reflZonePlate.getTempMisalignmentMatrix(), reflZonePlate.getInverseTempMisalignmentMatrix(), { 0,0,0,0,0,0 });


        // plane mirror with RAY-UI default values
        //PlaneMirror plM = PlaneMirror("PlaneMirrorMis",50, 200, 10, 0, 10000, {1,2,3,0.001,0.002,0.003}); // {1,2,3,0.01,0.02,0.03}
        // plane grating with default values
        PlaneGrating plG = PlaneGrating("PlaneGratingDeviationDefault", 1, 50, 200, 0, 10, 7.5, 10000, 100, 1000, 1, 2, { 1,2,3,0.001,0.002,0.003 }, { 0,0,0,0,0,0 }); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        // spherical grating with defalult values. SphereGrating(int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams);
        //SphereGrating s = SphereGrating("SphereGrating", 0, 50, 200, 10, 0.0, 0.0, 10000, 10000, 1000,  100, 1000, 1, {0,0,0,0,0,0});
        // std::cout << s.getRadius() << std::endl;
        // SphereMirror s = SphereMirror("SphereMirror", 50, 200, 10, 0.0, 10000, 10000, 1000, {0,0,0,0,0,0});

        //for (int i = 0; i < 1; i++) {
            //m_Beamline.addQuadric(s.getName(), s.getAnchorPoints(), s.getInMatrix(), s.getOutMatrix(), s.getTempMisalignmentMatrix(), s.getInverseTempMisalignmentMatrix());
            //m_Beamline.addQuadric(plM.getName(), plM.getAnchorPoints(), plM.getInMatrix(), plM.getOutMatrix(), plM.getTempMisalignmentMatrix(), plM.getInverseTempMisalignmentMatrix());
        //    m_Beamline.addQuadric(plG.getName(), plG.getAnchorPoints(), plG.getInMatrix(), plG.getOutMatrix(), plG.getTempMisalignmentMatrix(), plG.getInverseTempMisalignmentMatrix());
        //}
        //add beamline to tracer
        std::vector<RAY::Quadric> Quadrics = m_Beamline.getObjects();
        tracer.setBeamlineParameters(beamlinesSimultaneously, Quadrics.size(), number_of_rays * beamlinesSimultaneously);
        for (int j = 0; j < beamlinesSimultaneously; j++) {
            for (uint32_t i = 0; i < Quadrics.size(); i++) {
                tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix(), Quadrics[i].getTempMisalignmentMatrix(), Quadrics[i].getInverseTempMisalignmentMatrix(), Quadrics[i].getParameters());
            }
        }
        const clock_t begin_time = clock();
        tracer.run(); //run tracer
        std::cout << "tracer run time: " << float(clock() - begin_time) << " ms" << std::endl;

        std::cout << "run succeeded" << std::endl;

        std::cout << "tracerInterface run without output: " << float(clock() - all_begin_time) << " ms" << std::endl;

        //get rays from tracer
        auto outputRayIterator = tracer.getOutputIterator();
        std::cout << "tracer run incl load rays time: " << float(clock() - begin_time) << " ms" << std::endl;

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
        std::cout.precision(17);
        std::string filename = "../../output/output.csv";
        outputFile.open(filename);
        char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
        outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep << "Zloc" << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep << "Zdir" << std::endl;
        // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;

        size_t counter = 0;
        int print = 1;
        for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end(); i++) {
            if (counter % 8 == 0) {
                outputFile << counter / VULKANTRACER_RAY_DOUBLE_AMOUNT;
                if (print == 1) std::cout << ")" << std::endl;
                if (print == 1) std::cout << "(";
            }
            if (counter % 8 == 7) {
                outputFile << std::endl;
                counter++;
                continue;
            }
            outputFile << sep << *i;
            if (counter % 8 == 3) {
                if (print == 1) std::cout << ") ";
            }
            else if (counter % 8 == 4) {
                if (print == 1) std::cout << " (";
            }
            else if (counter % 8 != 0) {
                if (print == 1) std::cout << ", ";
            }
            if (print == 1) std::cout << *i;
            counter++;
        }
        if (print == 1) std::cout << ")" << std::endl;
        outputFile.close();
        std::cout << "done!" << std::endl;
    }

    //writes rays to file
    // void TracerInterface::writeToFile(std::vector<Ray> outputRays)
    // {
    //     std::cout << "writing to file..." << std::endl;
    //     std::ofstream outputFile;
    //     outputFile.precision(17);
    //     std::cout.precision(17);
    //     std::string filename = "../../output/output.csv";
    //     outputFile.open(filename);
    //     char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
    //     outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep << "Zloc" << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep << "Zdir" << std::endl;
    //     // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;

    //     size_t counter = 0;
    //     int print = 1;
    //     for (Ray ray : outputRays) {

    //         auto pos = ray.position;
    //         auto weight = ray.weight;
    //         auto dir = ray.direction;

    //         printf("Ray position: %d%d%d, weight: %d, dir: %d%d%d \n", pos.x, pos.y, pos.z, weight, dir.x, dir.y, dir.z);

    //     }
    //     std::cout << "done!" << std::endl;
    // }
    //reads from file. datatype (RayType, QuadricType) needs to be set
    //pretty ugly, should be rewritten later
    // void TracerInterface::readFromFile(std::string path, m_dataType dataType)
    // {
    //     std::ifstream inputFile;
    //     inputFile.open(path);
    //     switch (dataType) {
    //     case TracerInterface::RayType: {
    //         std::vector<Ray> newRayVector;
    //         newRayVector.resize(RAY_MAX_ELEMENTS_IN_VECTOR);
    //         std::string line;
    //         std::getline(inputFile, line);
    //         //std::cout<<line<<std::endl;
    //         uint32_t numberOfRays = 0;
    //         while (!inputFile.eof()) {
    //             std::getline(inputFile, line);
    //             if (line[0] == '\0') {
    //                 break;
    //             }
    //             int i = 0;
    //             char currentNumber[32];
    //             Ray newRay(glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0), 0);
    //             std::vector<double> newDoubles;
    //             for (int k = 0; k < VULKANTRACER_RAY_DOUBLE_AMOUNT; k++) {
    //                 int j = 0;
    //                 while ((line[i] != ',') && (line[i] != '\0')) {
    //                     currentNumber[j] = line[i];
    //                     j++;
    //                     i++;
    //                 }
    //                 i++;
    //                 currentNumber[j] = '\0';
    //                 newDoubles.emplace_back(std::stof(currentNumber));
    //             }
    //             newRay.m_position.x = newDoubles[1];
    //             newRay.m_position.y = newDoubles[2];
    //             newRay.m_position.z = newDoubles[3];
    //             newRay.m_weight = newDoubles[4];
    //             newRay.m_direction.x = newDoubles[5];
    //             newRay.m_direction.y = newDoubles[6];
    //             newRay.m_direction.z = newDoubles[7];
    //             //std::cout<<newDoubles[0]<<newDoubles[1]<<newDoubles[2]<<newDoubles[3]<<newDoubles[4]<<newDoubles[5]<<newRay.m_direction.y<<newRay.m_direction.z<<"test"<<std::endl;
    //             newRayVector.push_back(newRay);
    //             numberOfRays++;
    //             if (numberOfRays > RAY_MAX_ELEMENTS_IN_VECTOR) {
    //                 m_RayList.push_back(newRayVector);
    //                 numberOfRays = 0;
    //                 newRayVector.clear();
    //                 newRayVector.resize(RAY_MAX_ELEMENTS_IN_VECTOR);
    //             }

    //         }
    //         break;
    //     }
    //     case TracerInterface::QuadricType: {
    //         //int i = 1;
    //         break;
    //     }
    //     }

    // }

}
// namespace RAY
