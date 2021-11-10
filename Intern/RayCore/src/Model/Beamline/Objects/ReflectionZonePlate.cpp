#include "ReflectionZonePlate.h"

namespace RAYX
{
    /**
     * calculates transformation matrices,
     * sets parameters for the element in super class (optical element).
     * Sets RZP-specific parameters in this class.
     *
     * @param name                          name of optical element
     * @param geometricalShape              shape of RZP (elliptical vs rectangular)
     * @param curvatureType                 Plane, Sphere, Toroid
     * @param width                         total width of the element (x-dimension)
     * @param height                        height of the element (z- dimensions)
     * @param position                      position of the RZP in world coordinates
     * @param orientation                   orientation of the RZP in world coordinates
     * @param designEnergy                  given by user, used if auto==false
     * @param orderOfDiffraction            in what order the ray should be reflected
     * @param designEnergy                  energy for which the RZP is designed or the source energy
     * @param orderOfDiffraction            order of diffraction that should be traced
     * @param designOrderOfDiffraction      order of diffraction for which the RZP is designed
     * @param dAlpha                        incidence angle for which the RZP is designed
     * @param dBeta                         exit angle for which the RZP is designed
     * @param mEntrance
     * @param mExit
     * @param sEntrance
     * @param sExit
     * @param shortRadius                   short radius of toroid
     * @param longRadius                    radius of sphere or long radius of toroid
     * @param additionalZeroOrder           if true half of the rays will be refracted in the 0th order (=reflection), if false all will be refracted according to orderOfDiffraction Parameter
     * @param fresnelZOffset
     * @param slopeError                    7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
    */
    ReflectionZonePlate::ReflectionZonePlate(const char* name, Geometry::GeometricalShape geometricalShape, const int curvatureType,
        const double width, const double height, const glm::dvec4 position, const glm::dmat4x4 orientation, const double designEnergy, const double orderOfDiffraction,
        const double designOrderOfDiffraction, const double dAlpha, const double dBeta, const double mEntrance, const double mExit, const double sEntrance,
        const double sExit, const double shortRadius, const double longRadius, const int additionalZeroOrder,
        const double fresnelZOffset, const std::vector<double> slopeError)
        : OpticalElement(name, geometricalShape, width, height, position, orientation, slopeError),
        m_fresnelZOffset(fresnelZOffset),
        m_designAlphaAngle(degToRad(dAlpha)),
        m_designBetaAngle(degToRad(dBeta)),
        m_designOrderOfDiffraction(designOrderOfDiffraction),
        m_designEnergy(designEnergy), // in eV
        m_designSagittalEntranceArmLength(sEntrance), //in mm
        m_designSagittalExitArmLength(sExit),
        m_designMeridionalEntranceArmLength(mEntrance),
        m_designMeridionalExitArmLength(mExit),
        m_orderOfDiffraction(orderOfDiffraction)

    {
        // m_designEnergy = designEnergy; // if Auto == true, take energy of Source (param sourceEnergy), else m_designEnergy = designEnergy
        m_designWavelength = m_designEnergy == 0 ? 0 : hvlam(m_designEnergy);
        m_additionalOrder = double(additionalZeroOrder);

        m_curvatureType = curvatureType == 0 ? CT_PLANE : (curvatureType == 1 ? CT_TOROIDAL : CT_SPHERICAL);
        m_designType = DT_ZOFFSET; // DT_ZOFFSET (0) default
        m_derivationMethod = 0; // DM_FORMULA default
        m_rzpType = RT_ELLIPTICAL; // default (0)
        m_imageType = IT_POINT2POINT; // default (0)

        // set parameters in Quadric class
        if (m_curvatureType == CT_PLANE) {
            setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 4, 0, 0, 0 }));
        }
        else if (m_curvatureType == CT_SPHERICAL) {
            m_longRadius = longRadius; // for sphere and toroidal
            setSurface(std::make_unique<Quadric>(std::vector<double>{ 1, 0, 0, 0, 1, 1, 0, -m_longRadius, 0, 0, 1, 0, 4, 0, 0, 0 }));
        }
        else {
            // no structure for non-quadric elements yet
            m_longRadius = longRadius; // for sphere and toroidal
            m_shortRadius = shortRadius; // only for Toroidal
            setSurface(std::make_unique<Toroid>(longRadius, shortRadius));
        }

        printInfo();
        setElementParameters({
            double(m_imageType), double(m_rzpType), double(m_derivationMethod), m_designWavelength,
            double(m_curvatureType), m_designOrderOfDiffraction,m_orderOfDiffraction,m_fresnelZOffset,
            m_designSagittalEntranceArmLength,m_designSagittalExitArmLength,m_designMeridionalEntranceArmLength,m_designMeridionalExitArmLength,
            m_designAlphaAngle,m_designBetaAngle,0, double(m_additionalOrder) }
        );
        std::cout << "[RZP]: Created.\n";
    }

    ReflectionZonePlate::ReflectionZonePlate(const char* name, Geometry::GeometricalShape geometricalShape, const int curvatureType, const double widthA, const double widthB, const double height, const glm::dvec4 position, const glm::dmat4x4 orientation, const double designEnergy, const double orderOfDiffraction, const double designOrderOfDiffraction, const double dAlpha, const double dBeta, const double mEntrance, const double mExit, const double sEntrance, const double sExit, const double shortRadius, const double longRadius, const int additionalZeroOrder, const double fresnelZOffset, const std::vector<double> slopeError)
        : OpticalElement(name, geometricalShape, widthA, widthB, height, position, orientation, slopeError),
        m_fresnelZOffset(fresnelZOffset),
        m_designAlphaAngle(degToRad(dAlpha)),
        m_designBetaAngle(degToRad(dBeta)),
        m_designOrderOfDiffraction(designOrderOfDiffraction),
        m_designEnergy(designEnergy), // in eV
        m_designSagittalEntranceArmLength(sEntrance), //in mm
        m_designSagittalExitArmLength(sExit),
        m_designMeridionalEntranceArmLength(mEntrance),
        m_designMeridionalExitArmLength(mExit),
        m_orderOfDiffraction(orderOfDiffraction)

    {
        // m_designEnergy = designEnergy; // if Auto == true, take energy of Source (param sourceEnergy), else m_designEnergy = designEnergy
        m_designWavelength = m_designEnergy == 0 ? 0 : hvlam(m_designEnergy);
        m_additionalOrder = double(additionalZeroOrder);

        m_curvatureType = curvatureType == 0 ? CT_PLANE : (curvatureType == 1 ? CT_TOROIDAL : CT_SPHERICAL);
        m_designType = DT_ZOFFSET; // DT_ZOFFSET (0) default
        m_derivationMethod = 0; // DM_FORMULA default
        m_rzpType = RT_ELLIPTICAL; // default (0)
        m_imageType = IT_POINT2POINT; // default (0)

        // set parameters in Quadric class
        if (m_curvatureType == CT_PLANE) {
            setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 4, 0, 0, 0 }));
        }
        else if (m_curvatureType == CT_SPHERICAL) {
            m_longRadius = longRadius; // for sphere and toroidal
            setSurface(std::make_unique<Quadric>(std::vector<double>{ 1, 0, 0, 0, 1, 1, 0, -m_longRadius, 0, 0, 1, 0, 4, 0, 0, 0 }));
        }
        else {
            // no structure for non-quadric elements yet
            m_longRadius = longRadius; // for sphere and toroidal
            m_shortRadius = shortRadius; // only for Toroidal
            setSurface(std::make_unique<Toroid>(longRadius, shortRadius));
        }

        printInfo();
        setElementParameters({
            double(m_imageType), double(m_rzpType), double(m_derivationMethod), m_designWavelength,
            double(m_curvatureType), m_designOrderOfDiffraction,m_orderOfDiffraction,m_fresnelZOffset,
            m_designSagittalEntranceArmLength,m_designSagittalExitArmLength,m_designMeridionalEntranceArmLength,m_designMeridionalExitArmLength,
            m_designAlphaAngle,m_designBetaAngle,0, double(m_additionalOrder) }
        );
        std::cout << "[RZP]: Created.\n";
    }


    ReflectionZonePlate::~ReflectionZonePlate()
    {
    }

    std::shared_ptr<ReflectionZonePlate> ReflectionZonePlate::createFromXML(rapidxml::xml_node<>* node) {
        const char* name = node->first_attribute("name")->value();

        int gs;
        if (!xml::paramInt(node, "geometricalShape", &gs)) { return nullptr; }
        Geometry::GeometricalShape geometricalShape = static_cast<Geometry::GeometricalShape>(gs); // HACK(Jannis): convert to enum

        int curvatureType;
        if (!xml::paramInt(node, "curvatureType", &curvatureType)) { return nullptr; }

        double widthA;
        if (!xml::paramDouble(node, "totalWidth", &widthA)) { return nullptr; }

        double height;
        if (!xml::paramDouble(node, "totalLength", &height)) { return nullptr; }

        glm::dvec4 position;
        if (!xml::paramPosition(node, &position)) { return nullptr; }

        glm::dmat4x4 orientation;
        if (!xml::paramOrientation(node, &orientation)) { return nullptr; }

        double designEnergy;
        if (!xml::paramDouble(node, "designEnergy", &designEnergy)) { return nullptr; }

        double orderOfDiffraction;
        if (!xml::paramDouble(node, "orderDiffraction", &orderOfDiffraction)) { return nullptr; }

        double designOrderOfDiffraction;
        if (!xml::paramDouble(node, "designOrderDiffraction", &designOrderOfDiffraction)) { return nullptr; }

        double dAlpha;
        if (!xml::paramDouble(node, "designAlphaAngle", &dAlpha)) { return nullptr; }

        double dBeta;
        if (!xml::paramDouble(node, "designBetaAngle", &dBeta)) { return nullptr; }

        double mEntrance;
        if (!xml::paramDouble(node, "entranceArmLengthMer", &mEntrance)) { return nullptr; }

        double mExit;
        if (!xml::paramDouble(node, "exitArmLengthMer", &mExit)) { return nullptr; }

        double sEntrance;
        if (!xml::paramDouble(node, "entranceArmLengthSag", &sEntrance)) { return nullptr; }

        double sExit;
        if (!xml::paramDouble(node, "exitArmLengthSag", &sExit)) { return nullptr; }

        double shortRadius;
        if (!xml::paramDouble(node, "shortRadius", &shortRadius)) { return nullptr; }

        double longRadius;
        if (!xml::paramDouble(node, "longRadius", &longRadius)) { return nullptr; }

        int additionalZeroOrder = 0;
        xml::paramInt(node, "additionalOrder", &additionalZeroOrder); // may be missing in some RML files, that's fine though

        double fresnelZOffset;
        if (!xml::paramDouble(node, "FresnelZOffset", &fresnelZOffset)) { return nullptr; }

        std::vector<double> slopeError;
        if (!xml::paramSlopeError(node, &slopeError)) { return nullptr; }

        // ! temporary for testing trapezoid rzp
        double widthB;
        bool foundWidthB = xml::paramDouble(node, "totalWidthB", &widthB);
        if (foundWidthB) {
            return std::make_shared<ReflectionZonePlate>(name, geometricalShape, curvatureType,
                widthA, height, position, orientation, designEnergy, orderOfDiffraction,
                designOrderOfDiffraction, dAlpha, dBeta, mEntrance, mExit, sEntrance,
                sExit, shortRadius, longRadius, additionalZeroOrder,
                fresnelZOffset, slopeError
                );
        }
        else {
            return std::make_shared<ReflectionZonePlate>(name, geometricalShape, curvatureType,
                widthA, widthB, height, position, orientation, designEnergy, orderOfDiffraction,
                designOrderOfDiffraction, dAlpha, dBeta, mEntrance, mExit, sEntrance,
                sExit, shortRadius, longRadius, additionalZeroOrder,
                fresnelZOffset, slopeError
                );
        }
    }

    void ReflectionZonePlate::printInfo() const
    {
        std::cout.precision(17);

        std::cout << "[RZP]: " << m_rzpType;
        if (m_rzpType == RT_ELLIPTICAL) {
            std::cout << "type: ELLIPTICAL" << std::endl;
        }
        else if (m_rzpType == RT_MERIODIONAL) {
            std::cout << "type: MERIDIONAL" << std::endl;
        }

        std::cout << "[RZP]: " << m_designType;
        if (m_curvatureType == CT_PLANE) {
            std::cout << " curvature type: PLANE" << std::endl;
        }
        else if (m_curvatureType == CT_SPHERICAL) {
            std::cout << " curvature type: SPHERICAL" << std::endl;
        }
        else if (m_curvatureType == CT_TOROIDAL) {
            std::cout << " curvature type: TOROIDAL" << std::endl;
        }

        std::cout << "[RZP]: " << m_imageType;
        if (m_imageType == IT_POINT2POINT) {
            std::cout << " m_imageType: POINT2POINT" << std::endl;
        }
        else if (m_imageType == IT_ASTIGMATIC2ASTIGMATIC) {
            std::cout << " m_imageType: ASTIGMATIC2ASTIGMATIC" << std::endl;
        }

        std::cout << "[RZP]:\t VALUES" << std::endl;
        std::cout << "[RZP]:\tm_alpha0Angle: " << m_alpha0Angle << std::endl;
        std::cout << "[RZP]:\tm_beta0Angle: " << m_beta0Angle << std::endl;
        std::cout << "[RZP]:\tm_designAlphaAngle: " << m_designAlphaAngle << std::endl;
        std::cout << "[RZP]:\tm_designBetaAngle: " << m_designBetaAngle << std::endl;
        std::cout << "[RZP]:\tm_zOff: " << m_zOff << std::endl;
        std::cout << "[RZP]:\tm_wavelength: " << m_designWavelength << std::endl;
        std::cout << "[RZP]:\tm_lineDensity: " << m_lineDensity << std::endl;
        std::cout << "[RZP]:\tm_designOrderOfDiffraction: " << m_designOrderOfDiffraction << std::endl;
        std::cout << "[RZP]:\tm_orderOfDiffraction: " << m_orderOfDiffraction << std::endl;
        std::cout << "[RZP]:\tm_designEnergyMounting: " << m_designEnergyMounting << std::endl;
        std::cout << "[RZP]:\tm_fresnelZOffset: " << m_fresnelZOffset << std::endl;


    }

    /**
     * needed for caclulating incidence and exit angle
     */
    void ReflectionZonePlate::Illumination() {
        double b = m_meridionalDivergence / 1000;
        double a = m_grazingIncidenceAngle * PI / 180;
        double f = 2 * m_meridionalDistance;
        m_illuminationZ = -f * 1 / tan(b) * sin(a) + 1 / sin(b) * sqrt(pow(f, 2) * (pow(cos(b) * sin(a), 2) + pow(sin(b), 2)));
    }

    /**
     * needed for calculating incidence and exit angle
     */
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

    /**
     * needed for calculating exit angle beta
     */
    double ReflectionZonePlate::calcDz00() {
        double wavelength = m_designWavelength / 1000 / 1000; // mm
        //double alpha = m_designAlphaAngle;
        //double beta;
        //double zeta;
        //double fresnelZOffset;
        if (m_designType == DT_BETA) {
            //beta = m_designBetaAngle;
            calcFresnelZOffset(); // overwrite given Fresneloffset 
        }
        //else { // if(m_designType == DT_ZOFFSET) {
            //beta = m_betaAngle; // what is this beta angle
            // use given fresnelOffset
        //}
        // if imageType == point2pointXstretched ..

        // RAYX-UI calls rzpLineDensity function in fortran
        double DZ = rzpLineDensityDZ(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), wavelength);
        return DZ;
    }

    /**
     * Calculate fresnel z offset if DESIGN_TYPE == BETA from angle beta.
     * @param:
    */
    void ReflectionZonePlate::calcFresnelZOffset() {
        double betaAngle;
        if (m_designType == DT_BETA) {
            betaAngle = m_designBetaAngle;
        }
        m_betaAngle = betaAngle;
        double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle);
        double ROcosb = m_designSagittalExitArmLength * cos(m_betaAngle);
        double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle);
        double ROsinb = m_designSagittalExitArmLength * sin(m_betaAngle);
        double tanTheta = (RIsina + ROsinb) / (RIcosa + ROcosb);
        m_calcFresnelZOffset = (RIsina / tanTheta) - RIcosa;
    }

    /**
     * Calculate beta (exit angle) if DESIGN_TYPE == ZOFFSET from fresnel z offset
     * analogous to calcFresnelZOffset if design type is beta.
     */
    void ReflectionZonePlate::calcBeta() {
        if (m_designType == DT_ZOFFSET) {
            VectorR2Center();
            if (m_fresnelZOffset != 0) { // m_fresnelZOffset is given by the user as a parameter bc DESIGN_TYPE==DT_ZOFFSET
                m_betaAngle = acos((-m_R2ArmLength * m_R2ArmLength * m_designSagittalExitArmLength * m_designSagittalExitArmLength * m_fresnelZOffset * m_fresnelZOffset) / (2 * m_designSagittalExitArmLength * m_fresnelZOffset));
            }
        }
    }

    /**
     * needed for incidence and exit angle calculation
     */
    void ReflectionZonePlate::VectorR1Center() {
        if (m_designType == DT_ZOFFSET) {
            double param_R1cosZ = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle) * m_fresnelZOffset;
            m_R1ArmLength = sqrt(pow(param_R1cosZ, 2) + pow(m_designSagittalEntranceArmLength * sin(m_designAlphaAngle), 2));
            m_alpha0Angle = acos(param_R1cosZ / m_R1ArmLength);
        }
        else if (m_designType == DT_BETA) {
            double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle);
            double ROcosb = m_designSagittalExitArmLength * cos(m_designBetaAngle);
            double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle);
            double ROsinb = m_designSagittalExitArmLength * sin(m_designBetaAngle);
            double m_alpha0Angle = (RIsina + ROsinb) / (RIcosa + ROcosb);
            m_R1ArmLength = RIsina / sin(m_alpha0Angle);
        }
    }

    /**
     * needed for incidence and exit angle calculation
     */
    void ReflectionZonePlate::VectorR2Center() {
        if (m_designType == DT_ZOFFSET) {
            VectorR1Center();
            double R2s = m_designSagittalExitArmLength;
            double alpha = m_alpha0Angle; // why another alpha angle??
            m_R2ArmLength = 0.5 * (-2 * m_fresnelZOffset * cos(alpha) + sqrt(pow(2 * R2s, 2) - 2 * pow(m_fresnelZOffset, 2) + 2 * pow(m_fresnelZOffset, 2) * cos(2 * alpha)));
            m_beta0Angle = m_alpha0Angle;
        }
        else if (m_designType == DT_BETA) {
            double RIcosa = m_designSagittalEntranceArmLength * cos(m_designAlphaAngle);
            double ROcosb = m_designSagittalExitArmLength * cos(m_designBetaAngle);
            double RIsina = m_designSagittalEntranceArmLength * sin(m_designAlphaAngle);
            double ROsinb = m_designSagittalExitArmLength * sin(m_designBetaAngle);
            m_beta0Angle = (RIsina + ROsinb) / (RIcosa + ROcosb);
            m_R2ArmLength = ROsinb / sin(m_beta0Angle);
        }
    }

    /**
     * Calculates the design order of diffraction based on the design type (using the design beta angle
     * or the fresnel center offset)
     *
     * @param designOrderOfDiffraction parameter given by user
    */
    void ReflectionZonePlate::calcDesignOrderOfDiffraction(const double designOrderOfDiffraction) {
        int presign;
        if (m_designType == DT_ZOFFSET) {
            presign = (m_designAlphaAngle >= m_designBetaAngle) ? -1 : 1;
        }
        else if (m_designType == DT_BETA) {
            presign = (m_fresnelZOffset >= 0) ? -1 : 1;
        }
        std::cout << "[RZP]: presign: " << presign << std::endl;
        m_designOrderOfDiffraction = abs(designOrderOfDiffraction) * presign;
    } // design order of diffraction is directly given by the user

    /**
     * @brief Needed for calculating incidence and exit angle.
     * Calculates the line density at one specific point (x,y,z) for a given normal at this point and a wavelength
     * @param intersection          coordinates of intersection point
     * @param normal                normal at intersection point
     * @param WL                    wavelength of ray
     * @return line density on RZP in Z direction for given conditions
     */
    double ReflectionZonePlate::rzpLineDensityDZ(glm::dvec3 intersection, glm::dvec3 normal, const double WL) {
        double s_beta = sin(m_designAlphaAngle);
        double c_beta = cos(m_designBetaAngle);
        double s_alpha = sin(m_designAlphaAngle);
        double c_alpha = cos(m_designBetaAngle);

        double risag = m_designSagittalEntranceArmLength;
        double rosag = m_designSagittalExitArmLength;
        double rimer = m_designMeridionalEntranceArmLength;
        double romer = m_designMeridionalExitArmLength;

        double DZ;//, DX;
        double xi;
        double yi;
        double zi;
        double xm;
        double ym;
        double zm;

        if (m_imageType == IT_POINT2POINT) { // point to point (standard)
            if (normal.x == 0 && normal.z == 0) { // plane
                zi = -(risag * c_alpha + intersection.z);
                xi = intersection.x;
                yi = risag * s_alpha;
                zm = rosag * c_beta - intersection.z;
                xm = intersection.x;
                ym = rosag * s_beta;
            }
            else { // more general case, can be reduced to the plane with normal = (0,1,0) and y = 0
                zi = normal.x * normal.z * intersection.x - (normal.x * normal.x + normal.y * normal.y) * (intersection.z + risag * c_alpha) + normal.y * normal.z * (intersection.y - risag * s_alpha);
                xi = (normal.y * intersection.x - normal.x * intersection.y + normal.x * risag * s_alpha);
                yi = -(normal.x * intersection.x) - normal.y * intersection.y - normal.z * intersection.z - normal.z * risag * c_alpha + normal.y * risag * s_alpha;
                zm = normal.x * normal.z * intersection.x + (normal.x * normal.x + normal.y * normal.y) * (-intersection.z + rosag * c_beta) + normal.y * normal.z * (intersection.y - rosag * s_beta);
                xm = (normal.y * intersection.x - normal.x * intersection.y + normal.x * rosag * s_beta);
                ym = -(normal.x * intersection.x) - normal.y * intersection.y - normal.z * intersection.z + normal.z * rosag * c_beta + normal.y * rosag * s_beta;
            }
        }
        else if (m_imageType == IT_ASTIGMATIC2ASTIGMATIC) { // astigmatic to astigmatix
            double s_rim = rimer < 0 ? -1 : 1;
            double s_rom = romer < 0 ? -1 : 1;
            double c_2alpha = cos(2 * m_designAlphaAngle);
            double c_2beta = cos(2 * m_designBetaAngle);
            if (normal.x == 0 && normal.z == 0) { //   !plane

                zi = s_rim * (rimer * c_alpha + intersection.z);
                xi = (s_rim * intersection.x * (c_alpha * intersection.z - 2 * s_alpha * s_alpha * rimer + s_alpha * intersection.y + rimer)) / (c_alpha * intersection.z - 2 * s_alpha * s_alpha * risag + s_alpha * intersection.y + risag);
                yi = s_rim * (-rimer * s_alpha + intersection.y);
                zm = s_rom * (romer * c_beta - intersection.z);
                xm = (s_rom * intersection.x * (-c_beta * intersection.z - 2 * s_beta * s_beta * romer + s_beta * intersection.y + romer)) / (c_beta * intersection.z + 2 * s_beta * s_beta * rosag - s_beta * intersection.y - rosag);
                ym = s_rom * (romer * s_beta - intersection.y);
            }
            else {
                double denominator = intersection.z * c_alpha + risag * c_2alpha + intersection.y * s_alpha;
                double nominator = intersection.x * (intersection.z * c_alpha + rimer * c_2alpha + intersection.y * s_alpha);
                zi = s_rim * ((normal.x * normal.x + normal.y * normal.y) * (intersection.z + rimer * c_alpha) - normal.y * normal.z * (intersection.y - rimer * s_alpha) - (normal.x * normal.z * nominator) / denominator);
                xi = s_rim * (-(normal.x * intersection.y) + normal.x * rimer * s_alpha + (normal.y * nominator) / denominator);
                yi = s_rim * (normal.z * (intersection.z + rimer * c_alpha) + normal.y * (intersection.y - rimer * s_alpha) + (normal.x * nominator) / denominator);

                denominator = (-(intersection.z * c_beta) + rosag * c_2beta + intersection.y * s_beta);
                nominator = intersection.x * (-(intersection.z * c_beta) + romer * c_2beta + intersection.y * s_beta);
                zm = s_rom * ((normal.x * normal.x + normal.y * normal.y) * (-intersection.z + romer * c_beta) + normal.y * normal.z * (intersection.y - romer * s_beta) + (normal.x * normal.z * nominator) / denominator);
                xm = s_rom * (normal.x * (intersection.y - romer * s_beta) - (normal.y * nominator) / denominator);
                ym = s_rom * (normal.z * (-intersection.z + romer * c_beta) + normal.y * (-intersection.y + romer * s_beta) - (normal.x * nominator) / denominator);
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

    double ReflectionZonePlate::getDesignAlphaAngle() const {
        return m_designAlphaAngle;
    }
    double ReflectionZonePlate::getDesignBetaAngle() const {
        return m_designBetaAngle;
    }

    double ReflectionZonePlate::getGratingMount() const {
        return m_gratingMount;
    }

    double ReflectionZonePlate::getLongRadius() const {
        return m_longRadius;
    }


    double ReflectionZonePlate::getShortRadius() const {
        return m_shortRadius;
    }

    double ReflectionZonePlate::getFresnelZOffset() const {
        return m_fresnelZOffset;
    }

    double ReflectionZonePlate::getCalcFresnelZOffset() const {
        return m_calcFresnelZOffset; // calculated if DESIGN_TYPE==DT_BETA
    }

    // input and exit vector lengths
    double ReflectionZonePlate::getSagittalEntranceArmLength() const {
        return m_designSagittalEntranceArmLength;
    }

    double ReflectionZonePlate::getSagittalExitArmLength() const {
        return m_designSagittalExitArmLength;
    }

    double ReflectionZonePlate::getMeridionalEntranceArmLength() const {
        return m_designMeridionalEntranceArmLength;
    }

    double ReflectionZonePlate::getMeridionalExitArmLength() const {
        return m_designMeridionalExitArmLength;
    }

    double ReflectionZonePlate::getR1ArmLength() const {
        return m_R1ArmLength;
    }

    double ReflectionZonePlate::getR2ArmLength() const {
        return m_R2ArmLength;
    }

    double ReflectionZonePlate::getWaveLength() const {
        return m_designWavelength;
    }
    double ReflectionZonePlate::getDesignEnergy() const {
        return m_designEnergy;
    }


    double ReflectionZonePlate::getOrderOfDiffraction() const {
        return m_orderOfDiffraction;
    }

    double ReflectionZonePlate::getDesignOrderOfDiffraction() const {
        return m_designOrderOfDiffraction;
    }

    double ReflectionZonePlate::getDesignEnergyMounting() const {
        return m_designEnergyMounting; // derived from source?
    }

}
