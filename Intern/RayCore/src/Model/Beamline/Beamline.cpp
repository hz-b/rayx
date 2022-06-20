#include "Beamline.h"

#include <iostream>

#include "Debug/Instrumentor.h"
#include "Model/Beamline/Objects/Objects.h"

namespace RAYX {
Beamline::Beamline() {}

Beamline::~Beamline() {}

RayList Beamline::getInputRays() const {
    RayList list;

    list.insertVector(m_extraRays);

    for (auto s : m_LightSources) {
        auto sub = s->getRays();
        list.insertVector(sub);
    }

    return list;
}

}  // namespace RAYX
