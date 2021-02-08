#include "SphereMirror.h"

namespace RAY
{
    
    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @params: 
     *          width, height = total width, height of the mirror (x- and z- dimensions)
     *          grazingIncidence = desired incidence angle of the main ray
     *          azimuthal = rotation of mirror around z-axis
     *          distanceToPreceedingElement
     * 
    */
    SphereMirror::SphereMirror(double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, std::vector<double> misalignmentParams) 
    : Quadric({1,0,0,0, width,1,0,-1, height,0,1,0, 0,0,0,0}, {width, height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence), distanceToPreceedingElement) {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_entranceArmLength = entranceArmLength;
        m_exitArmLength = exitArmLength;
        m_chi = rad(azimuthal);
        m_alpha = rad(grazingIncidence);
        m_beta = m_alpha; // mirror -> exit angle = incidence angle
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        
        calcRadius(); // berechnen
        editQuadric({1,0,0,0, 0,1,0,-m_radius, 0,0,1,0, 0,0,0,0});
        setMisalignment(misalignmentParams);
    }

    SphereMirror::~SphereMirror()
    {
    }

    void SphereMirror::calcRadius() {
        double theta = m_alpha; // grazing incidence in rad
        m_radius = 2.0/sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }


    double SphereMirror::getWidth() {
        return m_totalWidth;
    }

    double SphereMirror::getHeight() {
        return m_totalHeight;
    }

    double SphereMirror::getRadius() {
        return m_radius;
    }
}
