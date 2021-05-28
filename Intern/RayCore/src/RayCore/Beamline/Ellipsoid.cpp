#include "Ellipsoid.h"

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
    Ellipsoid::Ellipsoid(const char* name, double width, double height, double grazingIncidence, double azimuthal, double distanceToPreceedingElement, 
        double entranceArmLength, double exitArmLength, int coordSys, int figRot, double a_11, std::vector<double> misalignmentParams, Quadric* previous) 
    : Quadric(name, previous) {
        std::cout << "ellipsoid" << std::endl;
        m_totalWidth = width;
        m_totalHeight = height;
        m_entranceArmLength = entranceArmLength;
        m_exitArmLength = exitArmLength;
        m_a11 = a_11;
        m_alpha = rad(grazingIncidence);
        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        m_offsetY0 = 0;// what is this for? RAY.FOR: "only !=0 in case of Monocapillary"

        m_misalignmentCoordSys = (coordSys == 0 ? CS_CURVATURE : CS_MIRROR);
        m_figureRotation = (figRot == 0 ? FR_YES : (figRot==1 ? FR_PLANE : FR_A11));

        calcHalfAxes();
        // grazingIncidence = m_alpha = m_alpha1 + d_tangentAngle
        calcAlphaBeta(grazingIncidence);
        // a33, 34, 44
        d_a33 = pow(m_shortHalfAxisB/m_longHalfAxisA, 2);
        d_a34 = m_z0 * d_a33;
        d_a44 = -pow(m_shortHalfAxisB,2) + pow(m_y0,2) + pow(m_z0*m_shortHalfAxisB/m_longHalfAxisA,2);
        m_radius = -m_y0;

        editQuadric({m_a11,0,0,0, 0,1,0,m_radius, 0,0,d_a33,d_a34, 0,0,0,d_a44});
        calcTransformationMatrices(m_alpha1, m_chi, m_beta, m_distanceToPreceedingElement, {0,0,0,0,0,0});
        setParameters({m_totalWidth,m_totalHeight,m_a11,m_y0, d_a33,d_a34,d_a44,0, 0,0,0,0, 0,0,0,0});

        // if m_misalignmentCoordSys == 1 rotate through d_tangentangle before misalignment and back after (-d_tangentangle)
        if (m_misalignmentCoordSys == CS_MIRROR){
            setTemporaryMisalignment({0,0,0,0,0,d_tangentAngle});
            std::vector<double> inTrans = getB2E();
            std::vector<double> outTrans = getE2B();
            std::vector<double> mis = getMisalignmentMatrix();
            std::vector<double> invMis = getInverseMisalignmentMatrix();
            std::vector<double> tempMis = getTempMisalignmentMatrix();
            std::vector<double> invTempMis = getInverseTempMisalignmentMatrix();

            std::vector<double> AT = getMatrixProductAsVector(tempMis, inTrans); // AT
            std::vector<double> MAT = getMatrixProductAsVector(mis, AT); //MAT
            std::vector<double> AMAT = getMatrixProductAsVector(invTempMis, MAT); //A^-1MAT
            setInMatrix(AMAT);

            std::vector<double> MA = getMatrixProductAsVector(invMis,tempMis); // M^-1A
            std::vector<double> AMA = getMatrixProductAsVector(invTempMis, MA); //A^-1M^-1A
            std::vector<double> TAMA = getMatrixProductAsVector(outTrans, AMA); // T-^1A^-1M^-1A
            setOutMatrix(TAMA);
        }else{
            setTemporaryMisalignment({0,0,0,0,0,0});
        }        
    }

    Ellipsoid::~Ellipsoid()
    {
    }

    void Ellipsoid::calcAlphaBeta(double grazingIncidence){
        m_alpha1 = m_alpha - d_tangentAngle;
        m_beta = m_alpha; // mirror -> exit angle = incidence angle
        std::cout << "alpha= " << m_alpha << " m_alpha1= " << m_alpha1 << " beta= "<< m_beta << std::endl;
    }

    /*
    void Ellipsoid::calcRadius() {
        double theta = m_alpha; // grazing incidence in rad
        m_radius = 2.0/sin(theta) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }*/

    // caclulates the half axes from the entrance and exit arm lengths, see ELLPARAM in RAY.FOR
    void Ellipsoid::calcHalfAxes() {
        double theta = m_alpha; // designGrazingIncidenceAngle always equal to alpha (grazingIncidenceAngle)??
        if(theta > PI/2) {
            theta = PI/2;
        }
        double a = 0.5 * (m_entranceArmLength+m_exitArmLength);

        double angle = atan(tan(theta) * (m_entranceArmLength - m_exitArmLength) / (m_entranceArmLength + m_exitArmLength));
        m_y0 = m_entranceArmLength * sin(theta - angle);
        double b = a * m_y0 * tan(angle);
        b =  0.25 * pow(m_y0,4) + pow(b,2);
        b = sqrt(0.5 * m_y0*m_y0 + sqrt(b));

        m_z0 = 0; // center of ellipsoid y0,z0
        if (b!=0) {
            m_z0 = (a/b)*(a/b) * m_y0 * tan(angle); 
        }
        m_longHalfAxisA = a;
        m_shortHalfAxisB = b;
        if(m_figureRotation == FR_YES) {
            d_halfAxisC = sqrt(pow(m_shortHalfAxisB,2)/1); // devided by 1??
        }else if(m_figureRotation == FR_PLANE) {
            d_halfAxisC = INFINITY;
        }else{
            d_halfAxisC = sqrt(pow(m_shortHalfAxisB,2)/m_a11);
        }
        d_tangentAngle = angle;
        std::cout << "A= "<< m_longHalfAxisA << ", B= " << m_shortHalfAxisB << ", C= " << d_halfAxisC << ", angle = " << d_tangentAngle << ", Z0 = " << m_z0 << ", Y0= " << m_y0 << std::endl;
    }

    double Ellipsoid::getWidth() {
        return m_totalWidth;
    }

    double Ellipsoid::getHeight() {
        return m_totalHeight;
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
    
    double Ellipsoid::getMy0() {
        return m_y0;
    }
    
    double Ellipsoid::getMz0(){
        return m_z0;
    }
    double Ellipsoid::getAlpha() {
        return m_alpha;
    }
    double Ellipsoid::getBeta() {
        return m_beta;
    }
    double Ellipsoid::getChi() {
        return m_chi;
    }
    double Ellipsoid::getDistanceToPreceedingElement() {
        return m_distanceToPreceedingElement;
    }
    double Ellipsoid::getShortHalfAxisB(){
        return m_shortHalfAxisB;
    }
    double Ellipsoid::getLongHalfAxisA(){
        return m_longHalfAxisA;
    }
    double Ellipsoid::getOffsetY0() {
        return m_offsetY0;
    }
    double Ellipsoid::getAlpha1(){ // from tangent angle and grazing incidence???
        return m_alpha1;
    }
    double Ellipsoid::getTangentAngle() {
        return d_tangentAngle;
    }
    double Ellipsoid::getA34() {
        return d_a34;
    }
    double Ellipsoid::getA33() {
        return d_a33;
    }
    double Ellipsoid::getA44() {
        return d_a44;
    }
    double Ellipsoid::getHalfAxisC() {
        return d_halfAxisC;
    }
}
