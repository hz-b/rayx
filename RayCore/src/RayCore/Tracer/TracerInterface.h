#pragma once

#include <vector>

class Ray;

namespace RAY
{
    class TracerInterface
    {
    public:
        TracerInterface();
        ~TracerInterface();

        bool run(const std::vector<Ray *> &m_rayList /*, BeamLine */ );
        
    private:
        std::vector<Ray *> m_rayList;
    };
} // namespace RAY