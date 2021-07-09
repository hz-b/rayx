#include "BeamlineObject.h"
#include <cassert>
#include <math.h>

namespace RAY
{
    BeamlineObject::BeamlineObject(const char* name) : m_name(name) {}
    BeamlineObject::BeamlineObject(){}

    BeamlineObject::~BeamlineObject()
    {
    }

    const char* BeamlineObject::getName() const {
        return m_name;
    }
    
} // namespace RAY