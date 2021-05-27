#include "SphereGrating.h"

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
    SphereGrating::SphereGrating(const char* name, int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams, std::vector<double> vls) 
    : Quadric(name) {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_designEnergyMounting = designEnergyMounting;
        m_lineDensity = lineDensity;
        m_orderOfDiffraction = orderOfDiffraction;
        m_a = abs(hvlam(m_designEnergyMounting)) * abs(m_lineDensity) * m_orderOfDiffraction * 1e-6;
        m_entranceArmLength = entranceArmLength;
        m_exitArmLength = exitArmLength;
        
        m_gratingMount = mount==0 ? GM_DEVIATION : GM_INCIDENCE;
        m_chi = rad(azimuthal);
        m_deviation = rad(deviation);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        m_vls = vls;

        calcAlpha(deviation, normalIncidence);
        calcRadius();
        // std::cout << m_a << std::endl;
        // set parameters in Quadric class
        editQuadric({1,0,0,0, m_totalWidth,1,0,-m_radius, m_totalHeight,m_a,1,0, 2,0,0,0});
        calcTransformationMatrices(m_alpha, m_chi, m_beta, m_distanceToPreceedingElement, misalignmentParams);
        setParameters({m_totalWidth, m_totalHeight, m_lineDensity, m_orderOfDiffraction, m_designEnergyMounting,m_a,m_vls[0],m_vls[1], m_vls[2],m_vls[3],m_vls[4],m_vls[5], 0,0,0,0});
        // Quadric(inputPoints, m_alpha, m_chi, m_beta, m_distanceToPreceedingElement);
    }

    SphereGrating::~SphereGrating()
    {
    }

    void SphereGrating::calcRadius() {
        if(m_gratingMount == GM_DEVIATION) {
            double theta = m_deviation > 0 ? (PI-m_deviation)/2 : PI/2 + m_deviation;
            m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }else if(m_gratingMount == GM_INCIDENCE) {
            double ca = cos(m_alpha);
            double cb = cos(m_beta);
            m_radius = (ca + cb) / ((ca*ca) / m_entranceArmLength + (cb*cb) / m_exitArmLength);
        }
    }

    void SphereGrating::calcAlpha(double deviation, double normalIncidence) {
        double angle;
        if (m_gratingMount == GM_DEVIATION) {
            angle = deviation;
        }else if (m_gratingMount == GM_INCIDENCE) {
            angle = -normalIncidence;
        }
        focus(angle);
        m_alpha = (PI/2)-m_alpha;
        m_beta = (PI/2)-abs(m_beta);
    }

    void SphereGrating::focus(double angle) {
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
    }

    double SphereGrating::getWidth() {
        return m_totalWidth;
    }

    double SphereGrating::getHeight() {
        return m_totalHeight;
    }

    double SphereGrating::getRadius() {
        return m_radius;
    }

    double SphereGrating::getExitArmLength() {
        return m_exitArmLength;
    }
    double SphereGrating::getEntranceArmLength(){
        return m_entranceArmLength;
    }
    // angles in rad and normal angles (measured from normal! not incidence!!)
    double SphereGrating::getAlpha(){
        return m_alpha;
    }
    double SphereGrating::getBeta(){
        return m_beta;
    }
    double SphereGrating::getDeviation(){
        return m_deviation;
    }
    double SphereGrating::getChi() {
        return m_chi;
    }
    double SphereGrating::getDistanceToPreceedingElement(){
        return m_distanceToPreceedingElement;
    }
    int SphereGrating::getGratingMount(){
        return m_gratingMount;
    }
    double SphereGrating::getDesignEnergyMounting(){
        return m_designEnergyMounting;
    }
    double SphereGrating::getLineDensity(){
        return m_lineDensity;
    }
    double SphereGrating::getOrderOfDiffraction(){
        return m_orderOfDiffraction;
    }
    double SphereGrating::getA(){
        return m_a;
    }
    
}
