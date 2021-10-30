#include "xml.h"
#include <Model/Beamline/LightSource.h>
#include <cstdio>
#include <cstring>
#include <iostream>

namespace RAYX {
    namespace xml {
        bool param(rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
            for (rapidxml::xml_node<>* p = node->first_node(); p; p = p->next_sibling()) {
                if (strcmp(p->name(), "param")) { continue; }
                if (strcmp(p->first_attribute("id")->value(), paramname) == 0) {
                    *out = p;
                    return true;
                }
            }
            return false;
        }

        bool paramDouble(rapidxml::xml_node<>* node, const char* paramname, double* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            if (sscanf(ref->value(), "%le", out) != 1) { return false; }

            return true;
        }

        bool paramInt(rapidxml::xml_node<>* node, const char* paramname, int* out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            if (sscanf(ref->value(), "%d", out) != 1) { return false; }

            return true;
        }

        bool paramStr(rapidxml::xml_node<>* node, const char* paramname, const char** out) {
            rapidxml::xml_node<>* ref;
            if (!param(node, paramname, &ref)) { return false; }
            *out = ref->value();
            return true;
        }

        bool paramDvec3(rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out) {
            rapidxml::xml_node<>* subnode;
            if (!param(node, paramname, &subnode)) { return false; }

            const char* names[3] = {"x", "y", "z"};
            double* ptrs[3] = {&out->x, &out->y, &out->z};

            for (rapidxml::xml_node<>* p = subnode->first_node(); p; p = p->next_sibling()) {
                for (uint32_t i = 0; i < 3; i++) {
                    if (strcmp(p->name(), names[i]) == 0) {
                        if (sscanf(p->value(), "%le", ptrs[i]) != 1) { return false; }
                        break;
                    }
                }
            }

            return true;
        }

        bool paramMisalignment(rapidxml::xml_node<>* node, std::vector<double>* out) {
            *out = std::vector<double>(6, 0.f);

            rapidxml::xml_node<>* p;
            if (!param(node, "alignmentError", &p)) { return false; }

            if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
                // all misalignment-values will be left at 0 if they are missing.
                // Hence we ignore the return values of the upcoming paramDouble-calls.
                xml::paramDouble(node, "translationXerror", &((*out)[0]));
                xml::paramDouble(node, "translationYerror", &((*out)[1]));
                xml::paramDouble(node, "translationZerror", &((*out)[2]));

                xml::paramDouble(node, "rotationXerror", &((*out)[3]));
                (*out)[3] /= 1000.f; // mrad -> rad conversion

                xml::paramDouble(node, "rotationYerror", &((*out)[4]));
                (*out)[4] /= 1000.f;

                xml::paramDouble(node, "rotationZerror", &((*out)[5]));
                (*out)[5] /= 1000.f;
            }

            return true;
        }

        bool paramPosition(rapidxml::xml_node<>* node, glm::dvec4* out) {
            glm::dvec3 position3;
            if (!xml::paramDvec3(node, "worldPosition", &position3)) { return false; }
            *out = glm::dvec4(position3, 1);

            return true;
        }

        bool paramOrientation(rapidxml::xml_node<>* node, glm::dmat4x4* out) {
            glm::dvec3 worldXdirection, worldYdirection, worldZdirection;
            if (!xml::paramDvec3(node, "worldXdirection", &worldXdirection)) { return false; }
            if (!xml::paramDvec3(node, "worldYdirection", &worldYdirection)) { return false; }
            if (!xml::paramDvec3(node, "worldZdirection", &worldZdirection)) { return false; }

            (*out)[0] = glm::dvec4(worldXdirection, 0);
            (*out)[1] = glm::dvec4(worldYdirection, 0);
            (*out)[2] = glm::dvec4(worldZdirection, 0);
            (*out)[3] = glm::dvec4(0, 0, 0, 1);

            return true;
        }

        bool paramSlopeError(rapidxml::xml_node<>* node, std::vector<double>* out) {
            *out = std::vector<double>(7, 0.f);

            rapidxml::xml_node<>* p;
            if (!param(node, "slopeError", &p)) { return false; }

            if (strcmp(p->first_attribute("comment")->value(), "Yes") == 0) {
                // all slopeError-values will be left at 0 if they are missing.
                // Hence we ignore the return values of the upcoming paramDouble-calls.
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

        bool paramVls(rapidxml::xml_node<>* node, std::vector<double>* out) {
            *out = std::vector<double>(6, 0.f);

            rapidxml::xml_node<>* p;
            if (!param(node, "lineSpacing", &p)) { return false; }

            if (strcmp(p->first_attribute("comment")->value(), "variable (VLS)") == 0) {
                // all vls-values will be left at 0 if they are missing.
                // Hence we ignore the return values of the upcoming paramDouble-calls.
                xml::paramDouble(node, "vlsParameterB2", &((*out)[0]));
                xml::paramDouble(node, "vlsParameterB3", &((*out)[1]));
                xml::paramDouble(node, "vlsParameterB4", &((*out)[2]));
                xml::paramDouble(node, "vlsParameterB5", &((*out)[3]));
                xml::paramDouble(node, "vlsParameterB6", &((*out)[4]));
                xml::paramDouble(node, "vlsParameterB7", &((*out)[5]));
            }

            return true;
        }

        bool paramEnergyDistribution(rapidxml::xml_node<>* node, EnergyDistribution* out) {
            int energyDistributionType;
            if (!xml::paramInt(node, "energyDistributionType", &energyDistributionType)) { return false; }

            if (energyDistributionType == ET_FILE) {
                    std::cerr << "unimplemented!\n"; //TODO(rudi)
                    return false;
            } else if (energyDistributionType == ET_VALUES) {
                    int spreadType;
                    if (!xml::paramInt(node, "energySpreadType", &spreadType)) { return false; }

                    double photonEnergy;
                    if (!xml::paramDouble(node, "photonEnergy", &photonEnergy)) { return false; }

                    double energySpread;
                    if (!xml::paramDouble(node, "energySpread", &energySpread)) { return false; }

                    bool continuous = !spreadType; // spreadType = 0 -> white band (meaning continuous), spreadType = 1 -> three energies (meaning discrete)

                    *out = EnergyDistribution(EnergyRange(photonEnergy, energySpread), continuous);

                    return true;
            } else {
                    std::cerr << "paramEnergyDistribution is not implemented for spreadType " << energyDistributionType << "!\n";
                    return false;
            }
        }
    }
}