#pragma once
#include <cstddef>

#include "Core.h"

namespace RAYX
{
    class RAYX_API SimulationEnv {
    public:
        static SimulationEnv& get();
        size_t genID();

        // ! temporarily public
        int m_numOfRays;


        SimulationEnv(SimulationEnv const&) = delete;
        void operator=(SimulationEnv const&) = delete;

    private:
        SimulationEnv() :
            m_idCounter(0),
            m_numOfRays(200000)
        {}

        size_t m_idCounter = 0;
    };
} // namespace RAYX