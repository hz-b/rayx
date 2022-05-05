#include "xml.h"

#include <Debug.h>
#include <Model/Beamline/LightSource.h>

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>

namespace RAYX {
namespace xml {

// general scope functions:
bool param(const rapidxml::xml_node<>* node, const char* paramname,
           rapidxml::xml_node<>** out) {
    if (!node || !out) {
        return false;
    }

    for (rapidxml::xml_node<>* p = node->first_node(); p;
         p = p->next_sibling()) {
        if (strcmp(p->name(), "param")) {
            continue;
        }
        if (strcmp(p->first_attribute("id")->value(), paramname) == 0) {
            *out = p;
            return true;
        }
    }
    return false;
}

bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname,
                 double* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) {
        return false;
    }
    if (sscanf(ref->value(), "%le", out) != 1) {
        return false;
    }

    return true;
}

bool paramInt(const rapidxml::xml_node<>* node, const char* paramname,
              int* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) {
        return false;
    }
    if (sscanf(ref->value(), "%d", out) != 1) {
        return false;
    }

    return true;
}

bool paramStr(const rapidxml::xml_node<>* node, const char* paramname,
              const char** out) {
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

bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname,
                glm::dvec3* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* subnode;
    if (!param(node, paramname, &subnode)) {
        return false;
    }

    const char* names[3] = {"x", "y", "z"};
    double* ptrs[3] = {&out->x, &out->y, &out->z};

    for (rapidxml::xml_node<>* p = subnode->first_node(); p;
         p = p->next_sibling()) {
        for (uint32_t i = 0; i < 3; i++) {
            if (strcmp(p->name(), names[i]) == 0) {
                if (sscanf(p->value(), "%le", ptrs[i]) != 1) {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}

bool paramMisalignment(const rapidxml::xml_node<>* node,
                       std::array<double, 6>* out) {
    if (!node || !out) {
        return false;
    }

    rapidxml::xml_node<>* p;

    out->fill(0.f);

    if (!param(node, "alignmentError", &p)) {
        return true;  // if error is not given, it'll be zero.
    }

    if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
        // all misalignment-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        xml::paramDouble(node, "translationXerror", &((*out)[0]));
        xml::paramDouble(node, "translationYerror", &((*out)[1]));
        xml::paramDouble(node, "translationZerror", &((*out)[2]));

        xml::paramDouble(node, "rotationXerror", &((*out)[3]));
        (*out)[3] /= 1000.f;  // mrad -> rad conversion

        xml::paramDouble(node, "rotationYerror", &((*out)[4]));
        (*out)[4] /= 1000.f;

        xml::paramDouble(node, "rotationZerror", &((*out)[5]));
        (*out)[5] /= 1000.f;
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

bool paramOrientationNoGroup(const rapidxml::xml_node<>* node,
                             glm::dmat4x4* out) {
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

    (*out)[0] = glm::dvec4(worldXdirection, 0);
    (*out)[1] = glm::dvec4(worldYdirection, 0);
    (*out)[2] = glm::dvec4(worldZdirection, 0);
    (*out)[3] = glm::dvec4(0, 0, 0, 1);

    return true;
}

bool paramSlopeError(const rapidxml::xml_node<>* node,
                     std::array<double, 7>* out) {
    if (!node || !out) {
        return false;
    }

    out->fill(0.f);

    rapidxml::xml_node<>* p;
    if (!param(node, "slopeError", &p)) {
        return false;
    }

    if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
        // all slopeError-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        xml::paramDouble(node, "slopeErrorSag", &((*out)[0]));
        xml::paramDouble(node, "slopeErrorMer", &((*out)[1]));
        xml::paramDouble(node, "thermalDistortionAmp", &((*out)[2]));
        xml::paramDouble(node, "thermalDistortionSigmaX", &((*out)[3]));
        xml::paramDouble(node, "thermalDistortionSigmaZ", &((*out)[4]));
        xml::paramDouble(node, "cylindricalBowingAmp", &((*out)[5]));
        xml::paramDouble(node, "cylindricalBowingRadius", &((*out)[6]));
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

bool paramEnergyDistribution(const rapidxml::xml_node<>* node,
                             std::filesystem::path rmlFile,
                             EnergyDistribution* out) {
    if (!node || !out) {
        return false;
    }

    int energyDistributionType_int;
    if (!xml::paramInt(node, "energyDistributionType",
                       &energyDistributionType_int)) {
        return false;
    }
    EnergyDistributionType energyDistributionType =
        static_cast<EnergyDistributionType>(energyDistributionType_int);

    int spreadType_int;
    if (!xml::paramInt(node, "energySpreadType", &spreadType_int)) {
        return false;
    }
    SpreadType spreadType = static_cast<SpreadType>(spreadType_int);

    bool continuous = spreadType == SpreadType::WhiteBand;

    if (energyDistributionType == EnergyDistributionType::File) {
        const char* filename;
        if (!xml::paramStr(node, "photonEnergyDistributionFile", &filename)) {
            return false;
        }
        std::filesystem::path path = std::filesystem::canonical(rmlFile);
        path.replace_filename(
            filename);  // this makes the path `filename` be relative to the
                        // path of the rml file

        DatFile df;
        if (!DatFile::load(path, &df)) {
            return false;
        }

        *out = EnergyDistribution(df, continuous);

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

        *out = EnergyDistribution(EnergyRange(photonEnergy, energySpread),
                                  continuous);

        return true;
    } else {
        RAYX_ERR << "paramEnergyDistribution is not implemented for "
                    "energyDistributionType"
                 << static_cast<int>(energyDistributionType) << "!";
        return false;
    }
}

bool paramPositionAndOrientation(const rapidxml::xml_node<>* node,
                                 const std::vector<xml::Group>& group_context,
                                 glm::dvec4* out_pos, glm::dmat4x4* out_ori) {
    std::array<double, 6> misalignment;
    misalignment.fill(0.f);

    paramPositionNoGroup(node, out_pos);
    paramOrientationNoGroup(node, out_ori);
    paramMisalignment(node, &misalignment);

    glm::dmat4x4 misOrientation =
        getRotationMatrix(-misalignment[3], misalignment[4], misalignment[5]);
    glm::dvec4 offset =
        glm::dvec4(misalignment[0], misalignment[1], misalignment[2], 1);
    // no need to add misalignment again
    if (group_context.size() == 0) {
        return true;
    }
    // remove misalignment from element
    *out_pos -= *out_ori * offset;
    *out_ori = *out_ori * glm::transpose(misOrientation);
    out_pos->w = 1;

    for (unsigned i = group_context.size(); i-- > 0;) {
        *out_ori = group_context[i].m_orientation * *out_ori;
        *out_pos = group_context[i].m_orientation * *out_pos;
        *out_pos += group_context[i].m_position;  // this gives us w=2!
        out_pos->w = 1;
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

Parser::Parser(rapidxml::xml_node<>* node,
               std::vector<xml::Group> group_context,
               std::filesystem::path rmlFile)
    : node(node), group_context(group_context), rmlFile(rmlFile) {}

const char* Parser::name() const {
    return node->first_attribute("name")->value();
}

// parsers for fundamental types
double Parser::parseDouble(const char* paramname) const {
    double d;
    if (!paramDouble(node, paramname, &d)) {
        throw std::runtime_error("parseDouble failed");
    }
    return d;
}

int Parser::parseInt(const char* paramname) const {
    int i;
    if (!paramInt(node, paramname, &i)) {
        throw std::runtime_error("parseInt failed");
    }
    return i;
}

const char* Parser::parseStr(const char* paramname) const {
    const char* s;
    if (!paramStr(node, paramname, &s)) {
        throw std::runtime_error("parseStr failed");
    }
    return s;
}

glm::dvec3 Parser::parseDvec3(const char* paramname) const {
    glm::dvec3 v;
    if (!paramDvec3(node, paramname, &v)) {
        throw std::runtime_error("parseDvec3 failed");
    }
    return v;
}

// parsers for derived parameters
std::array<double, 6> Parser::parseMisalignment() const {
    std::array<double, 6> x;
    if (!paramMisalignment(node, &x)) {
        throw std::runtime_error("parseMisalignment failed");
    }
    return x;
}

std::array<double, 7> Parser::parseSlopeError() const {
    std::array<double, 7> x;
    if (!paramSlopeError(node, &x)) {
        throw std::runtime_error("parseSlopeError failed");
    }
    return x;
}

std::array<double, 6> Parser::parseVls() const {
    std::array<double, 6> x;
    if (!paramVls(node, &x)) {
        throw std::runtime_error("parseVls failed");
    }
    return x;
}

EnergyDistribution Parser::parseEnergyDistribution() const {
    EnergyDistribution x;
    if (!paramEnergyDistribution(node, rmlFile, &x)) {
        throw std::runtime_error("parseEnergyDistribution failed");
    }
    return x;
}

glm::dvec4 Parser::parsePosition() const {
    glm::dvec4 x;
    glm::dmat4x4 y;
    if (!paramPositionAndOrientation(node, group_context, &x, &y)) {
        throw std::runtime_error("parsePosition failed");
    }
    return x;
}

glm::dmat4x4 Parser::parseOrientation() const {
    glm::dvec4 x;
    glm::dmat4x4 y;
    if (!paramPositionAndOrientation(node, group_context, &x, &y)) {
        throw std::runtime_error("parseOrientation failed");
    }
    return y;
}

Material Parser::parseMaterial() const {
    Material m;
    if (!paramMaterial(node, &m)) {
        RAYX_D_LOG
            << "No material specified in RML file: defaulting to copper!";
        return Material::Cu;
    }
    return m;
}

}  // namespace xml
}  // namespace RAYX
