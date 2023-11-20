#include "xml.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <utility>

#include "Beamline/EnergyDistribution.h"
#include "Beamline/LightSource.h"
#include "Debug/Debug.h"
#include "Shared/Constants.h"
#include "utils.h"

namespace RAYX::xml {

// general scope functions:
bool param(const rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
    if (!node || !out) {
        return false;
    }

    for (rapidxml::xml_node<>* p = node->first_node(); p; p = p->next_sibling()) {
        if (strcmp(p->name(), "param") != 0) {
            continue;
        }
        if (strcmp(p->first_attribute("id")->value(), paramname) == 0) {
            *out = p;
            return true;
        }
    }
    return false;
}

bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname, double* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) {
        return false;
    }

#if defined(WIN32)
    if (sscanf_s(ref->value(), "%le", out) != 1) {
#else
    if (sscanf(ref->value(), "%le", out) != 1) {
#endif
        return false;
    }

    return true;
}

bool paramInt(const rapidxml::xml_node<>* node, const char* paramname, int* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) {
        return false;
    }
#if defined(WIN32)
    if (sscanf_s(ref->value(), "%d", out) != 1) {
#else
    if (sscanf(ref->value(), "%d", out) != 1) {
#endif
        return false;
    }

    return true;
}

bool paramStr(const rapidxml::xml_node<>* node, const char* paramname, const char** out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) {
        return false;
    }
    *out = ref->value();
    return true;
}

bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* subnode;
    if (!param(node, paramname, &subnode)) {
        return false;
    }

    const char* names[3] = {"x", "y", "z"};
    double* ptrs[3] = {&out->x, &out->y, &out->z};

    for (rapidxml::xml_node<>* p = subnode->first_node(); p; p = p->next_sibling()) {
        for (uint32_t i = 0; i < 3; i++) {
            if (strcmp(p->name(), names[i]) == 0) {
#if defined(WIN32)
                if (sscanf_s(p->value(), "%le", ptrs[i]) != 1) {
#else
                if (sscanf(p->value(), "%le", ptrs[i]) != 1) {
#endif
                    return false;
                }
                break;
            }
        }
    }

    return true;
}

bool paramMisalignment(const rapidxml::xml_node<>* node, Misalignment* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* p;

    *out = {0, 0, 0, Rad(0), Rad(0), Rad(0)};

    if (!param(node, "alignmentError", &p)) {
        return true;  // if error is not given, it'll be zero.
    }

    if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
        // all misalignment-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        xml::paramDouble(node, "translationXerror", &out->m_translationXerror);
        xml::paramDouble(node, "translationYerror", &out->m_translationYerror);
        xml::paramDouble(node, "translationZerror", &out->m_translationZerror);


        //keep in mind, rotation on the x-Axis changes the psi and y rotation changes phi
        double x_mrad = 0;
        xml::paramDouble(node, "rotationXerror", &x_mrad);
        out->m_rotationXerror = Rad(x_mrad / 1000.0);  // convert mrad to rad.

        double y_mrad = 0;
        xml::paramDouble(node, "rotationYerror", &y_mrad);
        out->m_rotationYerror = Rad(y_mrad / 1000.0);

        double z_mrad = 0;
        xml::paramDouble(node, "rotationZerror", &z_mrad);
        out->m_rotationZerror = Rad(z_mrad / 1000.0);
    }

    return true;
}

bool paramPositionNoGroup(const rapidxml::xml_node<>* node, glm::dvec4* out) {
    if (!node || !out) {
        return false;
    }

    glm::dvec3 position3;
    if (!xml::paramDvec3(node, "worldPosition", &position3)) {
        return false;
    }
    *out = glm::dvec4(position3, 1);

    return true;
}

bool paramOrientationNoGroup(const rapidxml::xml_node<>* node, glm::dmat4x4* out) {
    if (!node || !out) {
        return false;
    }

    glm::dvec3 worldXdirection, worldYdirection, worldZdirection;
    if (!xml::paramDvec3(node, "worldXdirection", &worldXdirection)) {
        return false;
    }
    if (!xml::paramDvec3(node, "worldYdirection", &worldYdirection)) {
        return false;
    }
    if (!xml::paramDvec3(node, "worldZdirection", &worldZdirection)) {
        return false;
    }

#ifdef RAYX_DEBUG_MODE

    // check if vectors are a basis (determinant =/= 0)
    glm::dmat3x3 worldDirections = {worldXdirection, worldYdirection, worldZdirection};
    double determinant = glm::determinant(worldDirections);

    if (determinant == 0) {
        RAYX_WARN << "Vectors are not a basis.";
    }

    // ((v1 x v2) dot v3) > 0 ==> right-handed (else left-handed)
    glm::dvec3 crossProduct = glm::cross(worldXdirection, worldYdirection);
    double dotProduct = glm::dot(crossProduct, worldZdirection);

    if (dotProduct < 0) {
        RAYX_WARN << "Coordinate system is not right-handed.";
    }

#endif

    (*out)[0] = glm::dvec4(worldXdirection, 0);
    (*out)[1] = glm::dvec4(worldYdirection, 0);
    (*out)[2] = glm::dvec4(worldZdirection, 0);
    (*out)[3] = glm::dvec4(0, 0, 0, 1);

    return true;
}

bool paramSlopeError(const rapidxml::xml_node<>* node, SlopeError* out) {
    if (!node || !out) {
        return false;
    }

    *out = {0, 0, 0, 0, 0, 0, 0};

    rapidxml::xml_node<>* p;
    if (!param(node, "slopeError", &p)) {
        return false;
    }

    if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
        // all slopeError-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        xml::paramDouble(node, "slopeErrorSag", &out->m_sag);
        xml::paramDouble(node, "slopeErrorMer", &out->m_mer);
        xml::paramDouble(node, "thermalDistortionAmp", &out->m_thermalDistortionAmp);
        xml::paramDouble(node, "thermalDistortionSigmaX", &out->m_thermalDistortionSigmaX);
        xml::paramDouble(node, "thermalDistortionSigmaZ", &out->m_thermalDistortionSigmaZ);
        xml::paramDouble(node, "cylindricalBowingAmp", &out->m_cylindricalBowingAmp);
        xml::paramDouble(node, "cylindricalBowingRadius", &out->m_cylindricalBowingRadius);
    }

    return true;
}

bool paramVls(const rapidxml::xml_node<>* node, std::array<double, 6>* out) {
    if (!node || !out) {
        return false;
    }

    out->fill(0.f);

    rapidxml::xml_node<>* p;
    if (!param(node, "lineSpacing", &p)) {
        return false;
    }

    if (strcmp(p->first_attribute("comment")->value(), "variable (VLS)") == 0) {
        // all vls-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        xml::paramDouble(node, "vlsParameterB2", &((*out)[0]));
        xml::paramDouble(node, "vlsParameterB3", &((*out)[1]));
        xml::paramDouble(node, "vlsParameterB4", &((*out)[2]));
        xml::paramDouble(node, "vlsParameterB5", &((*out)[3]));
        xml::paramDouble(node, "vlsParameterB6", &((*out)[4]));
        xml::paramDouble(node, "vlsParameterB7", &((*out)[5]));
    }

    return true;
}

bool paramEnergyDistribution(const rapidxml::xml_node<>* node, const std::filesystem::path& rmlFile, EnergyDistribution* out) {
    if (!node || !out) {
        return false;
    }

    int energyDistributionType_int;
    if (!xml::paramInt(node, "energyDistributionType", &energyDistributionType_int)) {
        return false;
    }
    auto energyDistributionType = static_cast<EnergyDistributionType>(energyDistributionType_int);

    int spreadType_int;
    if (!xml::paramInt(node, "energySpreadType", &spreadType_int)) {
        return false;
    }

    /**
     * a different output is set for all Energy Distribution Types
     *
     * default: 0:HardEdge(WhiteBand)
     *          1:SoftEdge(Energyspread = sigma)
     *          2:SeperateEnergies(Spikes)
     */
    auto spreadType = static_cast<SpreadType>(spreadType_int);

    if (energyDistributionType == EnergyDistributionType::File) {
        const char* filename;
        if (!xml::paramStr(node, "photonEnergyDistributionFile", &filename)) {
            return false;
        }
        std::filesystem::path path = std::filesystem::canonical(rmlFile);
        path.replace_filename(filename);  // this makes the path `filename` be relative to the
                                          // path of the rml file

        DatFile df;
        if (!DatFile::load(path, &df)) {
            return false;
        }
        df.m_continuous = (spreadType == SpreadType::SoftEdge ? true : false);
        *out = EnergyDistribution(df);
        return true;

    } else if (energyDistributionType == EnergyDistributionType::Values) {
        double photonEnergy;
        if (!xml::paramDouble(node, "photonEnergy", &photonEnergy)) {
            return false;
        }

        double energySpread;
        if (!xml::paramDouble(node, "energySpread", &energySpread)) {
            return false;
        }

        if (spreadType == SpreadType::SoftEdge) {
            if (energySpread == 0) {
                energySpread = 1;
            }
            *out = EnergyDistribution(SoftEdge(photonEnergy, energySpread));

        } else if (spreadType == SpreadType::SeperateEnergies) {
            int numOfEnergies;
            if (!xml::paramInt(node, "SeperateEnergies", &numOfEnergies)) {
                std::cout << "No Number for Seperate Energies in RML File" << std::endl;
                numOfEnergies = 3;
            }
            numOfEnergies = abs(numOfEnergies);
            *out = EnergyDistribution(SeperateEnergies(photonEnergy, energySpread, numOfEnergies));
        } else {
            *out = EnergyDistribution(HardEdge(photonEnergy, energySpread));
        }

        return true;
    } else {
        RAYX_ERR << "paramEnergyDistribution is not implemented for "
                    "energyDistributionType"
                 << static_cast<int>(energyDistributionType) << "!";
        return false;
    }
}

bool paramElectronEnergyOrientation(const rapidxml::xml_node<>* node, ElectronEnergyOrientation* out) {
    if (!node || !out) {
        return false;
    }
    int energyOrientation_int;
    if (!xml::paramInt(node, "electronEnergyOrientation", &energyOrientation_int)) {
        return false;
    }
    *out = static_cast<ElectronEnergyOrientation>(energyOrientation_int);

    return false;
}

bool paramSourcePulseType(const rapidxml::xml_node<>* node, SourcePulseType* out) {
    if (!node || !out) {
        return false;
    }
    int spreadType_int;
    if (!xml::paramInt(node, "sourcePulseType", &spreadType_int)) {
        return false;
    }
    *out = static_cast<SourcePulseType>(spreadType_int);

    return false;
}

bool paramPositionAndOrientation(const rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context, glm::dvec4* out_pos,
                                 glm::dmat4x4* out_ori) {
    Misalignment misalignment = {0, 0, 0, Rad(0), Rad(0), Rad(0)};

    // Always returns True!
    paramPositionNoGroup(node, out_pos);
    paramOrientationNoGroup(node, out_ori);
    paramMisalignment(node, &misalignment);

    // TODO: What do we want to do with misalignment?
    // Currently, the world-position & world-orientation as given by the RML file already incorporates the misalignment.

    // This was the old approach: We remove the misalignment from the position&orientation to obtain the non-misaligned position&orientation.

    // glm::dmat4x4 misOrientation = getRotationMatrix(-misalignment.m_rotationXerror.rad, misalignment.m_rotationYerror.rad,
    // misalignment.m_rotationZerror.rad); glm::dvec4 offset = glm::dvec4(misalignment.m_translationXerror, misalignment.m_translationYerror,
    // misalignment.m_translationZerror, 1); *out_pos -= *out_ori * offset; *out_ori = *out_ori * glm::transpose(misOrientation); out_pos->w = 1;

    if (!group_context.empty()) {
        for (auto i = group_context.size(); i-- > 0;) {
            *out_ori = group_context[i].m_orientation * *out_ori;
            *out_pos = group_context[i].m_orientation * *out_pos;
            *out_pos += group_context[i].m_position;  // this gives us w=2!
            out_pos->w = 1;
        }
    }
    return true;
}

bool paramMaterial(const rapidxml::xml_node<>* node, Material* out) {
    if (!node || !out) {
        return false;
    }

    int reflType;
    if (paramInt(node, "reflectivityType", &reflType) && reflType == 0) {
        *out = Material::REFLECTIVE;
        return true;
    }

    const char* str;

    if (!paramStr(node, "materialSubstrate", &str)) {
        return false;
    }

    return materialFromString(str, out);
}

bool parseGroup(rapidxml::xml_node<>* node, xml::Group* out) {
    // default initialization
    out->m_position = glm::vec4(0, 0, 0, 1);
    out->m_orientation = glm::dmat4x4();

    if (strcmp(node->name(), "group") != 0) {
        return false;
    }

    // no return-value checks are done, as groups don't need to alter
    // position or orientation
    paramPositionNoGroup(node, &out->m_position);
    paramOrientationNoGroup(node, &out->m_orientation);

    return true;
}

// Parser implementation

Parser::Parser(rapidxml::xml_node<>* node, std::vector<xml::Group> group_context, std::filesystem::path rmlFile)
    : node(node), group_context(std::move(group_context)), rmlFile(std::move(rmlFile)) {}

const char* Parser::name() const { return node->first_attribute("name")->value(); }

// parsers for fundamental types
double Parser::parseDouble(const char* paramname) const {
    double d;
    if (!paramDouble(node, paramname, &d)) {
        RAYX_ERR << "parseDouble failed for \"" << paramname << "\"";
    }
    return d;
}

int Parser::parseInt(const char* paramname) const {
    int i;
    if (!paramInt(node, paramname, &i)) {
        RAYX_ERR << "parseInt failed for \"" << paramname << "\"";
    }
    return i;
}

const char* Parser::parseStr(const char* paramname) const {
    const char* s;
    if (!paramStr(node, paramname, &s)) {
        RAYX_ERR << "parseStr failed for \"" << paramname << "\"";
    }
    return s;
}

glm::dvec3 Parser::parseDvec3(const char* paramname) const {
    glm::dvec3 v;
    if (!paramDvec3(node, paramname, &v)) {
        RAYX_ERR << "parseDvec3 failed for \"" << paramname << "\"";
    }
    return v;
}

// parsers for derived parameters
Misalignment Parser::parseMisalignment() const {
    Misalignment x;
    if (!paramMisalignment(node, &x)) {
        RAYX_ERR << "parseMisalignment failed";
    }
    return x;
}

SlopeError Parser::parseSlopeError() const {
    SlopeError x;
    if (!paramSlopeError(node, &x)) {
        x = {0, 0, 0, 0, 0, 0, 0};
    }
    return x;
}

std::array<double, 6> Parser::parseVls() const {
    std::array<double, 6> x{};
    if (!paramVls(node, &x)) {
        RAYX_ERR << "parseVls failed";
    }
    return x;
}

EnergyDistribution Parser::parseEnergyDistribution() const {
    EnergyDistribution x;
    if (!paramEnergyDistribution(node, rmlFile, &x)) {
        RAYX_ERR << "parseEnergyDistribution failed";
    }
    return x;
}

glm::dvec4 Parser::parsePosition() const {
    glm::dvec4 x;
    glm::dmat4x4 y;
    if (!paramPositionAndOrientation(node, group_context, &x, &y)) {
        RAYX_ERR << "parsePosition failed";
    }
    return x;
}

glm::dmat4x4 Parser::parseOrientation() const {
    glm::dvec4 x;
    glm::dmat4x4 y;
    if (!paramPositionAndOrientation(node, group_context, &x, &y)) {
        RAYX_ERR << "parseOrientation failed";
    }
    return y;
}

Material Parser::parseMaterial() const {
    Material m;
    if (!paramMaterial(node, &m)) {
        RAYX_VERB << "No material specified in RML file: defaulting to copper!";
        return Material::Cu;
    }
    return m;
}

Cutout Parser::parseCutout(DesignPlane plane) const {
    int geom_shape;
    if (!paramInt(node, "geometricalShape", &geom_shape)) {
        RAYX_ERR << "geometricalShape missing, but required!";
    }

    auto x = [&] { return parseTotalWidth(); };

    auto z = [&] {
        if (plane == DesignPlane::XY) {
            return parseTotalHeight();
        } else if (plane == DesignPlane::XZ) {
            return parseTotalLength();
        } else {
            RAYX_ERR << "parseCutout encountered an invalid design plane!";
            return 0.0;
        }
    };

    if (geom_shape == CTYPE_RECT) {
        RectCutout rect;
        rect.m_width = x();
        rect.m_length = z();
        return serializeRect(rect);
    } else if (geom_shape == CTYPE_ELLIPTICAL) {
        EllipticalCutout elliptical;
        elliptical.m_diameter_x = x();
        elliptical.m_diameter_z = z();
        return serializeElliptical(elliptical);
    } else if (geom_shape == CTYPE_TRAPEZOID) {
        TrapezoidCutout trapezoid;
        trapezoid.m_widthA = x();
        trapezoid.m_widthB = parseDouble("totalWidthB");
        trapezoid.m_length = z();

        return serializeTrapezoid(trapezoid);
    } else {
        RAYX_ERR << "invalid geom_shape!";
        return {0, {0.0, 0.0, 0.0}};
    }
}

QuadricSurface Parser::parseQuadricParameters() const {
    QuadricSurface s;
    //s.m_type = STYPE_QUADRIC;
    s.m_icurv = parseDouble("surfaceBending"); //icurv
    s.m_a11 = parseDouble("A11");
    s.m_a12 = parseDouble("A12");
    s.m_a13 = parseDouble("A13");
    s.m_a14 = parseDouble("A14");
    s.m_a22 = parseDouble("A22");
    s.m_a23 = parseDouble("A23");
    s.m_a24 = parseDouble("A24");
    s.m_a33 = parseDouble("A33");
    s.m_a34 = parseDouble("A34");
    s.m_a44 = parseDouble("A44");

    return s;
}

CubicSurface Parser::parseCubicParameters() const {
    CubicSurface c;
    c.m_icurv = parseDouble("surfaceBending"); //icurv
    c.m_a11 = parseDouble("A11");
    c.m_a12 = parseDouble("A12");
    c.m_a13 = parseDouble("A13");
    c.m_a14 = parseDouble("A14");
    c.m_a22 = parseDouble("A22");
    c.m_a23 = parseDouble("A23");
    c.m_a24 = parseDouble("A24");
    c.m_a33 = parseDouble("A33");
    c.m_a34 = parseDouble("A34");
    c.m_a44 = parseDouble("A44");

    c.m_b12 = parseDouble("B12");
    c.m_b13 = parseDouble("B13");
    c.m_b21 = parseDouble("B21");
    c.m_b23 = parseDouble("B23");
    c.m_b31 = parseDouble("B31");
    c.m_b32 = parseDouble("B32");

    c.m_psi = parseDouble("tangentAngleAlpha") * PI / 180;
    return c;
}

ElectronEnergyOrientation Parser::parseElectronEnergyOrientation() const {
    ElectronEnergyOrientation orientation;
    if (!paramElectronEnergyOrientation(node, &orientation)) {
        return orientation;
    }

    return orientation;
}

SourcePulseType Parser::parseSourcePulseType() const {
    SourcePulseType spreadType;
    if (!paramSourcePulseType(node, &spreadType)) {
        return spreadType;
    }

    return spreadType;
}

double Parser::parseImageType() const {
    int imageType_int;

    if (!xml::paramInt(node, "imageType", &imageType_int)) {
        RAYX_ERR << "Cannot determine image type!";
    }

    return (double)imageType_int;
}

Rad Parser::parseAzimuthalAngle() const {
    double azimuthalAngle = 0;
    paramDouble(node, "azimuthalAngle", &azimuthalAngle);
    return Deg(azimuthalAngle).toRad();
}

double Parser::parseAdditionalOrder() const {
    double additionalZeroOrder = 0;

    // may be missing in some RML
    // files, that's fine though
    paramDouble(node, "additionalOrder", &additionalZeroOrder);
    return additionalZeroOrder;
}

}  // namespace RAYX::xml
