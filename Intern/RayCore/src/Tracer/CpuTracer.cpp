
#include "CpuTracer.h"

#include <chrono>
#include <cmath>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "Model/Beamline/OpticalElement.h"

namespace RAYX {

namespace CPU_TRACER {
#define CPP
#include "shader/main.comp"
}  // namespace CPU_TRACER

CpuTracer::CpuTracer() { RAYX_VERB << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

std::vector<Ray> CpuTracer::trace(const Beamline& beamline) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto rayList = beamline.getInputRays();

    CPU_TRACER::numberOfBeamlines = 1;
    CPU_TRACER::numberOfElementsPerBeamline = (double)beamline.m_OpticalElements.size();
    CPU_TRACER::numberOfRays = (double)rayList.size();
    CPU_TRACER::numberOfRaysPerBeamLine = (double)rayList.size();

    CPU_TRACER::quadricData.data.clear();
    CPU_TRACER::xyznull.data.clear();
    CPU_TRACER::matIdx.data.clear();
    CPU_TRACER::mat.data.clear();

    // init rayData, outputData
    CPU_TRACER::rayData.data = rayList;
    CPU_TRACER::outputData.data.resize(rayList.size());

    // init quadricData
    for (auto el : beamline.m_OpticalElements) {
        CPU_TRACER::Element e;
        e.surfaceParams = el->getSurfaceParams();
        e.inTrans = el->getInMatrix();
        e.outTrans = el->getOutMatrix();
        e.objectParameters = el->getObjectParameters();
        e.elementParameters = el->getElementParameters();
        CPU_TRACER::quadricData.data.push_back(e);
    }

    auto materialTables = beamline.calcMinimalMaterialTables();
    CPU_TRACER::mat.data = materialTables.materialTable;
    CPU_TRACER::matIdx.data = materialTables.indexTable;

    // Run the tracing by for all rays
    for (int i = 0; i < CPU_TRACER::numberOfRays; i++) {
        CPU_TRACER::gl_GlobalInvocationID = i;
        CPU_TRACER::main();
    }

    // Fetch Rays back from the Shader "container"
    return CPU_TRACER::outputData.data;
}
}  // namespace RAYX
