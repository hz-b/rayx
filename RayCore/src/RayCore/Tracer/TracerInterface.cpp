#include "TracerInterface.h"
#include "Ray.h"

namespace RAY
{
    TracerInterface::TracerInterface()
    {
    }

    TracerInterface::~TracerInterface()
    {
    }

    bool RAY::TracerInterface::run(const std::vector<Ray *> &rayList /*, BeamLine */)
    {
        return false;
    }
} // namespace RAY
