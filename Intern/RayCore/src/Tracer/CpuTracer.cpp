
#include "CpuTracer.h"

#include <chrono>
#include <cmath>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "PathResolver.h"

namespace RAYX {

namespace CPU_TRACER {
#define CPP
#include "shader/main.comp"
}  // namespace CPU_TRACER

CpuTracer::CpuTracer() { RAYX_LOG << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

RayList CpuTracer::trace(const Beamline& beamline) {
    auto rayList = beamline.getInputRays();

    CPU_TRACER::numberOfBeamlines = 1;
    CPU_TRACER::numberOfElementsPerBeamline = beamline.m_OpticalElements.size();
    CPU_TRACER::numberOfRays = rayList.rayAmount();
    CPU_TRACER::numberOfRaysPerBeamLine = rayList.rayAmount();

    CPU_TRACER::rayData.data.clear();
    CPU_TRACER::outputData.data.clear();
    CPU_TRACER::quadricData.data.clear();
    CPU_TRACER::xyznull.data.clear();
    CPU_TRACER::matIdx.data.clear();
    CPU_TRACER::mat.data.clear();

    // init rayData, outputData
    for (auto r : rayList) {
        CPU_TRACER::rayData.data.push_back(r);
        CPU_TRACER::outputData.data.push_back({});
    }

    // init quadricData
    for (auto el : beamline.m_OpticalElements) {
        CPU_TRACER::Element e;
        e.surfaceParams = arrayToGlm16(el->getSurfaceParams());
        e.inTrans = el->getInMatrix();
        e.outTrans = el->getOutMatrix();
        e.objectParameters = arrayToGlm16(el->getObjectParameters());
        e.elementParameters = arrayToGlm16(el->getElementParameters());
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
    RayList out;
    for (auto r : CPU_TRACER::outputData.data) {
        out.push(r);
    }

    return out;
}
}  // namespace RAYX
