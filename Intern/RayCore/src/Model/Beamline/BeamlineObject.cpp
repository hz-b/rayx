#include "BeamlineObject.h"

#include <math.h>

#include <cassert>

#include "Presenter/SimulationEnv.h"

namespace RAYX {
BeamlineObject::BeamlineObject(const char* name)
    : m_ID(SimulationEnv::get().genID()), m_name(name) {}
BeamlineObject::BeamlineObject() : m_ID(SimulationEnv::get().genID()) {}

BeamlineObject::~BeamlineObject() {}

const char* BeamlineObject::getName() const { return m_name; }

}  // namespace RAYX