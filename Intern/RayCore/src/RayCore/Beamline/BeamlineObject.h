#pragma once
#include "Core.h"
#include <vector>
#include <iostream>
#include <stdexcept>

namespace RAY
{

    class RAY_API BeamlineObject
    {
    public:
        BeamlineObject(const char* name);
        BeamlineObject();
        ~BeamlineObject();

        const char* getName() const;

    private:
        const char* m_name;
        
        
    };


} // namespace RAY