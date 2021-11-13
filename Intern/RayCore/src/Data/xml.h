#pragma once

#include <Model/Beamline/EnergyDistribution.h>

#include <glm.hpp>
#include <vector>

#include "rapidxml.hpp"

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
                       std::vector<double>* out);
bool paramPositionNoGroup(const rapidxml::xml_node<>* node, glm::dvec4* out);
bool paramOrientationNoGroup(const rapidxml::xml_node<>* node,
                             glm::dmat4x4* out);
bool paramSlopeError(const rapidxml::xml_node<>* node,
                     std::vector<double>* out);
bool paramVls(const rapidxml::xml_node<>* node, std::vector<double>* out);
bool paramEnergyDistribution(const rapidxml::xml_node<>* node,
                             EnergyDistribution* out);

bool paramPosition(const rapidxml::xml_node<>* node,
                   const std::vector<xml::Group>& group_context,
                   glm::dvec4* out);
bool paramOrientation(const rapidxml::xml_node<>* node,
                      const std::vector<xml::Group>& group_context,
                      glm::dmat4x4* out);

/** node needs to be a <group>-tag, output will be written to `out`. */
bool parseGroup(rapidxml::xml_node<>* node, xml::Group* out);
}  // namespace xml
}  // namespace RAYX
