#include "ImagePlane.h"

namespace RAY
{
    
    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @param width              width of mirror (x-dimension in element coord. sys.)
     * @param height             height of mirror (z-dimension in element coord. sys.)
     * @param grazingIncidence   desired incidence angle of the main ray
     * @param azimuthal          rotation of mirror around z-axis
     * @param dist               distance to preceeding element
     * @param misalignmentParams angles and distances for the mirror's misalignment
     * 
    */
    ImagePlane::ImagePlane(const char* name, double distance) 
    : Quadric(name) {
        m_distance = distance;
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        calcTransformationMatrices(0, 0, 0, 0, {0,0,0, 0,0,0});
        setParameters({distance,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0});
        setTemporaryMisalignment({0,0,0, 0,0,0}); 
        editQuadric({0,0,0,0, 0,0,0,0, 0,0,0,0, 5,0,0,0});
    }

    ImagePlane::~ImagePlane()
    {
    }

    double ImagePlane::getDistance() {
        return m_distance;
    }
 
}
