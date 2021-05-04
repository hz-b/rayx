#pragma once
#include "Core.h"
#include "glm.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Tracer/Ray.h"
#include <string>
#include <random>

namespace RAY
{

    class RAY_API LightSource
    {
    public:
        
        LightSource(int id, int numberOfRays, const char* name, std::vector<double> misalignment);
        const char* getName();
        int getNumberOfRays();
        int getId();
        std::vector<double> getMisalignmentParams();
        void setNumberOfRays(int numberOfRays);
        glm::dvec3 getDirectionFromAngles(double phi, double psi);
        // get the rays according to specific light source, has to be implemented in each class that inherits from LightSource
        virtual std::vector<Ray> getRays() = 0;

        LightSource();
        virtual ~LightSource();

    private:
        int m_id;
        const char* m_name;
        int m_numberOfRays;
        int m_id;
        std::vector<double> m_misalignmentParams;
        // std::vector<Ray *> m_rayList; ?

    };

} // namespace RAY