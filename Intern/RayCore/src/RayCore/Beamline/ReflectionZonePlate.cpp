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
     *          designEnergy = given by user, used if auto==false
     *          sourceEnergy = energy, taken from source, used if auto==true
     *          lineDensity = line density of the grating
     *          orderOfDiffraction =
    */
    ReflectionZonePlate::ReflectionZonePlate(const char* name, int mount, int curvatureType, int designType, int elementOffsetType, double width, double height, double deviation, double incidenceAngle, double azimuthal, double distanceToPreceedingElement, double designEnergy, double sourceEnergy, double orderOfDiffraction, double designOrderOfDiffraction, double dAlpha, double dBeta, double mEntrance, double mExit, double sEntrance, double sExit, double shortRadius, double longRadius, double elementOffsetZ, double beta, std::vector<double> misalignmentParams, Quadric* previous)
        : Quadric(name, previous) {
        m_totalWidth = width;
        m_totalHeight = height;
        m_designEnergy = sourceEnergy; // eV, if auto == true, else designEnergy
        // m_designEnergy = designEnergy; // if Auto == true, take energy of Source (param sourceEnergy), else m_designEnergy = designEnergy
        m_wavelength = m_designEnergy == 0 ? 0 : inm2eV / m_designEnergy;
        m_designOrderOfDiffraction = designOrderOfDiffraction;
        m_orderOfDiffraction = m_designOrderOfDiffraction; // auto = m_designOrderOfDiffraction, else given param

        m_designAlphaAngle = rad(dAlpha);
        m_designBetaAngle = rad(dBeta);

        m_curvatureType = curvatureType == 0 ? CT_PLANE : (curvatureType == 1 ? CT_TOROIDAL : CT_SPHERICAL);
        m_gratingMount = mount == 0 ? GM_DEVIATION : GM_INCIDENCE;
        m_designType = designType == 0 ? DT_ZOFFSET : DT_BETA; // default (0)
        m_derivationMethod = DM_FORMULA; // default (0)
        m_rzpType = RT_ELLIPTICAL; // default (0)
        m_imageType = IT_POINT2POINT; // default (0)
        m_diffractionMethod = DM_2D; // 2D default
        m_fullEfficiency = FE_OFF; // default (1)
        m_elementOffsetType = elementOffsetType == 0 ? EZ_MANUAL : EZ_BEAMDIVERGENCE; //EZ_BEAMDIVERGENCE; // EZ_MANUAL; // default (0)
        m_elementOffsetZ = elementOffsetZ;

        calcDesignOrderOfDiffraction(designOrderOfDiffraction);
        m_sagittalEntranceArmLength = sEntrance; //mm
        m_meridionalEntranceArmLength = mEntrance; // hard coded (will be a parameter later)
        m_sagittalExitArmLength = sExit;
        m_meridionalExitArmLength = mExit;

        m_chi = rad(azimuthal);
        m_distanceToPreceedingElement = distanceToPreceedingElement;
        m_grazingIncidenceAngle = rad(incidenceAngle);
        // m_derivationAngle = derivation; // not used in RAY-UI, does grating mount even matter?
        m_meridionalDistance = 0;
        m_meridionalDivergence = 0;

        m_frenselZOffset = 0; // default
        calcAlpha2();
        if (beta == 0) { // calculate from other parameters
            calcBeta2();
        }
        else { // auto == true
            setBeta(beta);
        }
        std::cout << "alpha: " << m_alpha << ", beta: " << m_beta << std::endl;
        printInfo();
        // set parameters in Quadric class
        if (m_curvatureType == CT_PLANE) {
            editQuadric({ 0,0,0,0, m_totalWidth,0,0,-1, m_totalHeight,0,0,0, 4,0,0,0 });
        }
        else if (m_curvatureType == CT_SPHERICAL) {
            m_longRadius = longRadius; // for sphere and toroidal
            editQuadric({ 1,0,0,0, m_totalWidth,1,0,-m_longRadius, m_totalHeight,0,1,0, 4,0,0,0 });
        }
        else {
            // no structure for non-quadric elements yet
            m_longRadius = longRadius; // for sphere and toroidal
            m_shortRadius = shortRadius; // only for Toroidal
            editQuadric({ 1,0,0,0, m_totalWidth,1,0,-m_longRadius, m_totalHeight,0,1,0, 4,0,0,0 });
        }

        calcTransformationMatrices(m_alpha, m_chi, m_beta, m_distanceToPreceedingElement, misalignmentParams);
        // the whole misalignment is also stored in temporaryMisalignment because it needs to be temporarily removed during tracing
        setTemporaryMisalignment(misalignmentParams);
        setParameters({
            double(m_imageType), double(m_rzpType), double(m_derivationMethod), m_wavelength,
            m_designEnergy, m_designOrderOfDiffraction,m_orderOfDiffraction,m_frenselZOffset,
            m_sagittalEntranceArmLength,m_sagittalExitArmLength,m_meridionalEntranceArmLength,m_meridionalExitArmLength,
            m_designAlphaAngle,m_designBetaAngle,m_elementOffsetZ,0 }
        );
    }

    ReflectionZonePlate::~ReflectionZonePlate()
    {
    }

    void ReflectionZonePlate::printInfo() {
        std::cout.precision(17);

        std::cout << m_elementOffsetType;
        if (m_elementOffsetType == EZ_MANUAL) {
            std::cout << " elementoffsettype: MANUAL" << std::endl;
        }
        else if (m_elementOffsetType == EZ_BEAMDIVERGENCE) {
            std::cout << " elementoffsettype: BEAMDIVERGENCE" << std::endl;
        }

        std::cout << m_gratingMount;
        if (m_gratingMount == GM_INCIDENCE) {
            std::cout << " m_gratingMount: INCIDENCE" << std::endl;
        }
        else if (m_gratingMount == GM_DEVIATION) {
            std::cout << " m_gratingMount: DEVIATION" << std::endl;
        }

        std::cout << m_rzpType;
        if (m_rzpType == RT_ELLIPTICAL) {
            std::cout << " rzp type: ELLIPTICAL" << std::endl;
        }
        else if (m_rzpType == RT_MERIODIONAL) {
            std::cout << " rzp type: MERIDIONAL" << std::endl;
        }

        std::cout << m_designType;
        if (m_designType == DT_ZOFFSET) {
            std::cout << " design type: OFFSET" << std::endl;
        }
        else if (m_designType == DT_BETA) {
            std::cout << " design type: BETA" << std::endl;
        }

        std::cout << m_curvatureType;
        if (m_curvatureType == CT_PLANE) {
            std::cout << " curvature type: PLANE" << std::endl;
        }
        else if (m_curvatureType == CT_SPHERICAL) {
            std::cout << " curvature type: SPHERICAL" << std::endl;
        }
        else if (m_curvatureType == CT_TOROIDAL) {
            std::cout << " curvature type: TOROIDAL" << std::endl;
        }

        std::cout << m_diffractionMethod;
        if (m_diffractionMethod == DM_TWOGRATINGS) {
            std::cout << " diffraction method: TWO GRATINGS" << std::endl;
        }
        else if (m_diffractionMethod == DM_2D) {
            std::cout << " diffraction method: 2D" << std::endl;
        }

        std::cout << m_derivationMethod;
        if (m_derivationMethod == DM_FORMULA) {
            std::cout << " derivation method: FORMULA" << std::endl;
        }
        else if (m_derivationMethod == DM_PLOYNOM) {
            std::cout << " derivation method POLYNOM" << std::endl;
        }

        std::cout << m_fullEfficiency;
        if (m_fullEfficiency == FE_OFF) {
            std::cout << " m_fullEfficiency: OFF" << std::endl;
        }
        else if (m_fullEfficiency == FE_ON) {
            std::cout << " m_fullEfficiency: ON" << std::endl;
        }

        std::cout << m_imageType;
        if (m_imageType == IT_POINT2POINT) {
            std::cout << " m_imageType: POINT2POINT" << std::endl;
        }
        else if (m_imageType == IT_ASTIGMATIC2ASTIGMATIC) {
            std::cout << " m_imageType: ASTIGMATIC2ASTIGMATIC" << std::endl;
        }

        std::cout << "VALUES" << std::endl;
        std::cout << "m_alpha0Angle: " << m_alpha0Angle << std::endl;
        std::cout << "m_beta0Angle: " << m_beta0Angle << std::endl;
        std::cout << "m_designAlphaAngle: " << m_designAlphaAngle << std::endl;
        std::cout << "m_designBetaAngle: " << m_designBetaAngle << std::endl;
        std::cout << "m_zOff: " << m_zOff << std::endl;
        std::cout << "m_wavelength: " << m_wavelength << std::endl;
        std::cout << "m_lineDensity: " << m_lineDensity << std::endl;
        std::cout << "m_designOrderOfDiffraction: " << m_designOrderOfDiffraction << std::endl;
        std::cout << "m_orderOfDiffraction: " << m_orderOfDiffraction << std::endl;
        std::cout << "m_designEnergyMounting: " << m_designEnergyMounting << std::endl;
        std::cout << "m_a: " << m_a << std::endl;
        std::cout << "m_elementOffsetZ: " << m_elementOffsetZ << std::endl;
        std::cout << "m_elementOffsetZCalc: " << m_elementOffsetZCalc << std::endl;
        std::cout << "m_calcFresnelZOffset: " << m_calcFresnelZOffset << std::endl;
        std::cout << "m_frenselZOffset: " << m_frenselZOffset << std::endl;


    }

    /**
     * calc alpha (incidence angle) from given mount (deviation or incidence angle given)
     *
     * @params: deviation:      deviation angle between incoming and outgoing ray
     *          incidenceAngle: grazing incidence angle
    */
    // never called??
    void ReflectionZonePlate::calcDesignAlphaAngle(double deviation, double incidenceAngle) {
        //double angle;
        if (m_gratingMount == GM_DEVIATION) {
            focus(deviation); // focus(hv=beamline->getPhotonEnergy, angle, d0=calcDz00, ord=orderofdiff, alpha, beta)
            m_grazingIncidenceAngle = PI / 2 - m_grazingIncidenceAngle;
        }
        else if (m_gratingMount == GM_INCIDENCE) {
            m_grazingIncidenceAngle = incidenceAngle; // m_designAlphaAngle
        }
    }

    // VectorIncidenceMainBeam
    // m_alpha = m_incidenceMainBeamAlphaAngle
    void ReflectionZonePlate::calcAlpha2() { //  grazing or normal?
        double alphaMtest;
        double distance = m_meridionalDistance;
        if (m_elementOffsetType == EZ_MANUAL) {
            m_zOff = m_elementOffsetZ;
        }
        else if (m_elementOffsetType == EZ_BEAMDIVERGENCE) {
            m_zOff = calcZOffset();
        }
        if (distance != 0) {
            m_incidenceMainBeamLength = sqrt(pow(distance, 2) + pow(m_zOff, 2) - 2 * distance * m_zOff * cos(m_grazingIncidenceAngle)); // kosinussatz
            alphaMtest = asin(sin(m_grazingIncidenceAngle) * distance / m_incidenceMainBeamLength); // sinussatz
            if (fabs(distance / sin(alphaMtest) - m_zOff / sin(PI - alphaMtest - m_grazingIncidenceAngle)) <= 1e-05) {
                m_alpha = PI - alphaMtest;
            }
            else {
                m_alpha = alphaMtest;
            }
        }
        else {
            m_incidenceMainBeamLength = 0;
            m_alpha = m_grazingIncidenceAngle;
        }
    }

    void ReflectionZonePlate::calcBeta2() {
        double DZ = (m_designOrderOfDiffraction == 0) ? 0 : calcDz00();
        std::cout << "DZ calcBeta2 " << DZ << std::endl;
        m_beta = acos(cos(m_grazingIncidenceAngle) - m_orderOfDiffraction * m_wavelength * 1e-6 * DZ);
    }

    void ReflectionZonePlate::Illumination() {
        double b = m_meridionalDivergence / 1000;
        double a = m_grazingIncidenceAngle * PI / 180;
        double f = 2 * m_meridionalDistance;
        m_illuminationZ = -f * 1 / tan(b) * sin(a) + 1 / sin(b) * sqrt(pow(f, 2) * (pow(cos(b) * sin(a), 2) + pow(sin(b), 2)));
    }


    double ReflectionZonePlate::calcZOffset() {
        Illumination();
        double f = m_meridionalDistance;
        double a = m_grazingIncidenceAngle;
        double IllZ = m_illuminationZ;
        double sq1 = pow(-4 * f * IllZ * cos(a) + 4 * pow(f, 2) + pow(IllZ, 2), -0.5);
        double sq2 = pow(4 * f * IllZ * cos(a) + 4 * pow(f, 2) + pow(IllZ, 2), -0.5);
        double illuminationZoffset;
        if (f == 0 || a == 0) {
            illuminationZoffset = 0;
        }
        else {
            illuminationZoffset = -(f * pow(1 - pow(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a)), 2) *
                pow(pow(fabs(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a))), 2) + 4 * pow(f, 2) * pow(sq1 + sq2, 2) * pow(sin(a), 2), -1), -0.5) *
                sin(a - acos((-(sq1 * (IllZ - 2 * f * cos(a))) + sq2 * (IllZ + 2 * f * cos(a))) *
                    pow(pow(fabs(sq1 * (IllZ - 2 * f * cos(a)) - sq2 * (IllZ + 2 * f * cos(a))), 2) + 4 * pow(f, 2) * pow(sq1 + sq2, 2) * pow(sin(a), 2), -0.5))));
        }
        return illuminationZoffset;
    }

    double ReflectionZonePlate::calcDz00() {
        double wavelength = m_wavelength / 1000 / 1000; // mm
        //double alpha = m_designAlphaAngle;
        //double beta;
        //double zeta;
        //double fresnelZOffset;
        if (m_designType == DT_BETA) {
            //beta = m_designBetaAngle;
            calcFresnelZOffset(); // overwrite given Fresneloffset 
        }
        else if (m_designType == DT_ZOFFSET) {
            //beta = m_betaAngle; // what is this beta angle
            // use given fresnelOffset
        }
        // if imageType == point2pointXstretched ..

        // RAY-UI calls rzpLineDensity function in fortran
        double DZ = rzpLineDensityDZ(0, 0, 0, 0, 1, 0, wavelength);
        return DZ;
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
        m_designOrderOfDiffraction = abs(designOrderOfDiffraction) * presign;
    }

    // never called??
    void ReflectionZonePlate::focus(double angle) {
        // from routine "focus" in RAY.FOR
        // focus(hv=beamline->getPhotonEnergy, angle, d0=calcDz00, ord=orderofdiff, alpha, beta)
        double a = m_designEnergy * abs(calcDz00() * m_orderOfDiffraction * 1.e-06);
        double theta = rad(abs(angle));
        if (angle <= 0) { // constant alpha mounting
            double arg = a - sin(theta);
            if (abs(arg) >= 1) { // cannot calculate alpha & beta
                m_grazingIncidenceAngle = 0;
                m_beta = 0;
            }
            else {
                m_grazingIncidenceAngle = theta;
                m_beta = asin(arg);
            }
        }
        else {  // constant alpha & beta mounting
            theta = theta / 2;
            double arg = a / 2 / cos(theta);
            if (abs(arg) >= 1) {
                m_grazingIncidenceAngle = 0;
                m_beta = 0;
            }
            else {
                m_beta = asin(arg) - theta;
                m_grazingIncidenceAngle = 2 * theta + m_beta;
            }
        }
        m_beta = PI / 2 - abs(m_beta);
    }

    double ReflectionZonePlate::rzpLineDensityDZ(double X, double Y, double Z, double FX, double FY, double FZ, double WL) {
        double s_beta = sin(m_designAlphaAngle);
        double c_beta = cos(m_designBetaAngle);
        double s_alpha = sin(m_designAlphaAngle);
        double c_alpha = cos(m_designBetaAngle);

        double risag = m_sagittalEntranceArmLength;
        double rosag = m_sagittalExitArmLength;
        double rimer = m_meridionalEntranceArmLength;
        double romer = m_meridionalExitArmLength;

        double DZ;//, DX;
        /*if (DERIVATION_METHOD == 1) {
            DX = getLineDensity1d(ptr_dx,x,z);
            DZ = getLineDensity1d(ptr_dz,x,z);
            return;
        }*/
        double xi;
        double yi;
        double zi;
        double xm;
        double ym;
        double zm;

        if (m_imageType == IT_POINT2POINT) { // point to point (standard)
            if (FX == 0 && FZ == 0) { // plane
                zi = -(risag * c_alpha + Z);
                xi = X;
                yi = risag * s_alpha;
                zm = rosag * c_beta - Z;
                xm = X;
                ym = rosag * s_beta;
            }
            else { // more general case, can be reduced to the plane with normal = (0,1,0) and y = 0
                zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
                xi = (FY * X - FX * Y + FX * risag * s_alpha);
                yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
                zm = FX * FZ * X + (FX * FX + FY * FY) * (-Z + rosag * c_beta) + FY * FZ * (Y - rosag * s_beta);
                xm = (FY * X - FX * Y + FX * rosag * s_beta);
                ym = -(FX * X) - FY * Y - FZ * Z + FZ * rosag * c_beta + FY * rosag * s_beta;
            }
        }
        else if (m_imageType == IT_ASTIGMATIC2ASTIGMATIC) { // astigmatic to astigmatix
            double s_rim = rimer < 0 ? -1 : 1;
            double s_rom = romer < 0 ? -1 : 1;
            double c_2alpha = cos(2 * m_designAlphaAngle);
            double c_2beta = cos(2 * m_designBetaAngle);
            if (FX == 0 && FZ == 0) { //   !plane

                zi = s_rim * (rimer * c_alpha + Z);
                xi = (s_rim * X * (c_alpha * Z - 2 * s_alpha * s_alpha * rimer + s_alpha * Y + rimer)) / (c_alpha * Z - 2 * s_alpha * s_alpha * risag + s_alpha * Y + risag);
                yi = s_rim * (-rimer * s_alpha + Y);
                zm = s_rom * (romer * c_beta - Z);
                xm = (s_rom * X * (-c_beta * Z - 2 * s_beta * s_beta * romer + s_beta * Y + romer)) / (c_beta * Z + 2 * s_beta * s_beta * rosag - s_beta * Y - rosag);
                ym = s_rom * (romer * s_beta - Y);
            }
            else {
                double denominator = Z * c_alpha + risag * c_2alpha + Y * s_alpha;
                double nominator = X * (Z * c_alpha + rimer * c_2alpha + Y * s_alpha);
                zi = s_rim * ((FX * FX + FY * FY) * (Z + rimer * c_alpha) - FY * FZ * (Y - rimer * s_alpha) - (FX * FZ * nominator) / denominator);
                xi = s_rim * (-(FX * Y) + FX * rimer * s_alpha + (FY * nominator) / denominator);
                yi = s_rim * (FZ * (Z + rimer * c_alpha) + FY * (Y - rimer * s_alpha) + (FX * nominator) / denominator);

                denominator = (-(Z * c_beta) + rosag * c_2beta + Y * s_beta);
                nominator = X * (-(Z * c_beta) + romer * c_2beta + Y * s_beta);
                zm = s_rom * ((FX * FX + FY * FY) * (-Z + romer * c_beta) + FY * FZ * (Y - romer * s_beta) + (FX * FZ * nominator) / denominator);
                xm = s_rom * (FX * (Y - romer * s_beta) - (FY * nominator) / denominator);
                ym = s_rom * (FZ * (-Z + romer * c_beta) + FY * (-Y + romer * s_beta) - (FX * nominator) / denominator);
            }
            double ris = sqrt(zi * zi + xi * xi + yi * yi);
            double rms = sqrt(zm * zm + xm * xm + ym * ym);

            double ai = zi / ris;
            //double bi = -xi / ris;
            double am = -zm / rms;
            //double bm = xm / rms;

            //double ci = yi/ris; // for what?
            //double cm = -ym/rms;

            DZ = (ai + am) / (WL * m_designOrderOfDiffraction);
            //DX = (-bi - bm) / (WL * m_designOrderOfDiffraction);

            return DZ;
        }

        double ris = sqrt(zi * zi + xi * xi + yi * yi);
        double rms = sqrt(zm * zm + xm * xm + ym * ym);

        //double ai = xi / ris;
        //double am = xm / rms;
        double bi = zi / ris;
        double bm = zm / rms;


        //DX = (ai + am) / (WL * m_designOrderOfDiffraction);
        DZ = (-bi - bm) / (WL * m_designOrderOfDiffraction);

        return DZ;

    }

    void ReflectionZonePlate::setBeta(double beta) { // in degree
        m_beta = rad(beta);
    }

    double ReflectionZonePlate::getWidth() {
        return m_totalWidth;
    }

    double ReflectionZonePlate::getHeight() {
        return m_totalHeight;
    }

    double ReflectionZonePlate::getAlpha() {
        return m_alpha;
    }
    double ReflectionZonePlate::getBeta() {
        return m_beta;
    }
    double ReflectionZonePlate::getDesignAlphaAngle() {
        return m_designAlphaAngle;
    }
    double ReflectionZonePlate::getDesignBetaAngle() {
        return m_designBetaAngle;
    }

    double ReflectionZonePlate::getGratingMount() {
        return m_gratingMount;
    }

    double ReflectionZonePlate::getLongRadius() {
        return m_longRadius;
    }


    double ReflectionZonePlate::getShortRadius() {
        return m_shortRadius;
    }

    double ReflectionZonePlate::getFresnelZOffset() {
        return m_frenselZOffset;
    }

    double ReflectionZonePlate::getCalcFresnelZOffset() {
        return m_calcFresnelZOffset; // calculated if DESIGN_TYPE==DT_BETA
    }

    // input and exit vector lengths
    double ReflectionZonePlate::getSagittalEntranceArmLength() {
        return m_sagittalEntranceArmLength;
    }

    double ReflectionZonePlate::getSagittalExitArmLength() {
        return m_sagittalExitArmLength;
    }

    double ReflectionZonePlate::getMeridionalEntranceArmLength() {
        return m_meridionalEntranceArmLength;
    }

    double ReflectionZonePlate::getMeridionalExitArmLength() {
        return m_meridionalExitArmLength;
    }

    double ReflectionZonePlate::getR1ArmLength() {
        return m_R1ArmLength;
    }

    double ReflectionZonePlate::getR2ArmLength() {
        return m_R2ArmLength; // what is this now??
    }

    double ReflectionZonePlate::getWaveLength() {
        return m_wavelength;
    }
    double ReflectionZonePlate::getDesignEnergy() {
        return m_designEnergy;
    }


    double ReflectionZonePlate::getOrderOfDiffraction() {
        return m_orderOfDiffraction;
    }

    double ReflectionZonePlate::getDesignOrderOfDiffraction() {
        return m_designOrderOfDiffraction;
    }

    double ReflectionZonePlate::getDesignEnergyMounting() {
        return m_designEnergyMounting; // derived from source?
    }

}
