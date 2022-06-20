#pragma once

#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>

#include "Core.h"
#include "Material/Material.h"
#include "Tracer/RayList.h"
#include "Tracer/Tracer.h"
#include "vulkan/vulkan.hpp"

struct Element {
    glm::dmat4 surfaceParams;
    glm::dmat4 inTrans;
    glm::dmat4 outTrans;
    glm::dmat4 objectParameters;
    glm::dmat4 elementParameters;
};

namespace RAYX {

class RAYX_API CpuTracer : public Tracer {
  public:
    CpuTracer();
    ~CpuTracer();

    RayList trace(const Beamline&) override;

  private:
    void run();
    // void addRay(double xpos, double ypos, double zpos, double xdir, double
    // ydir, double zdir, double weight); void addRay(double* location);
    // cleans and destroys the whole tracer instance
    // CALL CLEANTRACER BEFORE CALLING THIS ONE
    void cleanup();
    // empties raylist, Beamline and output data, etc. so that the tracer
    // instance can be used again
    void cleanTracer();

    void getRays();
    void addRayVector(std::vector<Ray>&& inRayVector);
    void addArrays(const std::array<double, 4 * 4>& surfaceParams,
                   const std::array<double, 4 * 4>& inputInMatrix,
                   const std::array<double, 4 * 4>& inputOutMatrix,
                   const std::array<double, 4 * 4>& objectParameters,
                   const std::array<double, 4 * 4>& elementParameters);
    void setBeamlineParameters(uint32_t inNumberOfBeamlines,
                               uint32_t inNumberOfQuadricsPerBeamline,
                               uint32_t inNumberOfRays);

    // getter
    std::list<std::vector<Ray>>::const_iterator getOutputIteratorBegin();
    std::list<std::vector<Ray>>::const_iterator getOutputIteratorEnd();

    const RayList& getRayList() { return m_RayList; }

    // Member structs:
    /* not contiguous in memory, shouldn't be used
    struct Quadric{
        Quadric() : points(16), inMatrix(16), outMatrix(16) {}
        Quadric(std::vector<double> inQuadric, std::vector<double>
    inputInMatrix, std::vector<double> inputOutMatrix){ assert(inQuadric.size()
    == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16); points
    = inQuadric; inMatrix = inputInMatrix; outMatrix = inputOutMatrix;
        }
        std::vector<double> points;
        std::vector<double> inMatrix;
        std::vector<double> outMatrix;
    };
    */

    // Ray-related vars:
    uint32_t m_numberOfBeamlines;
    uint32_t m_numberOfQuadricsPerBeamline;
    uint32_t m_numberOfRays;
    uint32_t m_numberOfRaysPerBeamline;
    RayList m_RayList;
    RayList m_OutputRays;
    std::vector<Element> m_elementData;

    // Material tables
    MaterialTables m_MaterialTables;
    std::array<bool, 92> m_relevantMaterials;

    struct Settings {
        bool m_isDebug;
        uint32_t m_buffersCount;
        uint32_t m_computeBuffersCount;
        uint32_t m_stagingBuffersCount;
    } m_settings;

    // Member functions:
    // Vulkan
    void prepareVulkan();
    void mainLoop();
    void setSettings();

    // Ray-related funcs:
    void divideAndSortRays();

    int main();
};
}  // namespace RAYX
