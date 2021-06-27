#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip> 
#include <sstream>

#include "TracerInterface.h"

#include "Beamline/MatrixSource.h"
#include "Beamline/PointSource.h"
#include "Beamline/ReflectionZonePlate.h"
#include "Debug.h"
#include "Ray.h"
#include "VulkanTracer.h"

#define SHORTOUTPUT false


namespace RAY
{
    TracerInterface::TracerInterface() :
        m_Beamline(Beamline::get())
    {

        RAY_DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        RAY_DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    void TracerInterface::addLightSource(LightSource* newSource) {
        m_LightSources.push_back(newSource);
    }

    void TracerInterface::generateRays(VulkanTracer* tracer, LightSource* source) {
        //only one Source for now
        if (!tracer) return;
        if (!source) return;
        std::vector<RAY::Ray> rays = source->getRays();
        RAY_DEBUG(std::cout << "add rays" << std::endl);
        tracer->addRayVector(rays.data(), rays.size());
    }

    // ! parameters are temporary and need to be removed again
    bool TracerInterface::run(double translationXerror, double translationYerror, double translationZerror)
    {

        const clock_t all_begin_time = clock();

        //create tracer instance
        VulkanTracer tracer;
        // readFromFile("../../io/input.csv", RayType);


        //add source to tracer
        int beamlinesSimultaneously = 1;
        int number_of_rays = 2000000;

        // petes setup
        PointSource p = PointSource(0, "spec1_first_rzp4", number_of_rays, 1, 0.005, 0.005, 0, 0.02, 0.06, 1, 1, 0, 0, 640, 120, { 0,0,0,0 });
        ReflectionZonePlate rzp = ReflectionZonePlate("ReflectionZonePete", 1, 0, 1, 1, 4, 60, 170, 2.2, 0, 90, p.getPhotonEnergy(), p.getPhotonEnergy(), 1, 1, 2.2, 4.75, 90, 400, 90, 400, 0, 0, 1, 0, -24.35, 4.75, { 0,0,0, 0,0,0 }, { 0,0,0,0, 0,0,0 }, nullptr);  // dx,dy,dz, dpsi,dphi,dchi //
        PlaneGrating plG = PlaneGrating("PeteGratingDeviationAzMis", 0, 50, 200, 10, 0.0, 7.5, 10000, 100, 1000, 1, 2, 0, { 0,0,0, 0,0,0 }, { 0,0,0,0,0,0 }, { 0,0,0,0,0, 0,0 }, nullptr); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        ImagePlane ip = ImagePlane("Image Plane", 385, nullptr); // one out of the bunch

        addLightSource(&p);
        for (int j = 0; j < beamlinesSimultaneously; j++) {
            generateRays(&tracer, &p);
        }
        m_Beamline.addQuadric(rzp);
        //m_Beamline.addQuadric(plG);
        m_Beamline.addQuadric(ip);


        // MatrixSource m = MatrixSource(0, "matrix source", number_of_rays, 0, 0.065, 0.04, 0, 0.001, 0.001, 100, 0, { 0,0,0,0 });
        // PlaneMirror p = PlaneMirror("plane Mirror", 50, 200, 10, 0, 10000, { 0,0,0, 0,0,0 }, { 10,10, 0,0,0, 0,0 }, nullptr);
        // Slit s = Slit("slit", 0, 20, 2, 0, 10000, 20, 1, m.getPhotonEnergy(), { 0,0,0, 0,0,0 }, { 0,0, 0,0,0, 0,0 }, nullptr);
        // ImagePlane i = ImagePlane("Image plane", 1000, nullptr);
        //PointSource m = PointSource(0, "Point source 1", number_of_rays, 0.065, 0.04, 1.0, 0.001, 0.001, 0, 0, 0, 0, 100, 10, {0,0,0,0});
        //std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        // addLightSource(&m);
        // generateRays(&tracer, &m);

        RAY_DEBUG(std::cout << "add rays to tracer done" << std::endl);



        //ReflectionZonePlate p1 = ReflectionZonePlate("ReflectionZonePlate1", 1, 0, 50, 200, 170, 1, 10, 1000, 100, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, { 0,0,0, 0,0,0 }, NULL); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        //RAY::ReflectionZonePlate reflZonePlate = ReflectionZonePlate("ReflectionZonePlate", 1, 0, 4, 60, 170, 2.2, 0, 90, 640, 640, -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 0, { 0,0,0,0,0,0 }); // dx,dy,dz, dpsi,dphi,dchi // 
        // plane mirror with RAY-UI default values
        //PlaneMirror p1 = PlaneMirror("PlaneMirror1", 50, 200, 10, 0, 10000, { 0,0,0, 0,0,0 }, nullptr); // {1,2,3,0.01,0.02,0.03}
        /*
        PlaneMirror p2 = PlaneMirror("PlaneMirror2", 50, 200, 15, 4, 10000, {1,2,3, 0.001,0.002,0.003}, &p1); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p3 = PlaneMirror("PlaneMirror3", 50, 200, 7, 10, 10000, {0,0,0, 0,0,0}, &p2); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p4 = PlaneMirror("PlaneMirror4", 50, 200, 22, 17, 10000, {0,0,0, 0,0,0}, &p3); // {1,2,3,0.01,0.02,0.03}
        */

        //m_Beamline.addQuadric(reflZonePlate.getName(), reflZonePlate.getAnchorPoints(), reflZonePlate.getInMatrix(), reflZonePlate.getOutMatrix(), reflZonePlate.getTempMisalignmentMatrix(), reflZonePlate.getInverseTempMisalignmentMatrix(), reflZonePlate.getParameters());
        //m_Beamline.addQuadric(s); //rzp.getName(), rzp.getAnchorPoints(), rzp.getInMatrix(), rzp.getOutMatrix(), rzp.getTempMisalignmentMatrix(), rzp.getInverseTempMisalignmentMatrix(), rzp.getParameters());
        //m_Beamline.addQuadric(i); //ip.getName(), ip.getAnchorPoints(), ip.getInMatrix(), ip.getOutMatrix(), ip.getTempMisalignmentMatrix(), ip.getInverseTempMisalignmentMatrix(), ip.getParameters());
        //add beamline to tracer
        const std::vector<RAY::Quadric>& Quadrics = m_Beamline.getObjects();
        tracer.setBeamlineParameters(beamlinesSimultaneously, Quadrics.size(), number_of_rays);

        for (int j = 0; j < beamlinesSimultaneously; j++) {
            for (int i = 0; i<int(Quadrics.size()); i++) {
                std::cout << "add " << Quadrics[i].getName() << std::endl;
                tracer.addQuadric(Quadrics[i].getAnchorPoints(), Quadrics[i].getInMatrix(), Quadrics[i].getOutMatrix(), Quadrics[i].getTempMisalignmentMatrix(), Quadrics[i].getInverseTempMisalignmentMatrix(), Quadrics[i].getObjectParameters(), Quadrics[i].getElementParameters());//, Quadrics[i].getInverseMisalignmentMatrix()
            }
        }

        const clock_t begin_time = clock();
        tracer.run(); //run tracer
        std::cout << "tracer run time: " << float(clock() - begin_time) << " ms" << std::endl;

        RAY_DEBUG(std::cout << "run succeeded" << std::endl);

        RAY_DEBUG(std::cout << "tracerInterface run without output: " << float(clock() - all_begin_time) << " ms" << std::endl);

        // transform in to usable data
        auto doubleVecSize = RAY_MAX_ELEMENTS_IN_VECTOR * 8;
        std::vector<double> doubleVec(doubleVecSize);
        size_t index = 0;

        // Print ray footprint into file
        std::ofstream outputFile("output.csv");
        outputFile.precision(17);
        if (SHORTOUTPUT)
            outputFile << "Index;Xloc;Yloc\n";
        else
            outputFile << "Index;Xloc;Yloc;Zloc;Weight;Xdir;Ydir;Zdir;Energy\n";

        //get rays from tracer
        for (auto outputRayIterator = tracer.getOutputIteratorBegin(), outputIteratorEnd = tracer.getOutputIteratorEnd();
            outputRayIterator != outputIteratorEnd; outputRayIterator++)
        {
            RAY_DEBUG(std::cout << "(*outputRayIterator).size(): " << (*outputRayIterator).size() << std::endl);

            memcpy(doubleVec.data(), (*outputRayIterator).data(), (*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double));
            doubleVec.resize((*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT);

            RAY_DEBUG(std::cout << "tracerInterface: sample ray: " << doubleVec[0] << ", " << doubleVec[1] << ", " << doubleVec[2] << ", " << doubleVec[3] << ", " << doubleVec[4] << ", " << doubleVec[5] << ", " << doubleVec[6] << ", energy: " << doubleVec[7] << std::endl);

            writeToFile(doubleVec, outputFile, index);
            index = index + (*outputRayIterator).size();
        }
        outputFile.close();

        RAY_DEBUG(std::cout << "tracer run incl load rays time: " << float(clock() - begin_time) << " ms" << std::endl);

        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        //intentionally not RAY_DEBUG()
        std::cout << "all done" << std::endl;
        return true;
    }

    //writes rays to file
    void TracerInterface::writeToFile(const std::vector<double>& outputRays, std::ofstream& file, int index) const
    {
        size_t size = outputRays.size();

        RAY_DEBUG(std::cout << "writing " << outputRays.size() / 8 << " rays to file..." << std::endl);

        if (SHORTOUTPUT) {
            char buff[64];
            for (size_t i = 0; i < size; i = i + 8) { // ! + 8 because of placeholder
                sprintf(buff, "%d;%.17f;%.17f\n", index, outputRays[i], outputRays[i + 1]);
                file << buff;
                index++;
            }
        }
        else {
            char buff[256];
            for (size_t i = 0; i < size; i = i + 8) { // ! + 8 because of placeholder 
                sprintf(buff, "%d;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f\n", index,
                    outputRays[i], outputRays[i + 1], outputRays[i + 2], outputRays[i + 3],
                    outputRays[i + 4], outputRays[i + 5], outputRays[i + 6], outputRays[i + 7]);
                file << buff;
                index++;
            }
        }

        RAY_DEBUG(std::cout << "done!" << std::endl);
    }
}
// namespace RAY
