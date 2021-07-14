#pragma once
#include "Core.h"
#include <vector>
#include <iostream>
#include <stdexcept>

namespace RAY
{
    /*
    * Brief: Abstract parent class for all beamline objects used in Ray-X.
    *
    */
    class RAY_API BeamlineObject
    {
    public:
        ~BeamlineObject();

        const char* getName() const;

    protected:
        BeamlineObject(const char* name);
        BeamlineObject();

    private:
        const char* m_name;


    };


} // namespace RAY