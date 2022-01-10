#include "TracerInterface.h"

#include <cmath>
#include <fstream>
#include <highfive/H5Easy.hpp>
#include <iomanip>
#include <sstream>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "Model/Beamline/LightSource.h"
#include "Model/Beamline/OpticalElement.h"
#include "Ray.h"

#define SHORTOUTPUT false

namespace RAYX {
TracerInterface::TracerInterface() : m_numElements(0), m_numRays(0) {
    RAYX_D_LOG << "Creating TracerInterface...";
}

TracerInterface::TracerInterface(int numElements, int numRays)
    : m_numElements(numElements), m_numRays(numRays) {
    RAYX_D_LOG << "Creating TracerInterface with " << numElements
               << " elements and " << numRays << " rays...";
}

TracerInterface::~TracerInterface() {
    RAYX_D_LOG << "Deleting TracerInterface...";
}

/** Generates rays from light source into the tracer.
 *
 *  @param tracer   reference to tracer
 *  @param source   shared pointer to light source used for ray generation
 */
void TracerInterface::generateRays(std::shared_ptr<LightSource> source) {
    // only one Source for now
    if (!source) return;
    RAYX_D_LOG << "add rays";
    m_RayTracer.addRayVector(source->getRays());
}

void TracerInterface::setBeamlineParameters() {
    m_RayTracer.setBeamlineParameters(1, m_numElements, m_numRays);
}

void TracerInterface::addOpticalElementToTracer(
    std::shared_ptr<OpticalElement> element) {
    m_RayTracer.addVectors(element->getSurfaceParams(), element->getInMatrix(),
                           element->getOutMatrix(),
                           element->getObjectParameters(),
                           element->getElementParameters());
}

bool TracerInterface::run() {
    RAYX_PROFILE_FUNCTION();

    m_RayTracer.run();  // run tracer
    RAYX_D_LOG << "Run succeeded!";

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
        outputFile << "Index;Xloc;Yloc;Zloc;Weight;Xdir;Ydir;Zdir;Energy;"
                      "Stokes0;Stokes1;Stokes2;Stokes3;pathLength;order;"
                      "lastElement;extraParam\n";

    // get rays from tracer
    for (auto outputRayIterator = m_RayTracer.getOutputIteratorBegin(),
              outputIteratorEnd = m_RayTracer.getOutputIteratorEnd();
         outputRayIterator != outputIteratorEnd; outputRayIterator++) {
        RAYX_D_LOG << "(*outputRayIterator).size(): "
                   << (*outputRayIterator).size();

        memcpy(doubleVec.data(), (*outputRayIterator).data(),
               (*outputRayIterator).size() * VULKANTRACER_RAY_DOUBLE_AMOUNT *
                   sizeof(double));
        doubleVec.resize((*outputRayIterator).size() *
                         VULKANTRACER_RAY_DOUBLE_AMOUNT);

        RAYX_D_LOG << "sample ray: " << doubleVec[0] << ", " << doubleVec[1]
                   << ", " << doubleVec[2] << ", " << doubleVec[3] << ", "
                   << doubleVec[4] << ", " << doubleVec[5] << ", "
                   << doubleVec[6] << ", energy: " << doubleVec[7]
                   << ", stokes 0: " << doubleVec[8];

        writeToFile(doubleVec, outputFile, index);
        index = index + (*outputRayIterator).size();
    }
    outputFile.close();

    // clean up tracer to avoid memory leaks
    m_RayTracer.cleanTracer();
    m_RayTracer.cleanup();
    // intentionally not RAYX_DEBUG()
    RAYX_LOG << "Done.";
    return true;
}

// writes rays to file
void TracerInterface::writeToFile(const std::vector<double>& outputRays,
                                  std::ofstream& file, int index) const {
    size_t size = outputRays.size();

    RAYX_D_LOG << "Writing " << outputRays.size() / RAY_DOUBLE_COUNT
               << " rays to file...";

    if (SHORTOUTPUT) {
        char buff[64];
        for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) {
            sprintf(buff, "%d;%.17f;%.17f\n", index, outputRays[i],
                    outputRays[i + 1]);
            file << buff;
            index++;
        }
    } else {
        char buff[384];
        for (size_t i = 0; i < size; i = i + RAY_DOUBLE_COUNT) {
            sprintf(buff,
                    "%d;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f;%"
                    ".17f;%.17f;%.17f;%.17f;%.17f;%.17f;%.17f\n",
                    index, outputRays[i], outputRays[i + 1], outputRays[i + 2],
                    outputRays[i + 3], outputRays[i + 4], outputRays[i + 5],
                    outputRays[i + 6], outputRays[i + 7], outputRays[i + 8],
                    outputRays[i + 9], outputRays[i + 10], outputRays[i + 11],
                    outputRays[i + 12], outputRays[i + 13], outputRays[i + 14],
                    outputRays[i + 15]);
            file << buff;
            index++;
        }
    }

    RAYX_D_LOG << "Writing done!";
}
}  // namespace RAYX
// namespace RAYX
