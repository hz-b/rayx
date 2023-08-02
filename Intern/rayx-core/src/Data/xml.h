#pragma once

#include <array>
#include <filesystem>
#include <glm.hpp>
#include <rapidxml.hpp>
#include <vector>

#include "Material/Material.h"
#include "Shared/Constants.h"
#include "utils.h"

namespace RAYX {
class EnergyDistribution;
// forward declarations:
enum class CentralBeamstop;
enum class CurvatureType;
enum class CylinderDirection;
enum class FigureRotation;
enum class GratingMount;
enum class SourceDist;
enum class GeometricalShape;

struct Misalignment {
    double m_translationXerror;
    double m_translationYerror;
    double m_translationZerror;
    Rad m_rotationXerror;
    Rad m_rotationYerror;
    Rad m_rotationZerror;
};

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
bool param(const rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out);
bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname, double* out);
bool paramInt(const rapidxml::xml_node<>* node, const char* paramname, int* out);
bool paramStr(const rapidxml::xml_node<>* node, const char* paramname, const char** out);
bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out);

// These functions parse more complex parts of beamline objects, and are used by
// multiple createFromXML functions.
bool paramMisalignment(const rapidxml::xml_node<>* node, Misalignment* out);
bool paramPositionNoGroup(const rapidxml::xml_node<>* node, glm::dvec4* out);
bool paramOrientationNoGroup(const rapidxml::xml_node<>* node, glm::dmat4x4* out);
bool paramSlopeError(const rapidxml::xml_node<>* node, SlopeError* out);
bool paramVls(const rapidxml::xml_node<>* node, std::array<double, 6>* out);
bool paramEnergyDistribution(const rapidxml::xml_node<>* node, const std::filesystem::path& rmlFile, EnergyDistribution* out);

bool paramPositionAndOrientation(const rapidxml::xml_node<>* node, const std::vector<xml::Group>& group_context, glm::dvec4* out_pos,
                                 glm::dmat4x4* out_ori);
bool paramMaterial(const rapidxml::xml_node<>* node, Material* out);

/** node needs to be a <group>-tag, output will be written to `out`. */
bool parseGroup(rapidxml::xml_node<>* node, xml::Group* out);

/**
 * Parser gives you useful utility functions to write your own createFromXML
 *functions.
 **/
struct Parser {
    Parser(rapidxml::xml_node<>* node, std::vector<xml::Group> group_context, std::filesystem::path rmlFile);

    const char* name() const;

    // parsers for fundamental types
    double parseDouble(const char* paramname) const;
    int parseInt(const char* paramname) const;
    const char* parseStr(const char* paramname) const;
    glm::dvec3 parseDvec3(const char* paramname) const;

    // parsers for derived parameters
    Misalignment parseMisalignment() const;
    SlopeError parseSlopeError() const;
    std::array<double, 6> parseVls() const;
    EnergyDistribution parseEnergyDistribution() const;
    glm::dvec4 parsePosition() const;
    glm::dmat4x4 parseOrientation() const;
    Material parseMaterial() const;
    Cutout parseCutout() const;

    // parsers for trivial derived parameters
    inline int parseNumberRays() const { return parseInt("numberRays"); }
    inline double parseSourceWidth() const { return parseDouble("sourceWidth"); }
    inline double parseSourceHeight() const { return parseDouble("sourceHeight"); }
    inline double parseSourceDepth() const { return parseDouble("sourceDepth"); }
    inline double parseHorDiv() const { return parseDouble("horDiv") / 1000.0; }
    inline double parseVerDiv() const { return parseDouble("verDiv") / 1000.0; }
    inline SourceDist parseSourceWidthDistribution() const { return static_cast<SourceDist>(parseInt("sourceWidthDistribution")); }
    inline SourceDist parseSourceHeightDistribution() const { return static_cast<SourceDist>(parseInt("sourceHeightDistribution")); }
    inline SourceDist parseHorDivDistribution() const { return static_cast<SourceDist>(parseInt("horDivDistribution")); }
    inline SourceDist parseVerDivDistribution() const { return static_cast<SourceDist>(parseInt("verDivDistribution")); }
    inline int parseLinearPol0() const { return parseInt("linearPol_0"); }
    inline int parseLinearPol45() const { return parseInt("linearPol_45"); }
    inline int parseCircularPol() const { return parseInt("circularPol"); }
    inline double parseTotalWidth() const { return parseDouble("totalWidth"); }
    inline double parseTotalLength() const { return parseDouble("totalLength"); }
    inline double parseTotalHeight() const { return parseDouble("totalHeight"); }
    inline Rad parseAzimuthalAngle() const { return Deg(parseDouble("azimuthalAngle")).toRad(); }
    inline Rad parseGrazingIncAngle() const { return Deg(parseDouble("grazingIncAngle")).toRad(); }
    inline double parseEntranceArmLength() const { return parseDouble("entranceArmLength"); }
    inline double parseExitArmLength() const { return parseDouble("exitArmLength"); }
    inline double parseEntranceArmLengthMer() const { return parseDouble("entranceArmLengthMer"); }
    inline double parseExitArmLengthMer() const { return parseDouble("exitArmLengthMer"); }
    inline double parseEntranceArmLengthSag() const { return parseDouble("entranceArmLengthSag"); }
    inline double parseExitArmLengthSag() const { return parseDouble("exitArmLengthSag"); }
    inline CentralBeamstop parseCentralBeamstop() const { return static_cast<CentralBeamstop>(parseInt("centralBeamstop")); }
    inline double parseTotalWidthStop() const { return parseDouble("totalWidthStop"); }
    inline double parseTotalHeightStop() const { return parseDouble("totalHeightStop"); }
    inline GratingMount parseGratingMount() const { return static_cast<GratingMount>(parseInt("gratingMount")); }
    inline double parseRadius() const { return parseDouble("radius"); }
    inline double parseDesignEnergy() const { return parseDouble("designEnergy"); }
    inline double parseLineDensity() const { return parseDouble("lineDensity"); }
    inline double parseOrderDiffraction() const { return parseDouble("orderDiffraction"); }
    inline double parseDesignEnergyMounting() const { return parseDouble("designEnergyMounting"); }
    inline double parseAdditionalOrder() const {
        double additionalZeroOrder = 0;

        // may be missing in some RML
        // files, that's fine though
        paramDouble(node, "additionalOrder", &additionalZeroOrder);
        return additionalZeroOrder;
    }

    inline CurvatureType parseCurvatureType() const { return static_cast<CurvatureType>(parseInt("curvatureType")); }

    inline double parseDesignOrderDiffraction() const { return parseDouble("designOrderDiffraction"); }
    inline Rad parseDesignAlphaAngle() const { return Deg(parseDouble("designAlphaAngle")).toRad(); }
    inline Rad parseDesignBetaAngle() const { return Deg(parseDouble("designBetaAngle")).toRad(); }
    inline double parseShortRadius() const { return parseDouble("shortRadius"); }
    inline double parseLongRadius() const { return parseDouble("longRadius"); }
    inline double parseFresnelZOffset() const { return parseDouble("FresnelZOffset"); }
    inline CylinderDirection parseBendingRadius() const { return static_cast<CylinderDirection>(parseInt("bendingRadius")); }
    inline double parseParameterA11() const { return parseDouble("parameter_a11"); }
    inline FigureRotation parseFigureRotation() const { return static_cast<FigureRotation>(parseInt("figureRotation")); }
    // TODO: Are values stored as 0.0 if set to AUTO?[RAY-UI]
    inline Rad parseDesignGrazingIncAngle() const { return Deg(parseDouble("designGrazingIncAngle")).toRad(); }
    inline double parseLongHalfAxisA() const { return parseDouble("longHalfAxisA"); }
    inline double parseShortHalfAxisB() const { return parseDouble("shortHalfAxisB"); }
    // if old ray ui file, need to recalculate position and orientation because
    // those in rml file are wrong. not necessary when our recalculated position
    // and orientation is stored
    inline double parseDistancePreceding() const { return parseDouble("distancePreceding"); }
    inline int parseMisalignmentCoordinateSystem() const { return parseInt("misalignmentCoordinateSystem"); }

    rapidxml::xml_node<>* node;
    std::vector<xml::Group> group_context;
    std::filesystem::path rmlFile;
};

}  // namespace xml

/// The DesignObject contains the design parameters. But the actual OpticalElements does not contain them.
/// The DesignObject is utilized to construct OpticalElements from it's parameters.
///
/// For now objects can only be created from XML, but later on - when the GUI is around,
/// we might want to add an abstract superclass DesignObject.
/// DesignObject would have child classes xml::Parser and gui::ObjectBuilder or something.
/// This superclass DesignObject could mostly keep the API of the xml::Parser, to make the transition trivial.
using DesignObject = xml::Parser;

}  // namespace RAYX
