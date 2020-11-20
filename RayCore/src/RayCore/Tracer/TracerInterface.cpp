#include "TracerInterface.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
    }

    TracerInterface::TracerInterface(const std::vector<Ray *> &rayList) : m_rayList(rayList)
    {
    }

    TracerInterface::~TracerInterface()
    {
    }

} // namespace RAY
