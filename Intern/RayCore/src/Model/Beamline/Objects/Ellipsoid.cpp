#include "Ellipsoid.h"

namespace RAYX
{


    /**
     * Angles given in degree and stored in rad. Initializes transformation matrices,
     * and parameters for the quadric in super class (quadric). Sets mirror-specific
     * parameters in this class.
     *
     * width, height = total width, height of the mirror (x- and z- dimensions)
     * grazingIncidence = desired incidence angle of the main ray
     * azimuthal = rotation of mirror around z-axis
     * distanceToPreceedingElement
     *
    */
    Ellipsoid::Ellipsoid(const char* name, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const double grazingIncidence,
        const double entranceArmLength, const double exitArmLength, const int figRot, const double a_11, const std::vector<double> slopeError)
        : OpticalElement(name, geometricalShape, width, height, position, orientation, slopeError),
        m_incidence(degToRad(grazingIncidence)),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_a11(a_11)
    {

        std::cout << name << std::endl;
        m_offsetY0 = 0;// what is this for? RAYX.FOR: "only !=0 in case of Monocapillary"

        m_figureRotation = (figRot == 0 ? FR_YES : (figRot == 1 ? FR_PLANE : FR_A11));
        calcHalfAxes();
        
        // a33, 34, 44
        m_a33 = pow(m_shortHalfAxisB / m_longHalfAxisA, 2);
        m_a34 = m_z0 * m_a33;
        m_a44 = -pow(m_shortHalfAxisB, 2) + pow(m_y0, 2) + pow(m_z0 * m_shortHalfAxisB / m_longHalfAxisA, 2);
        m_radius = -m_y0;

        double icurv = 1;
        setSurface(std::make_unique<Quadric>(std::vector<double>{m_a11, 0, 0, 0, icurv, 1, 0, m_radius, 0, 0, m_a33, m_a34, 7, 0, 0, m_a44}));
        setElementParameters({ sin(m_tangentAngle), cos(m_tangentAngle), m_y0, m_z0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });

    }


    Ellipsoid::~Ellipsoid()
    {
    }

    /*
    void Ellipsoid::calcRadius() {
        double theta = m_alpha; // grazing incidence in rad
        m_radius = 2.0/sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }*/

    // caclulates the half axes from the entrance and exit arm lengths, see ELLPARAM in RAYX.FOR
    void Ellipsoid::calcHalfAxes() {
        double theta = m_incidence; // designGrazingIncidenceAngle always equal to alpha (grazingIncidenceAngle)??
        if (theta > PI / 2) {
            theta = PI / 2;
        }
        double a = 0.5 * (m_entranceArmLength + m_exitArmLength);

        double angle = atan(tan(theta) * (m_entranceArmLength - m_exitArmLength) / (m_entranceArmLength + m_exitArmLength));
        m_y0 = m_entranceArmLength * sin(theta - angle);
        double b = a * m_y0 * tan(angle);
        b = 0.25 * pow(m_y0, 4) + pow(b, 2);
        b = sqrt(0.5 * m_y0 * m_y0 + sqrt(b));

        m_z0 = 0; // center of ellipsoid y0,z0
        if (b != 0) {
            m_z0 = (a / b) * (a / b) * m_y0 * tan(angle);
        }

        // << ellparam in RAY.for to calculate long and short half axis A and B
        m_longHalfAxisA = a;
        m_shortHalfAxisB = b;

        // calculate half axis C
        if (m_figureRotation == FR_YES) {
            m_halfAxisC = sqrt(pow(m_shortHalfAxisB, 2) / 1); // devided by 1??
        }
        else if (m_figureRotation == FR_PLANE) {
            std::cout << "[Ellipsoid]: FR PLane" << std::endl;
            m_halfAxisC = INFINITY;
        }
        else {
            m_halfAxisC = sqrt(pow(m_shortHalfAxisB, 2) / m_a11);
        }
        m_tangentAngle = angle;
        std::cout << "[Ellipsoid]: A= " << m_longHalfAxisA << ", B= " << m_shortHalfAxisB << ", C= " << m_halfAxisC << ", angle = " << m_tangentAngle << ", Z0 = " << m_z0 << ", Y0= " << m_y0 << std::endl;
    }

    double Ellipsoid::getRadius() {
        return m_radius;
    }

    double Ellipsoid::getExitArmLength() {
        return m_exitArmLength;
    }

    double Ellipsoid::getEntranceArmLength() {
        return m_entranceArmLength;
    }

    double Ellipsoid::getY0() {
        return m_y0;
    }

    double Ellipsoid::getZ0() {
        return m_z0;
    }
    double Ellipsoid::getIncidenceAngle() const {
        return m_incidence;
    }

    double Ellipsoid::getShortHalfAxisB() {
        return m_shortHalfAxisB;
    }
    double Ellipsoid::getLongHalfAxisA() {
        return m_longHalfAxisA;
    }
    double Ellipsoid::getOffsetY0() {
        return m_offsetY0;
    }
    double Ellipsoid::getTangentAngle() {
        return m_tangentAngle;
    }
    double Ellipsoid::getA34() {
        return m_a34;
    }
    double Ellipsoid::getA33() {
        return m_a33;
    }
    double Ellipsoid::getA44() {
        return m_a44;
    }
    double Ellipsoid::getHalfAxisC() {
        return m_halfAxisC;
    }
}
