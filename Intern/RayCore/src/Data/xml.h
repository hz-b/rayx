#pragma once

#include <Model/Beamline/EnergyDistribution.h>
#include <Model/Beamline/LightSource.h>
#include <Tracer/Vulkan/Material.h>

#include <array>
#include <filesystem>
#include <glm.hpp>
#include <vector>

#include "utils.h"

namespace RAYX {
/** The xml namespace defines functions, which help to implement the
 * createFromXML-functions for the beamline objects. All of these functions
 * return a boolean indicating whether they were successful. In-case of success
 * the output will be written into the `out` argument.
 */
namespace xml {

/** a representation of a <group>-tag used in parsing. */
struct Group {
    glm::dvec4 m_position;
    glm::dmat4x4 m_orientation;
};

/**
 * Parser gives you useful utility functions to write your own createFromXML
 *functions.
 **/
struct Parser {
    Parser(rapidxml::xml_node<>* node, std::vector<xml::Group> group_context,
           std::filesystem::path rmlFile);

    std::string name();

    // parsers for fundamental types
    double parseDouble(const char* paramname);
    int parseInt(const char* paramname);
    const char* parseStr(const char* paramname);
    glm::dvec3 parseDvec3(const char* paramname);

    // parsers for derived parameters
    std::array<double, 6> parseMisalignment();
    std::array<double, 7> parseSlopeError();
    std::array<double, 6> parseVls();
    EnergyDistribution parseEnergyDistribution();
    glm::dvec4 parsePosition();
    glm::dmat4x4 parseOrientation();
    Material parseMaterial();

    // parsers for trivial derived parameters
    inline int parseNumberRays() { return parseInt("numberRays"); }
    inline double parseSourceWidth() { return parseDouble("sourceWidth"); }
    inline double parseSourceHeight() { return parseDouble("sourceHeight"); }
    inline double parseSourceDepth() { return parseDouble("sourceDepth"); }
    inline double parseHorDiv() { return parseDouble("horDiv") / 1000.0; }
    inline double parseVerDiv() { return parseDouble("verDiv") / 1000.0; }
    inline SourceDist parseSourceWidthDistribution() {
        return static_cast<SourceDist>(parseInt("sourceWidthDistribution"));
    }
    inline SourceDist parseSourceHeightDistribution() {
        return static_cast<SourceDist>(parseInt("sourceHeightDistribution"));
    }
    inline SourceDist parseHorDivDistribution() {
        return static_cast<SourceDist>(parseInt("horDivDistribution"));
    }
    inline SourceDist parseVerDivDistribution() {
        return static_cast<SourceDist>(parseInt("verDivDistribution"));
    }
    inline int parseLinearPol0() { return parseInt("linearPol_0"); }
    inline int parseLinearPol45() { return parseInt("linearPol_45"); }
    inline int parseCircularPol() { return parseInt("circularPol"); }

    rapidxml::xml_node<>* node;
    std::vector<xml::Group> group_context;
    std::filesystem::path rmlFile;
};

// These functions get a `paramname` argument and look for <param
// id="`paramname`">...</param> entries in the XML node to then return it's
// content in the out-argument.
bool param(const rapidxml::xml_node<>* node, const char* paramname,
           rapidxml::xml_node<>** out);
bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname,
                 double* out);
bool paramInt(const rapidxml::xml_node<>* node, const char* paramname,
              int* out);
bool paramStr(const rapidxml::xml_node<>* node, const char* paramname,
              const char** out);
bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname,
                glm::dvec3* out);

// These functions parse more complex parts of beamline objects, and are used by
// multiple createFromXML functions.
bool paramMisalignment(const rapidxml::xml_node<>* node,
                       std::array<double, 6>* out);
bool paramPositionNoGroup(const rapidxml::xml_node<>* node, glm::dvec4* out);
bool paramOrientationNoGroup(const rapidxml::xml_node<>* node,
                             glm::dmat4x4* out);
bool paramSlopeError(const rapidxml::xml_node<>* node,
                     std::array<double, 7>* out);
bool paramVls(const rapidxml::xml_node<>* node, std::array<double, 6>* out);
bool paramEnergyDistribution(const rapidxml::xml_node<>* node,
                             std::filesystem::path rmlFile,
                             EnergyDistribution* out);

bool paramPositionAndOrientation(const rapidxml::xml_node<>* node,
                                 const std::vector<xml::Group>& group_context,
                                 glm::dvec4* out_pos, glm::dmat4x4* out_ori);
bool paramMaterial(const rapidxml::xml_node<>* node, Material* out);

/** node needs to be a <group>-tag, output will be written to `out`. */
bool parseGroup(rapidxml::xml_node<>* node, xml::Group* out);
}  // namespace xml
}  // namespace RAYX
