#include "BeamlineObject.h"
#include "Presenter/SimulationEnv.h"
#include <cassert>
#include <math.h>

namespace RAYX
{
    BeamlineObject::BeamlineObject(const char* name)
        : m_ID(SimulationEnv::get().genID()),
        m_name(name)
    {}
    BeamlineObject::BeamlineObject()
        : m_ID(SimulationEnv::get().genID())
    {}

    BeamlineObject::~BeamlineObject()
    {
    }

    const char* BeamlineObject::getName() const {
        return m_name;
    }

} // namespace RAYX