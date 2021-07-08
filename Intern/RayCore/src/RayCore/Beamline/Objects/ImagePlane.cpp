#include "ImagePlane.h"

namespace RAY
{

    /**
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * @param dist               distance to preceeding element
     * @param previous           pointer to previous element in beamline, needed for calculating world coordinates
     *
    */
    ImagePlane::ImagePlane(const char* name, const double distance, const Quadric* const previous)
        : Quadric(name, 0, 0, {0,0,0,0,0, 0,0}, previous) {
        m_distance = distance;
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        calcTransformationMatrices(0, 0, 0, distance, { 0,0,0, 0,0,0 });
        setElementParameters({ distance,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setObjectParameters({ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
        editQuadric({ 0,0,0,0, 0,0,0,0, 0,0,0,0, 5,0,0,0 });
    }

    ImagePlane::~ImagePlane()
    {
    }

    double ImagePlane::getDistance() {
        return m_distance;
    }

}
