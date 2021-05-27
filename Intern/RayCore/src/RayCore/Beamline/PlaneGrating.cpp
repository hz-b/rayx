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
    PlaneGrating::PlaneGrating(const char* name, int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergyMounting, double lineDensity, double orderOfDiffraction, double fixFocusConstantCFF, std::vector<double> misalignmentParams, std::vector<double> vls) 
    : Quadric(name) {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_designEnergyMounting = designEnergyMounting;
        m_lineDensity = lineDensity; // 1/d d = linespacing
        m_orderOfDiffraction = orderOfDiffraction;
        m_a = abs(hvlam(m_designEnergyMounting)) * abs(m_lineDensity) * m_orderOfDiffraction * 1e-6; // wavelength * linedensity * order of diffraction * 10^-6
        
        m_gratingMount = mount==0 ? GM_DEVIATION : (mount==1 ? GM_INCIDENCE : (mount==2 ? GM_CCF : GM_CCF_NO_PREMIRROR));
        m_fixFocusConstantCFF = fixFocusConstantCFF;
        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        calcAlpha(deviation, normalIncidence);
        std::cout << "alpha: " << m_alpha << ", beta: " << m_beta << " a: " << m_a << std::endl;
        
        // set parameters in Quadric class
        editQuadric({0,0,0,0, m_totalWidth,0,0,-1, m_totalHeight,m_a,0,0, 1,0,0,0});
        m_vls = vls; // into parameters
        calcTransformationMatrices(m_alpha, m_chi, m_beta, m_distanceToPreceedingElement, misalignmentParams);
        setParameters({m_totalWidth, m_totalHeight, m_lineDensity, m_orderOfDiffraction, m_designEnergyMounting,m_a,m_vls[0],m_vls[1], m_vls[2],m_vls[3],m_vls[4],m_vls[5], 0,0,0,0});
        setTemporaryMisalignment({0,0,0,0,0,0});
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
            angle = -normalIncidence;
        }else if(m_gratingMount == GM_CCF) {
            // TODO
        }else if(m_gratingMount == GM_CCF_NO_PREMIRROR) {
            // TODO
        }
        focus(angle);
    }

    void PlaneGrating::focus(double angle) {
        // from routine "focus" in RAY.FOR
        double theta = rad(abs(angle));
        std::cout << "deviation " << angle << "theta" << theta << std::endl;
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
        std::cout << m_alpha << ", " << m_beta << " angles" << std::endl;
        m_alpha = PI/2-m_alpha;
        m_beta = PI/2-abs(m_beta);
    }

    double PlaneGrating::getWidth() {
        return m_totalWidth;
    }

    double PlaneGrating::getHeight() {
        return m_totalHeight;
    }

    double PlaneGrating::getAlpha() {
        return m_alpha;
    }
    double PlaneGrating::getBeta() {
        return m_beta;
    }
    // in rad as well
    double PlaneGrating::getChi() {
        return m_chi;
    }
    double PlaneGrating::getDistanceToPreceedingElement() {
        return m_distanceToPreceedingElement;
    }
    int PlaneGrating::getGratingMount() {
        return m_gratingMount;
    }
    double PlaneGrating::getFixFocusConstantCFF() {
        return m_fixFocusConstantCFF;
    }

    double PlaneGrating::getDesignEnergyMounting () {
        return m_designEnergyMounting;
    }
    double PlaneGrating::getLineDensity(){
        return m_lineDensity;
    }
    double PlaneGrating::getOrderOfDiffraction() {
        return m_orderOfDiffraction;
    }
    double PlaneGrating::getA() { // calculated from line density, order of diffracion and design energy mounting
        return m_a;
    }
    std::vector<double> PlaneGrating::getVls() {
        return m_vls;
    };
}
