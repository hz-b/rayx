#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API ImagePlane : public OpticalElement {

    public:

        ImagePlane(const char* name, const double distance, const std::shared_ptr<OpticalElement> previous, bool global);
        ImagePlane();
        ~ImagePlane();

        double getDistance();

    private:
        double m_distance;
    };
}
