#pragma once

#include <Model/Beamline/EnergyDistribution.h>
#include <Model/Beamline/LightSource.h>
#include <Model/Geometry/Geometry.h>
#include <Tracer/Vulkan/Material.h>
#include <UserParameter/GeometricUserParams.h>

#include <array>
#include <filesystem>
#include <glm.hpp>
#include <vector>

#include "utils.h"

namespace RAYX {
// forward declarations:
enum class CentralBeamstop;
enum class CurvatureType;
enum class CylinderDirection;
enum class FigureRotation;

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

/**
 * Parser gives you useful utility functions to write your own createFromXML
 *functions.
 **/
struct Parser {
    Parser(rapidxml::xml_node<>* node, std::vector<xml::Group> group_context,
           std::filesystem::path rmlFile);

    const char* name();

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
    inline Geometry::GeometricalShape parseGeometricalShape() {
        return static_cast<Geometry::GeometricalShape>(
            parseInt("geometricalShape"));
    }
    inline double parseTotalWidth() { return parseDouble("totalWidth"); }
    inline double parseTotalLength() { return parseDouble("totalLength"); }
    inline double parseTotalHeight() { return parseDouble("totalHeight"); }
    inline double parseAzimuthalAngle() {
        return degToRad(parseDouble("azimuthalAngle"));
    }
    inline double parseGrazingIncAngle() {
        return parseDouble(
            "grazingIncAngle");  // TODO(rudi): is it intentional that we don't
                                 // do degToRad here?
    }
    inline double parseEntranceArmLength() {
        return parseDouble("entranceArmLength");
    }
    inline double parseExitArmLength() { return parseDouble("exitArmLength"); }
    inline double parseEntranceArmLengthMer() {
        return parseDouble("entranceArmLengthMer");
    }
    inline double parseExitArmLengthMer() {
        return parseDouble("exitArmLengthMer");
    }
    inline double parseEntranceArmLengthSag() {
        return parseDouble("entranceArmLengthSag");
    }
    inline double parseExitArmLengthSag() {
        return parseDouble("exitArmLengthSag");
    }
    inline CentralBeamstop parseCentralBeamstop() {
        return static_cast<CentralBeamstop>(parseInt("centralBeamstop"));
    }
    inline double parseTotalWidthStop() {
        return parseDouble("totalWidthStop");
    }
    inline double parseTotalHeightStop() {
        return parseDouble("totalHeightStop");
    }
    inline GratingMount parseGratingMount() {
        return static_cast<GratingMount>(parseInt("gratingMount"));
    }
    inline double parseRadius() { return parseDouble("radius"); }
    inline double parseDesignEnergy() { return parseDouble("designEnergy"); }
    inline double parseLineDensity() { return parseDouble("lineDensity"); }
    inline double parseOrderDiffraction() {
        return parseDouble("orderDiffraction");
    }
    inline double parseDesignEnergyMounting() {
        return parseDouble("designEnergyMounting");
    }
    inline double parseAdditionalOrder() {
        double additionalZeroOrder = 0;

        // may be missing in some RML
        // files, that's fine though
        paramDouble(node, "additionalOrder", &additionalZeroOrder);
        return additionalZeroOrder;
    }

    inline CurvatureType parseCurvatureType() {
        return static_cast<CurvatureType>(parseInt("curvatureType"));
    }

    inline double parseDesignOrderDiffraction() {
        return parseDouble("designOrderDiffraction");
    }
    inline double parseDesignAlphaAngle() {
        return parseDouble("designAlphaAngle");
    }
    inline double parseDesignBetaAngle() {
        return parseDouble("designBetaAngle");
    }
    inline double parseShortRadius() { return parseDouble("shortRadius"); }
    inline double parseLongRadius() { return parseDouble("longRadius"); }
    inline double parseFresnelZOffset() {
        return parseDouble("FresnelZOffset");
    }
    inline CylinderDirection parseBendingRadius() {
        return static_cast<CylinderDirection>(parseInt("bendingRadius"));
    }
    inline double parseParameterA11() { return parseDouble("parameter_a11"); }
    inline FigureRotation parseFigureRotation() {
        return static_cast<FigureRotation>(parseInt("figureRotation"));
    }
    // TODO: Are values stored as 0.0 if set to AUTO?[RAY-UI]
    inline double parseDesignGrazingIncAngle() {
        return parseDouble("designGrazingIncAngle");
    }
    inline double parseLongHalfAxisA() { return parseDouble("longHalfAxisA"); }
    inline double parseShortHalfAxisB() {
        return parseDouble("shortHalfAxisB");
    }
    // if old ray ui file, need to recalculate position and orientation because
    // those in rml file are wrong. not necessary when our recalculated position
    // and orientation is stored
    inline double parseDistancePreceding() {
        return parseDouble("distancePreceding");
    }
    inline int parseMisalignmentCoordinateSystem() {
        return parseInt("misalignmentCoordinateSystem");
    }

    rapidxml::xml_node<>* node;
    std::vector<xml::Group> group_context;
    std::filesystem::path rmlFile;
};

}  // namespace xml
}  // namespace RAYX
