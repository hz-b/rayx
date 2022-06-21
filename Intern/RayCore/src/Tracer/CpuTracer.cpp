
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

// those conversion functions only need to exist, as the shader doesn't use m_ prefixes to variables, whereas the c++ code does.
CPP_TRACER::Ray convert(Ray r) {
    CPP_TRACER::Ray out;
    out.position = r.m_position;
    out.weight = r.m_weight;
    out.direction = r.m_direction;
    out.energy = r.m_energy;
    out.stokes = r.m_stokes;
    out.pathLength = r.m_pathLength;
    out.order = r.m_order;
    out.lastElement = r.m_lastElement;
    out.extraParameter = r.m_extraParam; // TODO: unite extraParameter vs. extraParam
    return out;
}

Ray back_convert(CPP_TRACER::Ray r) {
    Ray out;
    out.m_position = r.position;
    out.m_weight = r.weight;
    out.m_direction = r.direction;
    out.m_energy = r.energy;
    out.m_stokes = r.stokes;
    out.m_pathLength = r.pathLength;
    out.m_order = r.order;
    out.m_lastElement = r.lastElement;
    out.m_extraParam = r.extraParameter;
    return out;
}

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
    CPP_TRACER::d_struct.data.clear();

    // init rayData, outputData
    for (auto a : rayList) {
        for (auto r : a) {
            CPP_TRACER::rayData.data.push_back(convert(r));
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
    for (int i = 0; i < CPP_TRACER::numberOfRays; i++) {
        CPP_TRACER::_debug_struct d;
        CPP_TRACER::d_struct.data.push_back(d);
    }

    for (int i = 0; i < CPP_TRACER::numberOfRays; i++) {
        CPP_TRACER::gl_GlobalInvocationID = i;
        CPP_TRACER::main();
    }

    RayList outRays;
    for (auto r : CPP_TRACER::outputData.data) {
        outRays.insertVector({back_convert(r)});
    }

    return outRays;
}

}  // namespace RAYX
