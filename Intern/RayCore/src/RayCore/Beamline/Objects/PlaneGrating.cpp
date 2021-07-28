#include "PlaneGrating.h"

namespace RAYX
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @param mount                 how angles of reflection are calculated: constant deviation, constant incidence,...
     * @param width,                total width and..
     * @param height                ..height of the mirror (x- and z- dimensions)
     * @param deviation             angle between incoming and outgoing main ray
     * @param grazingIncidence      desired incidence angle of the main ray
     * @param azimuthal             rotation of mirror around z-axis
     * @param distanceToPreceedingElement
     * @param designEnergyMounting  energy, taken from source
     * @param lineDensity           line density of the grating
     * @param orderOfDefraction
     * @param fixFocusConstantCFF
     * @param misalignmentParams    misalignmen parameters (x,y,z position and x,y,z direction)
     * @param vls                   vls grating paramters (6)
     * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * @param previous              pointer to previous element in beamline, needed for caclultation transformation matrices in global coordinate system
    */
    PlaneGrating::PlaneGrating(const char* name, const int mount, const double width, const double height, const double deviation, const double normalIncidence, const double azimuthal, const double distanceToPreceedingElement, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const double fixFocusConstantCFF, const int additional_zero_order, const std::vector<double> misalignmentParams, const std::vector<double> vls, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global) 
    : OpticalElement(name, width, height, slopeError, previous),
        m_totalWidth(width), 
        m_totalHeight(height), 
        m_fixFocusConstantCFF(fixFocusConstantCFF), 
        m_designEnergyMounting(designEnergyMounting), 
        m_lineDensity(lineDensity),
        m_orderOfDiffraction(orderOfDiffraction) 
     {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_a = abs(hvlam(m_designEnergyMounting)) * abs(m_lineDensity) * m_orderOfDiffraction * 1e-6; // wavelength * linedensity * order of diffraction * 10^-6
        std::cout << "wavelength" << abs(hvlam(m_designEnergyMounting)) << std::endl;
        m_additionalOrder = additional_zero_order == 0 ? AO_OFF : AO_ON;
        m_gratingMount = mount == 0 ? GM_DEVIATION : (mount == 1 ? GM_INCIDENCE : (mount == 2 ? GM_CCF : GM_CCF_NO_PREMIRROR));
        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        calcAlpha(deviation, normalIncidence);
        std::cout << "alpha: " << m_alpha << ", beta: " << m_beta << " a: " << m_a << std::endl;

        // set parameters in Optical Element class
        m_vls = vls; // into element parameters
        calcTransformationMatrices(m_alpha, m_chi, m_beta, m_distanceToPreceedingElement, misalignmentParams, global);
        setElementParameters({
            m_totalWidth, m_totalHeight, m_lineDensity, m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)), 0, m_vls[0], m_vls[1],
            m_vls[2], m_vls[3], m_vls[4], m_vls[5],
            0, 0, 0, double(m_additionalOrder) });
        setTemporaryMisalignment({ 0,0,0,0,0,0 });
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0}));
    }

    PlaneGrating::~PlaneGrating()
    {
    }

    void PlaneGrating::calcAlpha(const double deviation, const double normalIncidence) {
        double angle;
        if (m_gratingMount == GM_DEVIATION) {
            angle = deviation;
        }        
else if (m_gratingMount == GM_INCIDENCE) {
            angle = -normalIncidence;
        }        
else if (m_gratingMount == GM_CCF) {
            // TODO
        }        
else if (m_gratingMount == GM_CCF_NO_PREMIRROR) {
            // TODO
        }
        focus(angle);
    }

    void PlaneGrating::focus(double angle) {
        // from routine "focus" in RAYX.FOR
        double theta = rad(abs(angle));
        std::cout << "deviation " << angle << "theta" << theta << std::endl;
        if (angle <= 0) { // constant alpha mounting
            double arg = m_a - sin(theta);
            if (abs(arg) >= 1) { // cannot calculate alpha & beta
                m_alpha = 0;
                m_beta = 0;
            }            
else {
                m_alpha = theta;
                m_beta = asin(arg);
            }
        }        
else {  // constant alpha & beta mounting
            theta = theta / 2;
            double arg = m_a / 2 / cos(theta);
            if (abs(arg) >= 1) {
                m_alpha = 0;
                m_beta = 0;
            }            
else {
                m_beta = asin(arg) - theta;
                m_alpha = 2 * theta + m_beta;
            }
        }
        std::cout << m_alpha << ", " << m_beta << " angles" << std::endl;
        m_alpha = PI / 2 - m_alpha;
        m_beta = PI / 2 - abs(m_beta);
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

    double PlaneGrating::getDesignEnergyMounting() {
        return m_designEnergyMounting;
    }
    double PlaneGrating::getLineDensity() {
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
