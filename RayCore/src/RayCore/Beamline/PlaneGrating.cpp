#include "PlaneGrating.h"

namespace RAY
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @params: 
     *          mount = how angles of reflection are calculated: constant deviation, constant incidence,...
     *          width, height = total width, height of the mirror (x- and z- dimensions)
     *          deviation = angle between incoming and outgoing main ray
     *       or grazingIncidence = desired incidence angle of the main ray
     *          azimuthal = rotation of mirror around z-axis
     *          distanceToPreceedingElement
     *          designEnergyMounting = energy, taken from source
     *          lineDensity = line density of the grating
     *          orderOfDefraction = 
    */
    PlaneGrating::PlaneGrating(int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams) 
    : Quadric() {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_designEnergyMounting = designEnergyMounting;
        m_lineDensity = lineDensity;
        m_orderOfDiffraction = orderOfDiffraction;
        m_a = abs(hvlam(m_designEnergyMounting)) * m_lineDensity * m_orderOfDiffraction * 1e-6;
        
        m_gratingMount = mount==0 ? GM_DEVIATION : GM_INCIDENCE;
        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        
        calcAlpha(deviation, normalIncidence);
        
        // set parameters in Quadric class
        editQuadric({0,0,0,0, 0,0,0,-1, 0,0,0,0, 1,0,0,0});
        calcTransformationMatrices(PI/2-m_alpha, m_chi, PI/2-abs(m_beta), m_distanceToPreceedingElement);
        setMisalignment(misalignmentParams);
        setParameters({m_totalWidth, m_totalHeight, m_lineDensity, m_orderOfDiffraction, m_designEnergyMounting,m_a,0,0, 0,0,0,0, 0,0,0,0});
        // setMisalignment(misalignmentParams);
        // Quadric(inputPoints, m_alpha, m_chi, m_beta, m_distanceToPreceedingElement);
    }

    PlaneGrating::~PlaneGrating()
    {
    }

    void PlaneGrating::calcAlpha(double deviation, double normalIncidence) {
        double angle;
        if (m_gratingMount == GM_DEVIATION) {
            angle = deviation;
        }else if (m_gratingMount == GM_INCIDENCE) {
            angle = -deviation;
        }
        focus(angle);
    }

    void PlaneGrating::focus(double angle) {
        // from routine "focus" in RAY.FOR
        double theta = rad(abs(angle));
        if (angle <= 0) { // constant alpha mounting
            double arg = m_a - sin(theta);
            if(abs(arg) >= 1) { // cannot calculate alpha & beta
                m_alpha = 0;
                m_beta = 0;
            }else{
                m_alpha = theta;
                m_beta = asin(arg);
            }
        }else{  // constant alpha & beta mounting
            theta = theta / 2;
            double arg = m_a/2/cos(theta);
            if(abs(arg) >= 1) {
                m_alpha = 0;
                m_beta = 0;
            }else{
                m_beta = asin(arg) - theta;
                m_alpha = 2 * theta + m_beta;
            }
        }
        std::cout << "alpha, beta: " << m_alpha << ', '<< m_beta << std::endl;
    }

    double PlaneGrating::getWidth() {
        return m_totalWidth;
    }

    double PlaneGrating::getHeight() {
        return m_totalHeight;
    }
}
