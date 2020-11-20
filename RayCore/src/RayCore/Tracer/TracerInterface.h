#pragma once

#include <vector>

class Ray;

namespace RAY
{
    class TracerInterface
    {
    public:
        TracerInterface();
        TracerInterface(const std::vector<Ray *> &m_rayList);
        ~TracerInterface();

    private:
        std::vector<Ray *> m_rayList;
    };
} // namespace RAY