#include "BeamlineObject.h"

namespace RAY
{
    BeamlineObject::BeamlineObject(const char* name)
        : m_name(name)
    {
    }


    BeamlineObject::~BeamlineObject()
    {
        delete m_name;
    }




} // namespace RAY