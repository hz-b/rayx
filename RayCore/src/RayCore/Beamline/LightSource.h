#pragma once

#include "Core.h"
#include "glm.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Tracer/Ray.h"

namespace RAY
{

    class RAY_API LightSource
    {
    public:
        
        LightSource(int id, int numberOfRays, char* name);
        char* const getName();
        int getNumberOfRays();
        void setNumberOfRays(int numberOfRays);
        int getId();
        glm::dvec3 getDirectionFromAngles(double phi, double psi);
        // get the rays according to specific light source, has to be implemented in each class that inherits from LightSource
        virtual std::vector<Ray *> getRays() = 0;

        LightSource();
        virtual ~LightSource();

    private:
        char* m_name;
        int m_numberOfRays;
        int m_id;
        // std::vector<Ray *> m_rayList; ?

    };

} // namespace RAY