#include "SphereGrating.h"

namespace RAYX
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
    SphereGrating::SphereGrating(const char* name, int mount, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double entranceArmLength, double exitArmLength, double designEnergyMounting, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams, std::vector<double> vls, std::vector<double> slopeError, std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, width, height, rad(azimuthal), distanceToPreceedingElement, slopeError, previous),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_deviation(rad(deviation)),
        m_designEnergyMounting(designEnergyMounting),
        m_lineDensity(lineDensity),
        m_orderOfDiffraction(orderOfDiffraction),
        m_vls(vls)
    {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};

        m_a = abs(hvlam(m_designEnergyMounting)) * abs(m_lineDensity) * m_orderOfDiffraction * 1e-6;
        m_gratingMount = mount == 0 ? GM_DEVIATION : GM_INCIDENCE;

        calcAlpha(deviation, normalIncidence);
        calcRadius();
        // std::cout << m_a << std::endl;
        // set parameters in Quadric class
        double icurv = 1; // -1 if radius < 0 in fortran
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, icurv,1,0,-m_radius, 0,0,1,0, 2,0,0,0}));
        calcTransformationMatrices(misalignmentParams, global);
        setElementParameters({
            0, 0, m_lineDensity, m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)), 0, m_vls[0], m_vls[1],
            m_vls[2], m_vls[3], m_vls[4], m_vls[5],
            0, 0, 0, 0
            });
    }

    SphereGrating::~SphereGrating()
    {
    }

    void SphereGrating::calcRadius() {
        if (m_gratingMount == GM_DEVIATION) {
            double theta = m_deviation > 0 ? (PI - m_deviation) / 2 : PI / 2 + m_deviation;
            m_radius = 2.0 / sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
        }        
        else if (m_gratingMount == GM_INCIDENCE) {
            double ca = cos(getAlpha());
            double cb = cos(getBeta());
            m_radius = (ca + cb) / ((ca * ca) / m_entranceArmLength + (cb * cb) / m_exitArmLength);
        }
    }

    void SphereGrating::calcAlpha(double deviation, double normalIncidence) {
        double angle;
        if (m_gratingMount == GM_DEVIATION) {
            angle = deviation;
        }        
        else if (m_gratingMount == GM_INCIDENCE) {
            angle = -normalIncidence;
        }
        focus(angle);
    }

    void SphereGrating::focus(double angle) {
        // from routine "focus" in RAYX.FOR
        double theta = rad(abs(angle));
        double alph, bet;
        if (angle <= 0) { // constant alpha mounting
            double arg = m_a - sin(theta);
            if (abs(arg) >= 1) { // cannot calculate alpha & beta
                alph = 0;
                bet = 0;
            }            
            else {
                alph = theta;
                bet = asin(arg);
            }
        }        
        else {  // constant alpha & beta mounting
            theta = theta / 2;
            double arg = m_a / 2 / cos(theta);
            if (abs(arg) >= 1) {
                alph = 0;
                bet = 0;
            }            
            else {
                bet = asin(arg) - theta;
                alph = 2 * theta + bet;
            }
        }
        setAlpha((PI / 2) - alph);
        setBeta((PI / 2) - abs(bet));
    }

    double SphereGrating::getRadius() const {
        return m_radius;
    }

    double SphereGrating::getExitArmLength() const {
        return m_exitArmLength;
    }
    double SphereGrating::getEntranceArmLength() const {
        return m_entranceArmLength;
    }
    
    double SphereGrating::getDeviation() const {
        return m_deviation;
    }
    int SphereGrating::getGratingMount() const {
        return m_gratingMount;
    }
    double SphereGrating::getDesignEnergyMounting() const {
        return m_designEnergyMounting;
    }
    double SphereGrating::getLineDensity() const {
        return m_lineDensity;
    }
    double SphereGrating::getOrderOfDiffraction() const {
        return m_orderOfDiffraction;
    }
    double SphereGrating::getA() const {
        return m_a;
    }

}
