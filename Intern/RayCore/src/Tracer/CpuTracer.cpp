
#include "CpuTracer.h"

#include <chrono>
#include <cmath>

#include "Debug.h"
#include "Debug/Instrumentor.h"
#include "Material/Material.h"
#include "PathResolver.h"

namespace RAYX {

namespace CPP_TRACER {
#define CPP
#include "shader/main.comp"
}  // namespace CPP_TRACER

CpuTracer::CpuTracer() { RAYX_LOG << "Initializing Cpu Tracer.."; }

CpuTracer::~CpuTracer() {}

RayList CpuTracer::trace(const Beamline& beamline) {
    auto rayList = beamline.getInputRays();

    CPP_TRACER::numberOfBeamlines = 1;
    CPP_TRACER::numberOfElementsPerBeamline = beamline.m_OpticalElements.size();
    CPP_TRACER::numberOfRays = rayList.rayAmount();
    CPP_TRACER::numberOfRaysPerBeamLine = rayList.rayAmount();

    CPP_TRACER::rayData.data.clear();
    CPP_TRACER::outputData.data.clear();
    CPP_TRACER::quadricData.data.clear();
    CPP_TRACER::xyznull.data.clear();
    CPP_TRACER::matIdx.data.clear();
    CPP_TRACER::mat.data.clear();
#ifdef RAYX_DEBUG_MODE
    CPP_TRACER::d_struct.data.clear();
#endif

    // init rayData, outputData
    for (auto a : rayList) {
        for (auto r : a) {
            CPP_TRACER::rayData.data.push_back(r);
            CPP_TRACER::outputData.data.push_back({});
        }
    }

    // init quadricData
    for (auto el : beamline.m_OpticalElements) {
        CPP_TRACER::Element e;
        e.surfaceParams = arrayToGlm16(el->getSurfaceParams());
        e.inTrans = arrayToGlm16(el->getInMatrix());
        e.outTrans = arrayToGlm16(el->getOutMatrix());
        e.objectParameters = arrayToGlm16(el->getObjectParameters());
        e.elementParameters = arrayToGlm16(el->getElementParameters());
        CPP_TRACER::quadricData.data.push_back(e);
    }

    auto materialTables = beamline.calcMinimalMaterialTables();
    CPP_TRACER::mat.data = materialTables.materialTable;
    CPP_TRACER::matIdx.data = materialTables.indexTable;

// init debug buffer
#ifdef RAYX_DEBUG_MODE
    for (int i = 0; i < CPP_TRACER::numberOfRays; i++) {
        CPP_TRACER::_debug_struct d;
        CPP_TRACER::d_struct.data.push_back(d);
    }
#endif

    // Run the tracing by for all rays
    for (int i = 0; i < CPP_TRACER::numberOfRays; i++) {
        CPP_TRACER::gl_GlobalInvocationID = i;
        CPP_TRACER::main();
    }

    // Fetch Rays back from the Shader "container"
    RayList out;
    for (auto r : CPP_TRACER::outputData.data) {
        out.push(r);
    }
    return out;
}

void CpuTracer::setMaterialTables(MaterialTables materialTables) {
    CPP_TRACER::mat.data = materialTables.materialTable;
    CPP_TRACER::matIdx.data = materialTables.indexTable;
}

}  // namespace RAYX
