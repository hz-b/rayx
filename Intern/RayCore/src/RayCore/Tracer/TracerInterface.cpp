#include <cmath>
#include <fstream>
#include <iomanip> 
#include <sstream>

#include "TracerInterface.h"

#include "Beamline/Objects/MatrixSource.h"
#include "Beamline/Objects/PointSource.h"
#include "Beamline/Objects/ReflectionZonePlate.h"
#include "UserParameter/GeometricUserParams.h"
#include "Debug.h"
#include "Ray.h"

#define SHORTOUTPUT false


namespace RAYX
{
    TracerInterface::TracerInterface() :
        m_numElements(0),
        m_numRays(0)
    {
        RAYX_DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::TracerInterface(int numElements, int numRays) :
        m_numElements(numElements),
        m_numRays(numRays)
    {
        RAYX_DEBUG(std::cout << "Creating TracerInterface..." << std::endl);
    }

    TracerInterface::~TracerInterface()
    {
        RAYX_DEBUG(std::cout << "Deleting TracerInterface..." << std::endl);
    }

    /** Generates rays from light source into the tracer.
     *
     *  @param tracer   reference to tracer
     *  @param source   shared pointer to light source used for ray generation
     */
    void TracerInterface::generateRays(std::shared_ptr<LightSource> source) {
        //only one Source for now
        if (!source) return;
        std::vector<RAYX::Ray> rays = source->getRays();
        RAYX_DEBUG(std::cout << "add rays" << std::endl);
        m_RayTracer.addRayVector(rays.data(), rays.size());
    }

    void TracerInterface::setBeamlineParameters() {
        m_RayTracer.setBeamlineParameters(1, m_numElements, m_numRays);
    }

    void TracerInterface::addOpticalElementToTracer(std::shared_ptr<OpticalElement> element) {
        m_RayTracer.addVectors(element->getSurfaceParams(), element->getInMatrix(), element->getOutMatrix(), element->getTempMisalignmentMatrix(), element->getInverseTempMisalignmentMatrix(), element->getObjectParameters(), element->getElementParameters());
    }

    // ! parameters are temporary and need to be removed again
    bool TracerInterface::run(double translationXerror, double translationYerror, double translationZerror)
    {


        RAYX_DEBUG(std::cout << "add rays to tracer done" << std::endl);

        const clock_t begin_time = clock();
        m_RayTracer.run(); //run tracer
        std::cout << "tracer run time: " << float(clock() - begin_time) << " ms" << std::endl;

        RAYX_DEBUG(std::cout << "run succeeded" << std::endl);

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
        for (auto outputRayIterator = m_RayTracer.getOutputIteratorBegin(), outputIteratorEnd = m_RayTracer.getOutputIteratorEnd();
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

        //clean up tracer to avoid memory leaks
        m_RayTracer.cleanup();
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
