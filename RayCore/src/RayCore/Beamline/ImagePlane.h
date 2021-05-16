#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API ImagePlane : public Quadric {

    public:
        
        ImagePlane(const char* name, double distance, Quadric* previous); 
        ImagePlane();
        ~ImagePlane();

        double getDistance();

    private:
        double m_distance;
    };
}
