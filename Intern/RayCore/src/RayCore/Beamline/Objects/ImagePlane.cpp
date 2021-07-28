#include "ImagePlane.h"

namespace RAYX
{

    /**
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * @param dist               distance to preceeding element
     * @param previous           pointer to previous element in beamline, needed for calculating world coordinates
     *
    */
    ImagePlane::ImagePlane(const char* name, const double distance, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, 0, 0, { 0,0,0,0,0, 0,0 }, previous) {
        m_distance = distance;
        setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0 }));
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        calcTransformationMatrices(0, 0, 0, distance, { 0,0,0, 0,0,0 }, global);
        setElementParameters({ distance,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setObjectParameters({ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
    }

    ImagePlane::~ImagePlane()
    {
    }

    double ImagePlane::getDistance() {
        return m_distance;
    }

}
