#include "PlaneGrating.h"

namespace RAYX
{
    // TODO(Jannis): complete documentation
    /**
     * Angles given in degree and stored in rad. Initializes transformation
     * matrices, and parameters for the quadric in super class (quadric).
     * Sets mirror-specific parameters in this class.
     * @param name                          name of the plane grating
     * @param mount                         how angles of reflection are calculated: constant deviation, constant incidence,...
     * @param width                         total width and..
     * @param height                        ..height of the mirror (x- and z- dimensions)
     * @param deviation                     angle between incoming and outgoing main ray
     * @param normalIncidence               angle between incoming ray and normal of element
     * @param azimuthal                     rotation of mirror around z-axis
     * @param distanceToPreceedingElement   distance to previous element
     * @param designEnergyMounting          energy, taken from source
     * @param lineDensity                   line density of the grating
     * @param orderOfDiffraction            the order in which the grating should refract the ray
     * @param fixFocusConstantCFF           
     * @param additionalZeroOrder           if true half of the rays will be refracted in the 0th order (=reflection), if false all will be refracted according to orderOfDiffraction Parameter
     * @param misalignmentParams            misalignmen parameters (x,y,z position and x,y,z direction)
     * @param vls                           vls grating paramters (6) (variable line spacing)
     * @param slopeError                    7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * @param previous                      pointer to previous element in beamline, needed for caclultation transformation matrices in global coordinate system
     * @param global                        if tracing should be done in global or ray coordinates
    
    PlaneGrating::PlaneGrating(const char* name, const int mount, const double width, const double height, const double deviation, const double normalIncidence, const double azimuthal, const double distanceToPreceedingElement, const double designEnergyMounting, const double lineDensity, const double orderOfDiffraction, const double fixFocusConstantCFF, const int additionalZeroOrder, const std::vector<double> misalignmentParams, const std::vector<double> vls, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, width, height, rad(azimuthal), distanceToPreceedingElement, slopeError, previous),
        m_designEnergyMounting(designEnergyMounting),
        m_lineDensity(lineDensity),
        m_orderOfDiffraction(orderOfDiffraction)
    {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        std::cout << "wavelength" << abs(hvlam(m_designEnergyMounting)) << std::endl;
        m_additionalOrder = additionalZeroOrder;
        m_gratingMount = mount == 0 ? GM_DEVIATION : (mount == 1 ? GM_INCIDENCE : (mount == 2 ? GM_CCF : GM_CCF_NO_PREMIRROR));

        calcAlpha(deviation, normalIncidence);
        std::cout.precision(17);
        std::cout << "alpha: " << getAlpha() << ", beta: " << getBeta() << std::endl;

        // set parameters in Optical Element class
        m_vls = vls; // into element parameters
        calcTransformationMatricesFromAngles(misalignmentParams, global);
        setElementParameters({
            0, 0, m_lineDensity, m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)), 0, m_vls[0], m_vls[1],
            m_vls[2], m_vls[3], m_vls[4], m_vls[5],
            0, 0, 0, double(m_additionalOrder) });
        setTemporaryMisalignment({ 0,0,0,0,0,0 });
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0}));
    }*/



    /**
     * simplified constructor that assumes that incidence and exit angle are already calculated and the position and orientation has been derived from them already
     * 
     * @param name                  name of element
     * @param width                 width of element (x dimension)
     * @param height                height of element (z dimension)
     * @param position              position in world coordinate system
     * @param orientation           orientation(rotation) of element in world coordinate system
     * @param designEnergy          the energy for which the grating is designed. design wavelength can be derived from this
     * @param lineDensity           line density of grating
     * @param orderOfDiffraction    the order in which the grating should refract the ray
     * @param additionalZeroOrder   if true half of the rays will be refracted in the 0th order (=reflection), if false all will be refracted according to orderOfDiffraction Parameter
     * @param vls                   vls grating paramters (6) (variable line spacing)
     * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion: x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * 
     */
    PlaneGrating::PlaneGrating(const char* name, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double designEnergy, const double lineDensity, const double orderOfDiffraction, const int additionalZeroOrder, const std::vector<double> vls, const std::vector<double> slopeError)
        : OpticalElement(name, width, height, position, orientation, slopeError),
        m_designEnergyMounting(designEnergy),
        m_lineDensity(lineDensity),
        m_orderOfDiffraction(orderOfDiffraction),
        m_additionalOrder(additionalZeroOrder),
        m_vls(vls) // TODO(Theresa): store these five params in element as attributes (m_..) at all or only in elementParams?
    {
        std::cout << "design wavelength = " << abs(hvlam(m_designEnergyMounting)) << std::endl;
        
        // set element specific parameters in Optical Element class. will be moved to shader and are needed for tracing
        setElementParameters({
            0, 0, m_lineDensity, m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)), 0, m_vls[0], m_vls[1],
            m_vls[2], m_vls[3], m_vls[4], m_vls[5],
            0, 0, 0, double(m_additionalOrder) });
        
        setTemporaryMisalignment({ 0,0,0,0,0,0 }); // remove??
        // parameters of quadric surface
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0}));
    }

    PlaneGrating::~PlaneGrating()
    {
    }

    /* functions to derive angles from user input parameters -> moved to somewhere else for now */
    /*void PlaneGrating::calcAlpha(const double deviation, const double normalIncidence) {
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
        double alph, bet;
        double a = abs(hvlam(m_designEnergyMounting)) * abs(m_lineDensity) * m_orderOfDiffraction * 1e-6;
        std::cout << "deviation " << angle << "theta" << theta << std::endl;
        if (angle <= 0) { // constant alpha mounting
            double arg = a - sin(theta);
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
            double arg = a / 2 / cos(theta);
            if (abs(arg) >= 1) {
                alph = 0;
                bet = 0;
            }
            else {
                bet = asin(arg) - theta;
                alph = 2 * theta + bet;
            }
        }
        std::cout << alph << ", " << bet << " angles" << std::endl;
        setAlpha(PI / 2 - alph);
        setBeta(PI / 2 - abs(bet));
    }*/
    /* END */

    double PlaneGrating::getDesignEnergyMounting() {
        return m_designEnergyMounting;
    }
    double PlaneGrating::getLineDensity() {
        return m_lineDensity;
    }
    double PlaneGrating::getOrderOfDiffraction() {
        return m_orderOfDiffraction;
    }
    std::vector<double> PlaneGrating::getVls() {
        return m_vls;
    };
}
