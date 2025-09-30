#include "xml.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <utility>

#include "Angle.h"
#include "Beamline/Beamline.h"
#include "Beamline/EnergyDistribution.h"
#include "Beamline/StringConversion.h"
#include "Debug/Debug.h"
#include "Element/Element.h"
#include "Shader/Constants.h"

namespace RAYX::xml {

// general scope functions:

// The most general param function. It finds a <param> within the parent tag by the name `paramname`,
// and returns it in `out`.
bool param(const rapidxml::xml_node<>* node, const char* paramname, rapidxml::xml_node<>** out) {
    if (!node || !out) { return false; }

    for (rapidxml::xml_node<>* p = node->first_node(); p; p = p->next_sibling()) {
        if (strcmp(p->name(), "param") != 0) { continue; }
        if (strcmp(p->first_attribute("id")->value(), paramname) == 0) {
            *out = p;
            return true;
        }
    }
    return false;
}

// calls `param` and converts to a double.
bool paramDouble(const rapidxml::xml_node<>* node, const char* paramname, double* out) {
    if (!node || !out) { return false; }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) { return false; }

#if defined(WIN32)
    if (sscanf_s(ref->value(), "%le", out) != 1) {
#else
    if (sscanf(ref->value(), "%le", out) != 1) {
#endif
        return false;
    }

    return true;
}

// calls `param` and converts to a int.
bool paramInt(const rapidxml::xml_node<>* node, const char* paramname, int* out) {
    if (!node || !out) { return false; }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) { return false; }
#if defined(WIN32)
    if (sscanf_s(ref->value(), "%d", out) != 1) {
#else
    if (sscanf(ref->value(), "%d", out) != 1) {
#endif
        return false;
    }

    return true;
}

// calls `param` and converts to a string.
bool paramStr(const rapidxml::xml_node<>* node, const char* paramname, const char** out) {
    if (!node || !out) { return false; }

    rapidxml::xml_node<>* ref;
    if (!param(node, paramname, &ref)) { return false; }
    *out = ref->value();
    return true;
}

// calls `param` and converts to a dvec3.
bool paramDvec3(const rapidxml::xml_node<>* node, const char* paramname, glm::dvec3* out) {
    if (!node || !out) { return false; }

    rapidxml::xml_node<>* subnode;
    if (!param(node, paramname, &subnode)) { return false; }

    const char* names[3] = {"x", "y", "z"};
    double* ptrs[3]      = {&out->x, &out->y, &out->z};

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

// loads the "raw" position without considering how the group context affects the position.
// In other words, this is the position "within" the group.
std::optional<glm::dvec4> paramPosition(const rapidxml::xml_node<>* node) {
    if (!node) { return std::nullopt; }

    glm::dvec3 position3;
    if (!xml::paramDvec3(node, "worldPosition", &position3)) { return std::nullopt; }
    return glm::dvec4(position3, 1);
}

std::filesystem::path Parser::parseEnergyDistributionFile() const {
    std::filesystem::path datpath = Parser::parseStr("photonEnergyDistributionFile");
    // Check if the path is empty
    if (datpath.empty()) {
        // Since the photon energy distribution file is optional, return an empty path
        RAYX_VERB << "No photon energy distribution file specified.";
        return std::filesystem::path();  // Or handle as per your application's logic
    }

    std::filesystem::path combinedPath = rmlFile.parent_path() / datpath;
    try {
        combinedPath = std::filesystem::canonical(combinedPath);
    } catch (const std::exception& e) { RAYX_EXIT << "Failed to canonicalize datfile path: " << combinedPath.string() << " -- Error: " << e.what(); }

    RAYX_VERB << "Combined datfile path: " << combinedPath;
    return combinedPath;
}

// analoguous to paramPosition but for orientation.
std::optional<glm::dmat4x4> paramOrientation(const rapidxml::xml_node<>* node) {
    if (!node) { return std::nullopt; }

    glm::dvec3 worldXdirection, worldYdirection, worldZdirection;
    if (!xml::paramDvec3(node, "worldXdirection", &worldXdirection)) { return std::nullopt; }
    if (!xml::paramDvec3(node, "worldYdirection", &worldYdirection)) { return std::nullopt; }
    if (!xml::paramDvec3(node, "worldZdirection", &worldZdirection)) { return std::nullopt; }

#ifdef RAYX_DEBUG_MODE

    // check if vectors are a basis (determinant =/= 0)
    glm::dmat3x3 worldDirections = {worldXdirection, worldYdirection, worldZdirection};
    double determinant           = glm::determinant(worldDirections);

    if (determinant == 0) { RAYX_WARN << "Vectors are not a basis."; }

    // ((v1 x v2) dot v3) > 0 ==> right-handed (else left-handed)
    glm::dvec3 crossProduct = glm::cross(worldXdirection, worldYdirection);
    double dotProduct       = glm::dot(crossProduct, worldZdirection);

    if (dotProduct < 0) { RAYX_WARN << "Coordinate system is not right-handed."; }

#endif

    glm::dmat4x4 orientation;
    orientation[0] = glm::dvec4(worldXdirection, 0);
    orientation[1] = glm::dvec4(worldYdirection, 0);
    orientation[2] = glm::dvec4(worldZdirection, 0);
    orientation[3] = glm::dvec4(0, 0, 0, 1);

    return orientation;
}

bool paramSlopeError(const rapidxml::xml_node<>* node, SlopeError* out) {
    if (!node || !out) { return false; }

    *out = {0, 0, 0, 0, 0, 0, 0};

    rapidxml::xml_node<>* p;
    if (!param(node, "slopeError", &p)) { return false; }

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
    if (!node || !out) { return false; }

    out->fill(0.f);

    rapidxml::xml_node<>* p;
    if (!param(node, "lineSpacing", &p)) { return false; }

    if (strcmp(p->first_attribute("comment")->value(), "variable (VLS)") == 0) {
        // all vls-values will be left at 0 if they are missing.
        // Hence we ignore the return values of the upcoming
        // paramDouble-calls.
        // TODO: Parameter "vlsDefinition" still missing since last rayui update. Not used in rayx
        xml::paramDouble(node, "vlsParameter1", &((*out)[0]));
        xml::paramDouble(node, "vlsParameter2", &((*out)[1]));
        xml::paramDouble(node, "vlsParameter3", &((*out)[2]));
        xml::paramDouble(node, "vlsParameter4", &((*out)[3]));
        xml::paramDouble(node, "vlsParameter5", &((*out)[4]));
        xml::paramDouble(node, "vlsParameter6", &((*out)[5]));
    }

    return true;
}

//multilayer coating
bool paramCoating(const rapidxml::xml_node<>* node, Coating::MultilayerCoating* out) {
    if (!node || !out) {
        return false;
    }

    // Root für die Layer bestimmen: entweder 'node' selbst oder <param id="Coating">
    rapidxml::xml_node<>* layersRoot = nullptr;
    if (node->first_node("layer")) {
        layersRoot = const_cast<rapidxml::xml_node<>*>(node);
    } else {
        if (!param(node, "Coating", &layersRoot)) {
            RAYX_WARN << "Missing <param id='Coating'> for multilayer";
            return false;
        }
    }

    int numLayers = 0;
    int numDefinedLayers = 0;
    for (auto* l = layersRoot->first_node("layer"); l; l = l->next_sibling("layer")) {
        numDefinedLayers++;
    }

    if (!xml::paramInt(node, "numberOfLayers", &numLayers)) {
        numLayers = numDefinedLayers;  // fallback: use number of actually defined layers
    }

    out->numLayers = numLayers;
    if (numLayers <= 0) {
        RAYX_WARN << "Invalid number of layers: " << numLayers << ". Must be greater than 0.";
        return false;
    }

    out->layers.resize(numLayers);
    int i = 0;
    for (auto* layerNode = layersRoot->first_node("layer"); layerNode; layerNode = layerNode->next_sibling("layer"), ++i) {
        Coating::OneCoating& layer = out->layers[i];

        const char* materialStr = nullptr;
        if (auto* m = layerNode->first_attribute("material")) {
            materialStr = m->value();
        } else {
            RAYX_WARN << "Layer " << i + 1 << " is missing a <material> element.";
            return false;
        }
        Material material;
        materialFromString(materialStr, &material);
        layer.material = static_cast<int>(material);

        if (auto* t = layerNode->first_attribute("thickness")) {
            layer.thickness = std::stod(t->value());
        } else {
            RAYX_WARN << "Missing thickness for layer " << (i + 1);
            return false;
        }

        if (auto* r = layerNode->first_attribute("roughness")) {
            layer.roughness = std::stod(r->value());
        } else {
            RAYX_WARN << "Missing roughness for layer " << (i + 1);
            return false;
        }
    }
    if (i < numLayers) {
        // repeat the defined layers to fill the array 
        for (; i < numLayers; ++i) {
            out->layers[i] = out->layers[i % numDefinedLayers];
        }
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
     *          2:SeparateEnergies(Spikes)
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

        } else if (spreadType == SpreadType::SeparateEnergies) {
            int numOfEnergies;
            if (!xml::paramInt(node, "SeparateEnergies", &numOfEnergies)) {
                std::cout << "No Number for Separate Energies in RML File" << std::endl;
                numOfEnergies = 3;
            }
            numOfEnergies = abs(numOfEnergies);
            *out = EnergyDistribution(SeparateEnergies(photonEnergy, energySpread, numOfEnergies));
        } else {
            *out = EnergyDistribution(HardEdge(photonEnergy, energySpread));
        }

        return true;
    } else {
        RAYX_EXIT << "paramEnergyDistribution is not implemented for "
                     "energyDistributionType"
                  << static_cast<int>(energyDistributionType) << "!";
        return false;
    }
}

bool paramElectronEnergyOrientation(const rapidxml::xml_node<>* node, ElectronEnergyOrientation* out) {
    if (!node || !out) { return false; }
    int energyOrientation_int;
    if (!xml::paramInt(node, "electronEnergyOrientation", &energyOrientation_int)) { return false; }
    *out = static_cast<ElectronEnergyOrientation>(energyOrientation_int);

    return false;
}

bool paramSourcePulseType(const rapidxml::xml_node<>* node, SourcePulseType* out) {
    if (!node || !out) { return false; }
    int spreadType_int;
    if (!xml::paramInt(node, "sourcePulseType", &spreadType_int)) { return false; }
    *out = static_cast<SourcePulseType>(spreadType_int);

    return false;
}

bool paramMaterial(const rapidxml::xml_node<>* node, Material* out) {
    if (!node || !out) { return false; }

    int reflType;
    if (paramInt(node, "reflectivityType", &reflType) && reflType == 0) {
        *out = Material::REFLECTIVE;
        return true;
    }

    const char* str;

    if (!paramStr(node, "materialSubstrate", &str) && !paramStr(node, "crystalMaterial", &str) &&
        !paramStr(node, "materialCoating", &str)) {
        return false;
    }

    return materialFromString(str, out);
}

std::optional<Group> parseGroup(rapidxml::xml_node<>* node) {
    if (!node || (std::strcmp(node->name(), "group") != 0)) { return std::nullopt; }

    Group group;

    if (auto position = paramPosition(node); position) {
        group.setPosition(*position);
    } else {
        group.setPosition(glm::dvec4(0, 0, 0, 1));
    }

    if (auto orientation = paramOrientation(node); orientation) {
        group.setOrientation(*orientation);
    } else {
        group.setOrientation(glm::dmat4(1));
    }

    return group;
}

// Parser implementation

Parser::Parser(rapidxml::xml_node<>* node, std::filesystem::path rmlFile) : node(node), rmlFile(std::move(rmlFile)) {}

const char* Parser::name() const { return node->first_attribute("name")->value(); }

ElementType Parser::type() const {
    const char* val = node->first_attribute("type")->value();
    return StringToElementType.at(std::string(val));
}

// parsers for fundamental types
double Parser::parseDouble(const char* paramname) const {
    double d;
    if (!paramDouble(node, paramname, &d)) { RAYX_EXIT << "parseDouble failed for \"" << paramname << "\""; }
    return d;
}

int Parser::parseInt(const char* paramname) const {
    int i;
    if (!paramInt(node, paramname, &i)) { RAYX_EXIT << "parseInt failed for \"" << paramname << "\""; }
    return i;
}

const char* Parser::parseStr(const char* paramname) const {
    const char* s = nullptr;
    if (!paramStr(node, paramname, &s)) { RAYX_EXIT << "parseStr failed for \"" << paramname << "\""; }
    return s;
}

glm::dvec3 Parser::parseDvec3(const char* paramname) const {
    glm::dvec3 v;
    if (!paramDvec3(node, paramname, &v)) { RAYX_EXIT << "parseDvec3 failed for \"" << paramname << "\""; }
    return v;
}

SlopeError Parser::parseSlopeError() const {
    SlopeError x;
    if (!paramSlopeError(node, &x)) { x = {0, 0, 0, 0, 0, 0, 0}; }
    return x;
}

std::array<double, 6> Parser::parseVls() const {
    std::array<double, 6> x{};
    if (!paramVls(node, &x)) { RAYX_EXIT << "parseVls failed"; }
    return x;
}

glm::dvec4 Parser::parsePosition() const {
    auto positionOpt = paramPosition(node);
    if (!positionOpt) { RAYX_EXIT << "parsePosition failed"; }
    return *positionOpt;
}

glm::dmat4x4 Parser::parseOrientation() const {
    auto orientationOpt = paramOrientation(node);
    if (!orientationOpt) { RAYX_EXIT << "parseOrientation failed"; }
    return *orientationOpt;
}

Material Parser::parseMaterial() const {
    Material m;
    if (!paramMaterial(node, &m)) {
        RAYX_VERB << "No material specified in RML file: defaulting to copper!";
        return Material::Cu;
    }
    return m;
}

Coating::MultilayerCoating Parser::parseCoating() const {
    Coating::MultilayerCoating m;
    // get children from param Cotaing 
    
    if (!paramCoating(node, &m)) {
        RAYX_EXIT << "parseCoating failed";
    }
    return m;
}

Cutout Parser::parseCutout(DesignPlane plane, std::string type) const {
    int geom_shape_int;
    if (!paramInt(node, "geometricalShape", &geom_shape_int)) {
        if (type == "ImagePlane") {
            return Cutout::Unlimited{};
        }
        RAYX_EXIT << "geometricalShape missing, but required!";
    }

    auto geom_shape = static_cast<CutoutType>(geom_shape_int);

    auto x = [&] { return parseTotalWidth(); };

    auto z = [&] {
        if (plane == DesignPlane::XY) {
            return parseTotalHeight();
        } else if (plane == DesignPlane::XZ) {
            return parseTotalLength();
        } else {
            RAYX_EXIT << "parseCutout encountered an invalid design plane!";
            return 0.0;
        }
    };

    if (geom_shape == CutoutType::Rect) {
        return Cutout::Rect{
            .m_width = x(),
            .m_length = z(),
        };
    } else if (geom_shape == CutoutType::Elliptical) {
        Cutout::Elliptical elliptical;
        elliptical.m_diameter_x = x();
        elliptical.m_diameter_z = z();
        return elliptical;
    } else if (geom_shape == CutoutType::Trapezoid) {
        Cutout::Trapezoid trapezoid;
        trapezoid.m_widthA = x();
        trapezoid.m_widthB = parseDouble("totalWidthB");
        trapezoid.m_length = z();

        return trapezoid;
    } else {
        RAYX_EXIT << "invalid geometrical shape!";
        return Cutout::Unlimited{};
    }
}

Surface::Quadric Parser::parseQuadricParameters() const {
    Surface::Quadric s;
    s.m_icurv = parseInt("surfaceBending");  // icurv
    s.m_a11   = parseDouble("A11");
    s.m_a12   = parseDouble("A12");
    s.m_a13   = parseDouble("A13");
    s.m_a14   = parseDouble("A14");
    s.m_a22   = parseDouble("A22");
    s.m_a23   = parseDouble("A23");
    s.m_a24   = parseDouble("A24");
    s.m_a33   = parseDouble("A33");
    s.m_a34   = parseDouble("A34");
    s.m_a44   = parseDouble("A44");

    return s;
}

Surface::Cubic Parser::parseCubicParameters() const {
    Surface::Cubic c;
    c.m_icurv = parseInt("surfaceBending");  // icurv
    c.m_a11   = parseDouble("A11");
    c.m_a12   = parseDouble("A12");
    c.m_a13   = parseDouble("A13");
    c.m_a14   = parseDouble("A14");
    c.m_a22   = parseDouble("A22");
    c.m_a23   = parseDouble("A23");
    c.m_a24   = parseDouble("A24");
    c.m_a33   = parseDouble("A33");
    c.m_a34   = parseDouble("A34");
    c.m_a44   = parseDouble("A44");

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
    auto orientation = ElectronEnergyOrientation::Clockwise;  // default initialize, even tho we dont know at this point
    if (!paramElectronEnergyOrientation(node, &orientation)) { return orientation; }

    return orientation;
}

SourcePulseType Parser::parseSourcePulseType() const {
    auto spreadType = SourcePulseType::None;  // default initialize, even tho we dont know at this point
    if (!paramSourcePulseType(node, &spreadType)) { return spreadType; }

    return spreadType;
}

double Parser::parseImageType() const {
    int imageType_int;

    if (!xml::paramInt(node, "imageType", &imageType_int)) { RAYX_EXIT << "Cannot determine image type!"; }

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
