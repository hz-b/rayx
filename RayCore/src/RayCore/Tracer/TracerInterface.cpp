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

    void TracerInterface::generateRays(VulkanTracer* tracer, LightSource* source) {
        //only one Source for now
        if (!tracer) return;
        if (!source) return;
        std::vector<RAY::Ray> rays = (*source).getRays();
        (*tracer).addRayVector(rays.data(), rays.size());
    }

    // ! parameters are temporary and need to be removed again
    bool TracerInterface::run(double translationXerror, double translationYerror, double translationZerror)
    {

        const clock_t all_begin_time = clock();
        //create tracer instance
        VulkanTracer tracer;
        // readFromFile("../../io/input.csv", RayType);


        //add source to tracer
        //initialize matrix light source with default params
        int beamlinesSimultaneously = 1;
        int number_of_rays = 1 << 17;
        MatrixSource m = MatrixSource(0, "Matrix20", number_of_rays, 0.065, 0.04, 0.0, 0.001, 0.001, { 0,0,0,0 }); // source misalignment only 4 dimensional (x,y, phi, psi)

        addLightSource(&m);
        generateRays(&tracer, m_LightSources[0]);

        std::cout << "add rays to tracer done" << std::endl;



        std::cout.precision(17);

        PointSource ptSource(0, "Point Source", 2000000, 0.005, 0.005, 0, 20, 60, 0, 0, 0, 0, { 0,0,0,0 });

        ReflectionZonePlate reflZonePlate("Reflection Zoneplate", 1, ReflectionZonePlate::CURVATURE_TYPE::CT_PLANE, 4, 60, 170, 2.2, 0, 90, 640, 640, -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 0, { translationXerror,translationYerror,translationZerror,0,0,0 });

        m_Beamline.addQuadric(reflZonePlate.getName(), reflZonePlate.getAnchorPoints(), reflZonePlate.getInMatrix(), reflZonePlate.getOutMatrix(), reflZonePlate.getTempMisalignmentMatrix(), reflZonePlate.getInverseTempMisalignmentMatrix(), reflZonePlate.getParameters());


        //add beamline to tracer
        std::vector<RAY::Quadric> Quadrics = m_Beamline.getObjects();
        tracer.setBeamlineParameters(beamlinesSimultaneously, Quadrics.size(), number_of_rays * beamlinesSimultaneously);
        for (int j = 0; j < beamlinesSimultaneously; j++) {
            for (uint32_t i = 0; i < Quadrics.size(); i++) {
                for (int k = 0; k < 16; k++) {
                    std::cout << Quadrics[i].getAnchorPoints()[k] << ", ";
                    if (k % 4 == 3) std::cout << std::endl;
                }
                tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix(), Quadrics[i].getTempMisalignmentMatrix(), Quadrics[i].getInverseTempMisalignmentMatrix(), Quadrics[i].getParameters());
            }
        }
        const clock_t begin_time = clock();
        tracer.run(); //run tracer
        std::cout << "tracer run time: " << float(clock() - begin_time) << " ms" << std::endl;

        std::cout << "run succeeded" << std::endl;

        std::cout << "tracerInterface run without output: " << float(clock() - all_begin_time) << " ms" << std::endl;

        //get rays from tracer
        auto outputRayIterator = tracer.getOutputIteratorBegin();
        // transform in to usable data
        auto doubleVecSize = RAY_MAX_ELEMENTS_IN_VECTOR * 8;
        std::vector<double> doubleVec(doubleVecSize);
        bool append = false;
        for (; outputRayIterator != tracer.getOutputIteratorEnd(); outputRayIterator++) {
            // std::cout << "ray 16384 xpos: " << (*outputRayIterator)[16384].getxPos() << std::endl;
            // std::cout << "ray 16383 xpos: " << (*outputRayIterator)[16383].getxPos() << std::endl;
            // std::cout << "ray 16385 xpos: " << (*outputRayIterator)[16385].getxPos() << std::endl;
            // std::cout << "ray 16386 xpos: " << (*outputRayIterator)[16386].getxPos() << std::endl;
            std::cout << "(*outputRayIterator).size(): " << (*outputRayIterator).size() << std::endl;
            memcpy(doubleVec.data(), (*outputRayIterator).data(), (*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double));
            doubleVec.resize((*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT);
            writeToFile(doubleVec, append);
            append = true;
        }
        std::cout << "tracer run incl load rays time: " << float(clock() - begin_time) << " ms" << std::endl;


        std::cout << std::endl;
        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        /*while (true) {
            int i = 1;
        }*/
        return true;
    }

    //writes rays to file
    void TracerInterface::writeToFile(std::list<double> outputRays) const
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
    void TracerInterface::writeToFile(std::vector<double> outputRays, bool append) const
    {
        std::cout << "writing " << outputRays.size() / 8 << " rays to file..." << std::endl;
        std::ofstream outputFile;
        outputFile.precision(17);
        std::cout.precision(17);
        std::string filename = "output.csv";
        if (append) {
            outputFile.open(filename, std::ios::app);
        }
        else {
            outputFile.open(filename);
        }
        char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
        outputFile /*<< "Index" << sep*/ << "Xloc" << sep << "Yloc" << sep << "Zloc" << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep << "Zdir" << sep << "Placeholder" << std::endl;
        // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;

        size_t counter = 0;
        int print = 1;
        for (int i = 0; i < outputRays.size(); i++) {
            outputFile << outputRays[i] << ";";
            //printf("Ray position: %2.6f;%2.6f;%2.6f;%2.6f;%2.6f;%2.6f;%2.6f; \n", outputRays[i++], outputRays[i++], outputRays[i++], outputRays[i++], outputRays[i++], outputRays[i++], outputRays[i++]);
            if ((i + 1) % 8 == 0) outputFile << std::endl;
        }

        std::cout << "done!" << std::endl;
    }

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
