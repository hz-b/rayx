#include "ReflectionZonePlate.h"

namespace RAY
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets RZP-specific parameters in this class
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
    ReflectionZonePlate::ReflectionZonePlate(const char* name, int mount, int curvatureType, double width, double height, double deviation, double normalIncidence, double azimuthal, double distanceToPreceedingElement, double designEnergy, double lineDensity, double orderOfDiffraction, std::vector<double> misalignmentParams)
        : Quadric(name) {
        // parameters in array 
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_totalWidth = width;
        m_totalHeight = height;
        m_designEnergy = designEnergy; // eV
        double wavelength = hvlam(m_designEnergy) * 1e-6;  // mm
        m_orderOfDiffraction = -1; // auto = m_designOrderOfDiffraction

        double designOrderOfDiffraction = -1; // hard coded (will be a parameter later)
        m_designEnergyMounting = 2; // TODO
        m_designAlphaAngle = rad(1);
        m_designBetaAngle = rad(1);

        calcDesignOrderOfDiffraction(designOrderOfDiffraction);
        m_sagittalEntranceArmLength = 500; //mm
        m_meriodicalEntranceArmLength = 500; // hard coded (will be a parameter later)

        // m_lineDensity = lineDensity;
        // m_a = abs(hvlam(m_designEnergyMounting)) * m_lineDensity * m_orderOfDiffraction * 1e-6;
        m_curvatureType = curvatureType == 0 ? CT_PLANE : (curvatureType == 1 ? CT_TOROIDAL : CT_SPHERICAL);

        m_gratingMount = mount == 0 ? GM_DEVIATION : GM_INCIDENCE;
        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        calcAlpha(deviation, normalIncidence);
        std::cout << "alpha: " << (PI / 2 - m_alpha) << ", beta: " << PI / 2 - abs(m_beta) << std::endl;

        // set parameters in Quadric class
        if (m_curvatureType == CT_PLANE) {
            editQuadric({ 0,0,0,0, m_totalWidth,0,0,-1, m_totalHeight,m_a,0,0, 1,0,0,0 });
        }        
else if (m_curvatureType == CT_SPHERICAL) {
            m_longRadius = 2; // TODO derive from input params
            m_shortRadius = 1; // TODO
            editQuadric({ 1,0,0,0, m_totalWidth,1,0,-m_longRadius, m_totalHeight,m_a,1,0, 1,0,0,0 });
        }        
else {
            // no structure for non-quadric elements yet
            editQuadric({ 1,0,0,0, m_totalWidth,1,0,-m_radius, m_totalHeight,m_a,1,0, 1,0,0,0 });
        }
        calcTransformationMatrices(PI / 2 - m_alpha, m_chi, PI / 2 - abs(m_beta), m_distanceToPreceedingElement, { 0,0,0,0,0,0 }); // the whole misalignment is stored in temporaryMisalignment because it needs to be temporarily removed during tracing (-> store in separate matrix, not inMatrix/outMatrix)
        setTemporaryMisalignment(misalignmentParams);
        setParameters({ m_totalWidth, m_totalHeight, m_lineDensity, m_orderOfDiffraction, m_designEnergyMounting,m_a,0,0, 0,0,0,0, 0,0,0,0 });
    }

    ReflectionZonePlate::~ReflectionZonePlate()
    {
    }

    /**
     * calc alpha (incidence angle) from given mount (deviation or incidence angle given)
     *
     * @params: deviation:      deviation angle between incoming and outgoing ray
     *          incidenceAngle: grazing incidence angle
    */
    void ReflectionZonePlate::calcAlpha(double deviation, double incidenceAngle) { //  grazing or normal?
        double angle;
        if (m_gratingMount == GM_DEVIATION) {
            angle = deviation;
            double d0 = calcDz00(); // what is this doing?
            m_a = abs(hvlam(m_designEnergy)) * m_lineDensity * m_orderOfDiffraction * 1e-6;
            focus(angle);
        }        
else if (m_gratingMount == GM_INCIDENCE) {
            angle = incidenceAngle;
        }
    }

    double ReflectionZonePlate::calcDz00() {
        double wavelength = m_designEnergy == 0 ? 0 : inm2eV / m_designEnergy;
        double beta;
        double zeta;
        double fresnelZOffset;
        if (m_designType == DT_BETA) {
            beta = m_designBetaAngle;
            calcFresnelZOffset();
        }        
else if (m_designType == DT_ZOFFSET) {
            beta = m_betaAngle; // what is this beta angle???
        }
        return 1.0;
    }

    /**
     * calculate fresnel z offset if DESIGN_TYPE == BETA from angle beta.
     * @param:
    */
    void ReflectionZonePlate::calcFresnelZOffset() {
        double betaAngle;
        if (m_designType == DT_BETA) {
            betaAngle = m_designBetaAngle;
        }
        m_betaAngle = betaAngle;
        double RIcosa = m_sagittalEntranceArmLength * cos(m_designAlphaAngle);
        double ROcosb = m_sagittalExitArmLength * cos(m_betaAngle);
        double RIsina = m_sagittalEntranceArmLength * sin(m_designAlphaAngle);
        double ROsinb = m_sagittalExitArmLength * sin(m_betaAngle);
        double tanTheta = (RIsina + ROsinb) / (RIcosa + ROcosb);
        m_calcFresnelZOffset = (RIsina / tanTheta) - RIcosa;
    }

    /**
     * calculate beta if DESIGN_TYPE == ZOFFSET from fresnel z offset
     * analogous to calcFresnelZOffset if design type is beta.
    */
    void ReflectionZonePlate::calcBeta() {
        if (m_designType == DT_ZOFFSET) {
            VectorR2Center();
            if (m_frenselZOffset != 0) { // m_fresnelZOffset is given by the user as a parameter bc DESIGN_TYPE==DT_ZOFFSET
                m_betaAngle = acos((-m_R2ArmLength * m_R2ArmLength * m_sagittalExitArmLength * m_sagittalExitArmLength * m_frenselZOffset * m_frenselZOffset) / (2 * m_sagittalExitArmLength * m_frenselZOffset));
            }
        }
    }

    void ReflectionZonePlate::VectorR1Center() {
        if (m_designType == DT_ZOFFSET) {
            double param_R1cosZ = m_sagittalEntranceArmLength * cos(m_designAlphaAngle) * m_frenselZOffset;
            m_R1ArmLength = sqrt(pow(param_R1cosZ, 2) + pow(m_sagittalEntranceArmLength * sin(m_designAlphaAngle), 2));
            m_alpha0Angle = acos(param_R1cosZ / m_R1ArmLength);
        }        
else if (m_designType == DT_BETA) {
            double RIcosa = m_sagittalEntranceArmLength * cos(m_designAlphaAngle);
            double ROcosb = m_sagittalExitArmLength * cos(m_designBetaAngle);
            double RIsina = m_sagittalEntranceArmLength * sin(m_designAlphaAngle);
            double ROsinb = m_sagittalExitArmLength * sin(m_designBetaAngle);
            double m_alpha0Angle = (RIsina + ROsinb) / (RIcosa + ROcosb);
            m_R2ArmLength = RIsina / sin(m_alpha0Angle);
        }
    }

    void ReflectionZonePlate::VectorR2Center() {
        if (m_designType == DT_ZOFFSET) {
            VectorR1Center();
            double R2s = m_sagittalExitArmLength;
            double alpha = m_alpha0Angle; // why another alpha angle??
            m_R2ArmLength = 0.5 * (-2 * m_frenselZOffset * cos(alpha) + sqrt(pow(2 * R2s, 2) - 2 * pow(m_frenselZOffset, 2) + 2 * pow(m_frenselZOffset, 2) * cos(2 * alpha)));
            m_beta0Angle = m_alpha0Angle;
        }        
else if (m_designType == DT_BETA) {
            double RIcosa = m_sagittalEntranceArmLength * cos(m_designAlphaAngle);
            double ROcosb = m_sagittalExitArmLength * cos(m_designBetaAngle);
            double RIsina = m_sagittalEntranceArmLength * sin(m_designAlphaAngle);
            double ROsinb = m_sagittalExitArmLength * sin(m_designBetaAngle);
            m_beta0Angle = (RIsina + ROsinb) / (RIcosa + ROcosb);
            m_R2ArmLength = ROsinb / sin(m_beta0Angle);
        }
    }

    /**
     * calculates the design order of diffraction based on the design type (using the design beta angle
     * or the fresnel center offset)
     *
     * @params: designOrderOfDiffraction: parameter given by user
    */
    void ReflectionZonePlate::calcDesignOrderOfDiffraction(double designOrderOfDiffraction) {
        int presign;
        if (m_designType == DT_ZOFFSET) {
            presign = (m_designAlphaAngle >= m_designBetaAngle) ? -1 : 1;
        }        
else if (m_designType == DT_BETA) {
            presign = (m_frenselZOffset >= 0) ? -1 : 1;
        }
        m_designOrderOfDiffraction = abs(m_designOrderOfDiffraction) * presign;
    }

    void ReflectionZonePlate::focus(double angle) {
        // from routine "focus" in RAY.FOR
        double theta = rad(abs(angle));
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
    }

    double ReflectionZonePlate::getWidth() {
        return m_totalWidth;
    }

    double ReflectionZonePlate::getHeight() {
        return m_totalHeight;
    }
}
