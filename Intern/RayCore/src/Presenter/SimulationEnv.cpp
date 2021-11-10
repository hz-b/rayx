#include "SimulationEnv.h"

namespace RAYX {

SimulationEnv& SimulationEnv::get() {
    static SimulationEnv simEnv;
    return simEnv;
}

size_t SimulationEnv::genID() { return ++m_idCounter; }

}  // namespace RAYX