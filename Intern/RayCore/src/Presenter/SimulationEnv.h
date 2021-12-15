#pragma once
#include <cstddef>

#include "Core.h"

namespace RAYX {
class RAYX_API SimulationEnv {
  public:
    static SimulationEnv& get();
    size_t genID();

    // ! temporarily public
    int m_numOfRays;

    SimulationEnv(SimulationEnv const&) = delete;
    void operator=(SimulationEnv const&) = delete;

  private:
    SimulationEnv() : m_numOfRays(2000000), m_idCounter(0) {}

    size_t m_idCounter;
};
}  // namespace RAYX