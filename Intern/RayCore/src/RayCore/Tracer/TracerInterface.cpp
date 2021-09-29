#include <cmath>
#include <fstream>
#include <iomanip> 
#include <sstream>

#include "TracerInterface.h"

#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/Objects/PointSource.h"
#include "Beamline/Objects/ReflectionZonePlate.h"
#include "GeometricUserParams.h"
#include "Debug.h"
#include "Ray.h"
#include "VulkanTracer.h"

#define SHORTOUTPUT false


namespace RAYX
{
    TracerInterface::TracerInterface()
    {
        RAYX_DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        RAYX_DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    /** Adds new light source to light sources.
     *
     *  @param newSource shared pointer to light source to be added
     */
    void TracerInterface::addLightSource(std::shared_ptr<LightSource> newSource) {
        m_LightSources.push_back(newSource);
    }

    /** Generates rays from light source into the tracer.
     *
     *  @param tracer   reference to tracer
     *  @param source   shared pointer to light source used for ray generation
     */
    void TracerInterface::generateRays(VulkanTracer& tracer, std::shared_ptr<LightSource> source) {
        //only one Source for now
        if (!source) return;
        std::vector<RAYX::Ray> rays = source->getRays();
        RAYX_DEBUG(std::cout << "add rays" << std::endl);
        tracer.addRayVector(rays.data(), rays.size());
    }

    void TracerInterface::addOpticalElementToTracer(VulkanTracer& tracer, std::shared_ptr<OpticalElement> element) {
        tracer.addVectors(element->getSurfaceParams(), element->getInMatrix(), element->getOutMatrix(), element->getTempMisalignmentMatrix(), element->getInverseTempMisalignmentMatrix(), element->getObjectParameters(), element->getElementParameters());
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
        int number_of_rays = 20000;

        std::shared_ptr<MatrixSource> m = std::make_shared<MatrixSource>(0, "matrix source", 20000, 0, 0.065, 0.04, 0, 0.001, 0.001, 100, 0, 1, 0, 0, std::vector<double>{ -3, -2, 0, 0 });
        //std::shared_ptr<Slit> s = std::make_shared<Slit>("slit", 1, 2, 20, 2, 7.5, 10000, 20, 1, m->getPhotonEnergy(), std::vector<double>{2, 1, 0, 0, 0, 0 }, nullptr, GLOBAL);
        
        GeometricUserParams param = GeometricUserParams(10, 10, 0, 10000, std::vector<double>{1,2,3,0.001,0.002,0.003});
        glm::dvec4 pos_mirror = param.calcPosition();
        glm::dmat4x4 or_mirror = param.calcOrientation();
        std::shared_ptr<PlaneMirror> pm = std::make_shared<PlaneMirror>("PM", 50, 200, pos_mirror, or_mirror, std::vector<double>{0,0,0,0,0,0,0});
        std::shared_ptr<PlaneMirror> pm2 = std::make_shared<PlaneMirror>("PM", 50, 200, 10, 0, 10000, std::vector<double>{1,2,3,0.001,0.002,0.003}, std::vector<double>{0,0,0,0,0,0,0}, nullptr, true);
        
        GeometricUserParams tor_param = GeometricUserParams(10, 10, 0, 10000, std::vector<double>{0,0,0, 0,0,0});
        glm::dvec4 tor_position = tor_param.calcPosition();
        glm::dmat4x4 tor_orientation = tor_param.calcOrientation();
        std::shared_ptr<ToroidMirror> t = std::make_shared<ToroidMirror>("ToroidMirror", 0, 50, 200, tor_position, tor_orientation, 10, 10000, 1000, 10000, 1000, std::vector<double>{0,0,0,0, 0,0,0});
        std::shared_ptr<ToroidMirror> t2 = std::make_shared<ToroidMirror>("ToroidMirror2", 0, 50, 200, 10, 0, 10000, 10000, 1000, 10000, 1000, std::vector<double>{0,0,0, 0,0,0}, std::vector<double>{0,0,0,0, 0,0,0}, nullptr, true);
        
        std::cout << "\n IMAGE PLANE \n" << std::endl;
        
        GeometricUserParams im_param = GeometricUserParams(0, 0, 0, 1000, std::vector<double>{0,0,0, 0,0,0});
        glm::dvec4 pos_imageplane = im_param.calcPosition(tor_param, tor_position, tor_orientation);
        glm::dmat4x4 or_imageplane = im_param.calcOrientation(tor_param, tor_position, tor_orientation);
        std::shared_ptr<ImagePlane> i = std::make_shared<ImagePlane>("Image plane", pos_imageplane, or_imageplane);
        std::shared_ptr<ImagePlane> i2 = std::make_shared<ImagePlane>("Image plane", 1000, t2, true);
        
        
        // petes setup
        //PointSource p = PointSource(0, "spec1_first_rzp4", number_of_rays, 1, 0.005, 0.005, 0, 0.02, 0.06, 1, 1, 0, 0, 640, 120, 1, 0, 0, { 0,0,0,0 });
        //ReflectionZonePlate rzp = ReflectionZonePlate("ReflectionZonePete", 1, 0, 1, 1, 4, 60, 170, 2.2, 0, 90, p.getPhotonEnergy(), p.getPhotonEnergy(), 1, 1, 2.2, 4.75, 90, 400, 90, 400, 0, 0, 1, 0, -24.35, 4.75, { 0,0,0, 0,0,0 }, { 0,0,0,0, 0,0,0 }, nullptr);  // dx,dy,dz, dpsi,dphi,dchi //
        //PlaneGrating plG = PlaneGrating("PeteGratingDeviationAzMis", 0, 50, 200, 10, 0.0, 7.5, 10000, 100, 1000, 1, 2, 0, { 0,0,0, 0,0,0 }, { 0,0,0,0,0,0 }, { 0,0,0,0,0, 0,0 }, nullptr); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        //ImagePlane ip = ImagePlane("Image Plane", 385, &rzp); // one out of the bunch
        //PlaneMirror pl = PlaneMirror("plane Mirror", 50, 200, 10, 0, 10000, { 0,0,0, 0,0,0 }, { 10,10, 0,0,0, 0,0 }, nullptr);

        addLightSource(m);
        for (int j = 0; j < beamlinesSimultaneously; j++) {
            generateRays(tracer, m);
        }
        m_Beamline.addOpticalElement(t2);
        //m_Beamline.addQuadric(pl);
        m_Beamline.addOpticalElement(i2);





        // MatrixSource m = MatrixSource(0, "matrix source", number_of_rays, 0, 0.065, 0.04, 0, 0.001, 0.001, 100, 0, { 0,0,0,0 });
        // PlaneMirror p = PlaneMirror("plane Mirror", 50, 200, 10, 0, 10000, { 0,0,0, 0,0,0 }, { 10,10, 0,0,0, 0,0 }, nullptr);
        // Slit s = Slit("slit", 0, 20, 2, 0, 10000, 20, 1, m.getPhotonEnergy(), { 0,0,0, 0,0,0 }, { 0,0, 0,0,0, 0,0 }, nullptr);
        // ImagePlane i = ImagePlane("Image plane", 1000, nullptr);
        //PointSource m = PointSource(0, "Point source 1", number_of_rays, 0.065, 0.04, 1.0, 0.001, 0.001, 0, 0, 0, 0, 100, 10, {0,0,0,0});
        //std::cout << m.getName() << " with " << m.getNumberOfRays() << " Rays." << std::endl;std::cout.precision(15); // show 16 decimals

        // addLightSource(&m);
        // generateRays(&tracer, &m);

        RAYX_DEBUG(std::cout << "add rays to tracer done" << std::endl);



        //ReflectionZonePlate p1 = ReflectionZonePlate("ReflectionZonePlate1", 1, 0, 50, 200, 170, 1, 10, 1000, 100, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, { 0,0,0, 0,0,0 }, NULL); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
        //RAYX::ReflectionZonePlate reflZonePlate = ReflectionZonePlate("ReflectionZonePlate", 1, 0, 4, 60, 170, 2.2, 0, 90, 640, 640, -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 0, { 0,0,0,0,0,0 }); // dx,dy,dz, dpsi,dphi,dchi // 
        // plane mirror with RAYX-UI default values
        //PlaneMirror p1 = PlaneMirror("PlaneMirror1", 50, 200, 10, 0, 10000, { 0,0,0, 0,0,0 }, nullptr); // {1,2,3,0.01,0.02,0.03}
        /*
        PlaneMirror p2 = PlaneMirror("PlaneMirror2", 50, 200, 15, 4, 10000, {1,2,3, 0.001,0.002,0.003}, &p1); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p3 = PlaneMirror("PlaneMirror3", 50, 200, 7, 10, 10000, {0,0,0, 0,0,0}, &p2); // {1,2,3,0.01,0.02,0.03}
        PlaneMirror p4 = PlaneMirror("PlaneMirror4", 50, 200, 22, 17, 10000, {0,0,0, 0,0,0}, &p3); // {1,2,3,0.01,0.02,0.03}
        */

        //add beamline to tracer
        const std::vector<std::shared_ptr<OpticalElement>> Elements = m_Beamline.getObjects();
        tracer.setBeamlineParameters(beamlinesSimultaneously, Elements.size(), number_of_rays);



        for (int j = 0; j < beamlinesSimultaneously; j++) {
            for (int i = 0; i<int(Elements.size()); i++) {
                std::cout << "add " << Elements[i]->getName() << std::endl;
                addOpticalElementToTracer(tracer, Elements[i]);//, Quadrics[i].getInverseMisalignmentMatrix()
            }
        }

        const clock_t begin_time = clock();
        tracer.run(); //run tracer
        std::cout << "tracer run time: " << float(clock() - begin_time) << " ms" << std::endl;

        RAYX_DEBUG(std::cout << "run succeeded" << std::endl);

        RAYX_DEBUG(std::cout << "tracerInterface run without output: " << float(clock() - all_begin_time) << " ms" << std::endl);
        if (true) {

            // transform in to usable data
            auto doubleVecSize = RAY_MAX_ELEMENTS_IN_VECTOR * RAY_DOUBLE_COUNT;
            std::vector<double> doubleVec(doubleVecSize);
            size_t index = 0;

            // Print ray footprint into file
            std::ofstream outputFile("output.csv");
            outputFile.precision(17);
            if (SHORTOUTPUT)
                outputFile << "Index;Xloc;Yloc\n";
            else
                outputFile << "Index;Xloc;Yloc;Zloc;Weight;Xdir;Ydir;Zdir;Energy;Stokes0;Stokes1;Stokes2;Stokes3\n";

            //get rays from tracer
            for (auto outputRayIterator = tracer.getOutputIteratorBegin(), outputIteratorEnd = tracer.getOutputIteratorEnd();
                outputRayIterator != outputIteratorEnd; outputRayIterator++)
            {
                RAYX_DEBUG(std::cout << "(*outputRayIterator).size(): " << (*outputRayIterator).size() << std::endl);

                memcpy(doubleVec.data(), (*outputRayIterator).data(), (*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT * sizeof(double));
                doubleVec.resize((*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT);

                RAYX_DEBUG(std::cout << "tracerInterface: sample ray: " << doubleVec[0] << ", " << doubleVec[1] << ", " << doubleVec[2] << ", " << doubleVec[3] << ", " << doubleVec[4] << ", " << doubleVec[5] << ", " << doubleVec[6] << ", energy: " << doubleVec[7] << std::endl);

                writeToFile(doubleVec, outputFile, index);
                index = index + (*outputRayIterator).size();
            }
            outputFile.close();

            RAYX_DEBUG(std::cout << "tracer run incl load rays time: " << float(clock() - begin_time) << " ms" << std::endl);

        }
        //clean up tracer to avoid memory leaks
        tracer.cleanup();
        //intentionally not RAYX_DEBUG()
        std::cout << "all done" << std::endl;
        return true;
    }

    //writes rays to file
    void TracerInterface::writeToFile(const std::vector<double>& outputRays, std::ofstream& file, int index) const
    {
        size_t size = outputRays.size();

        RAYX_DEBUG(std::cout << "writing " << outputRays.size() / 8 << " rays to file..." << std::endl);

        if (SHORTOUTPUT) {
            char buff[64];
            for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) { // ! + 8 because of placeholder
                sprintf(buff, "%d;%.17f;%.17f\n", index, outputRays[i], outputRays[i + 1]);
                file << buff;
                index++;
            }
        }
        else {
            char buff[384];
            for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) { // ! + 8 because of placeholder 
                sprintf(buff, "%d;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f\n", index,
                    outputRays[i], outputRays[i + 1], outputRays[i + 2], outputRays[i + 3],
                    outputRays[i + 4], outputRays[i + 5], outputRays[i + 6], outputRays[i + 7],
                    outputRays[i + 8], outputRays[i + 9], outputRays[i + 10], outputRays[i + 11]);
                file << buff;
                index++;
            }
        }

        RAYX_DEBUG(std::cout << "done!" << std::endl);
    }
}
// namespace RAYX
