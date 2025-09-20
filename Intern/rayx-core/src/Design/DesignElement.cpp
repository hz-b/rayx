#include "DesignElement.h"

#include <iostream>
#include <memory>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

namespace RAYX {

DesignElement::DesignElement(DesignElement&& other) noexcept { m_elementParameters = std::move(other.m_elementParameters); }

DesignElement& DesignElement::operator=(DesignElement&& other) noexcept {
    m_elementParameters = std::move(other.m_elementParameters);
    return *this;
}

std::unique_ptr<BeamlineNode> DesignElement::clone() const {
    DesignElement clone;
    clone.m_elementParameters = m_elementParameters.clone();
    return std::make_unique<DesignElement>(std::move(clone));
}

OpticalElementAndTransform DesignElement::compile(const glm::dvec4& parentPos, const glm::dmat4& parentOri) const {
    glm::dvec4 worldPos = parentOri * getPosition() + parentPos;
    glm::dmat4 worldOri = parentOri * getOrientation();

    // Create a temporary copy with world instead of local pos/ori
    std::unique_ptr<BeamlineNode> de = this->clone();
    DesignElement* dePtr             = static_cast<DesignElement*>(de.get());
    dePtr->setPosition(worldPos);
    dePtr->setOrientation(worldOri);

    DesignPlane plane = getDesignPlane();

    if (getType() == ElementType::ExpertsMirror) {
        return makeElement(*dePtr, serializeMirror(), makeQuadric(*dePtr), plane);
    } else {
        Surface surface     = makeSurface(*dePtr);
        Behaviour behaviour = makeBehaviour(*dePtr);
        if (getType() == ElementType::Slit) {
            return makeElement(*dePtr, behaviour, surface, plane, {});
        } else if (getType() == ElementType::ImagePlane) {
            return makeElement(*dePtr, behaviour, surface, plane, serializeUnlimited());
        } else {
            return makeElement(*dePtr, behaviour, surface, plane);
        }
    }
}

void DesignElement::setName(std::string s) { m_elementParameters["name"] = s; }
void DesignElement::setType(ElementType s) { m_elementParameters["type"] = s; }

std::string DesignElement::getName() const { return m_elementParameters["name"].as_string(); }
ElementType DesignElement::getType() const { return m_elementParameters["type"].as_elementType(); }

void DesignElement::setPosition(glm::dvec4 p) {
    m_elementParameters["position"]      = Map();
    m_elementParameters["position"]["x"] = p.x;
    m_elementParameters["position"]["y"] = p.y;
    m_elementParameters["position"]["z"] = p.z;
    m_elementParameters["position"]["w"] = p.w;
}

glm::dvec4 DesignElement::getPosition() const {
    glm::dvec4 d;
    d[0] = m_elementParameters["position"]["x"].as_double();
    d[1] = m_elementParameters["position"]["y"].as_double();
    d[2] = m_elementParameters["position"]["z"].as_double();
    d[3] = 0;
    return d;
}

void DesignElement::setOrientation(glm::dmat4x4 o) {
    m_elementParameters["xDirection"]      = Map();
    m_elementParameters["xDirection"]["x"] = o[0][0];
    m_elementParameters["xDirection"]["y"] = o[0][1];
    m_elementParameters["xDirection"]["z"] = o[0][2];
    m_elementParameters["xDirection"]["w"] = o[0][3];

    m_elementParameters["yDirection"]      = Map();
    m_elementParameters["yDirection"]["x"] = o[1][0];
    m_elementParameters["yDirection"]["y"] = o[1][1];
    m_elementParameters["yDirection"]["z"] = o[1][2];
    m_elementParameters["yDirection"]["w"] = o[1][3];

    m_elementParameters["zDirection"]      = Map();
    m_elementParameters["zDirection"]["x"] = o[2][0];
    m_elementParameters["zDirection"]["y"] = o[2][1];
    m_elementParameters["zDirection"]["z"] = o[2][2];
    m_elementParameters["zDirection"]["w"] = o[2][3];
}

glm::dmat4x4 DesignElement::getOrientation() const {
    glm::dmat4x4 o;

    o[0][0] = m_elementParameters["xDirection"]["x"].as_double();
    o[0][1] = m_elementParameters["xDirection"]["y"].as_double();
    o[0][2] = m_elementParameters["xDirection"]["z"].as_double();
    o[0][3] = 0;

    o[1][0] = m_elementParameters["yDirection"]["x"].as_double();
    o[1][1] = m_elementParameters["yDirection"]["y"].as_double();
    o[1][2] = m_elementParameters["yDirection"]["z"].as_double();
    o[1][3] = 0;

    o[2][0] = m_elementParameters["zDirection"]["x"].as_double();
    o[2][1] = m_elementParameters["zDirection"]["y"].as_double();
    o[2][2] = m_elementParameters["zDirection"]["z"].as_double();
    o[2][3] = 0;

    return o;
}

void DesignElement::setSlopeError(SlopeError s) {
    m_elementParameters["SlopeError"]                            = Map();
    m_elementParameters["SlopeError"]["slopeErrorSag"]           = s.m_sag;
    m_elementParameters["SlopeError"]["slopeErrorMer"]           = s.m_mer;
    m_elementParameters["SlopeError"]["thermalDistortionAmp"]    = s.m_thermalDistortionAmp;
    m_elementParameters["SlopeError"]["thermalDistortionSigmaX"] = s.m_thermalDistortionSigmaX;
    m_elementParameters["SlopeError"]["thermalDistortionSigmaZ"] = s.m_thermalDistortionSigmaZ;
    m_elementParameters["SlopeError"]["cylindricalBowingAmp"]    = s.m_cylindricalBowingAmp;
    m_elementParameters["SlopeError"]["cylindricalBowingRadius"] = s.m_cylindricalBowingRadius;
}
SlopeError DesignElement::getSlopeError() const {
    SlopeError s;
    s.m_sag                     = m_elementParameters["SlopeError"]["slopeErrorSag"].as_double();
    s.m_mer                     = m_elementParameters["SlopeError"]["slopeErrorMer"].as_double();
    s.m_thermalDistortionAmp    = m_elementParameters["SlopeError"]["thermalDistortionAmp"].as_double();
    s.m_thermalDistortionSigmaX = m_elementParameters["SlopeError"]["thermalDistortionSigmaX"].as_double();
    s.m_thermalDistortionSigmaZ = m_elementParameters["SlopeError"]["thermalDistortionSigmaZ"].as_double();
    s.m_cylindricalBowingAmp    = m_elementParameters["SlopeError"]["cylindricalBowingAmp"].as_double();
    s.m_cylindricalBowingRadius = m_elementParameters["SlopeError"]["cylindricalBowingRadius"].as_double();

    return s;
}

void DesignElement::setCutout(Cutout c) {
    m_elementParameters["geometricalShape"] = c.m_type;
    if (c.m_type == CutoutType::Rect) {
        RectCutout rect                     = deserializeRect(c);
        m_elementParameters["CutoutWidth"]  = rect.m_width;
        m_elementParameters["CutoutLength"] = rect.m_length;
    } else if (c.m_type == CutoutType::Elliptical) {
        EllipticalCutout elli                  = deserializeElliptical(c);
        m_elementParameters["CutoutDiameterX"] = elli.m_diameter_x;
        m_elementParameters["CutoutDiameterZ"] = elli.m_diameter_z;
    } else if (c.m_type == CutoutType::Trapezoid) {
        TrapezoidCutout trapi               = deserializeTrapezoid(c);
        m_elementParameters["CutoutWidthA"] = trapi.m_widthA;
        m_elementParameters["CutoutWidthB"] = trapi.m_widthB;
        m_elementParameters["CutoutLength"] = trapi.m_length;
    }
}
Cutout DesignElement::getCutout() const {
    Cutout c;

    c.m_type = m_elementParameters["geometricalShape"].as_openingShape();

    if (c.m_type == CutoutType::Rect) {  // Rectangle
        RectCutout rect;
        rect.m_width  = m_elementParameters["CutoutWidth"].as_double();
        rect.m_length = m_elementParameters["CutoutLength"].as_double();
        c             = serializeRect(rect);
    } else if (c.m_type == CutoutType::Elliptical) {  // Ellipsoid
        EllipticalCutout elli;
        elli.m_diameter_x = m_elementParameters["CutoutDiameterX"].as_double();
        elli.m_diameter_z = m_elementParameters["CutoutDiameterZ"].as_double();
        c                 = serializeElliptical(elli);
    } else if (c.m_type == CutoutType::Trapezoid) {  // Trapezoid
        TrapezoidCutout trapi;
        trapi.m_widthA = m_elementParameters["CutoutWidthA"].as_double();
        trapi.m_widthB = m_elementParameters["CutoutWidthB"].as_double();
        trapi.m_length = m_elementParameters["CutoutLength"].as_double();
        c              = serializeTrapezoid(trapi);
    }

    return c;
}

Cutout DesignElement::getGlobalCutout() const { return serializeUnlimited(); }

void DesignElement::setVLSParameters(const std::array<double, 6>& values) {
    m_elementParameters["vlsParams"] = Map();

    m_elementParameters["vlsParams"]["vlsParameterB2"] = values[0];
    m_elementParameters["vlsParams"]["vlsParameterB3"] = values[1];
    m_elementParameters["vlsParams"]["vlsParameterB4"] = values[2];
    m_elementParameters["vlsParams"]["vlsParameterB5"] = values[3];
    m_elementParameters["vlsParams"]["vlsParameterB6"] = values[4];
    m_elementParameters["vlsParams"]["vlsParameterB7"] = values[5];
}

std::array<double, 6> DesignElement::getVLSParameters() const {
    return {m_elementParameters["vlsParams"]["vlsParameterB2"].as_double(), m_elementParameters["vlsParams"]["vlsParameterB3"].as_double(),
            m_elementParameters["vlsParams"]["vlsParameterB4"].as_double(), m_elementParameters["vlsParams"]["vlsParameterB5"].as_double(),
            m_elementParameters["vlsParams"]["vlsParameterB6"].as_double(), m_elementParameters["vlsParams"]["vlsParameterB7"].as_double()};
}

void DesignElement::setExpertsOptics(Surface value) {
    Surface::Quadric qua = variant::get<Surface::Quadric>(value.m_surface);
    m_elementParameters["expertsParams"] = Map();
    m_elementParameters["expertsParams"]["A11"] = qua.m_a11;
    m_elementParameters["expertsParams"]["A12"] = qua.m_a12;
    m_elementParameters["expertsParams"]["A13"] = qua.m_a13;
    m_elementParameters["expertsParams"]["A14"] = qua.m_a14;
    m_elementParameters["expertsParams"]["A22"] = qua.m_a22;
    m_elementParameters["expertsParams"]["A23"] = qua.m_a23;
    m_elementParameters["expertsParams"]["A24"] = qua.m_a24;
    m_elementParameters["expertsParams"]["A33"] = qua.m_a33;
    m_elementParameters["expertsParams"]["A34"] = qua.m_a34;
    m_elementParameters["expertsParams"]["A44"] = qua.m_a44;
}

Surface DesignElement::getExpertsOptics() const {
    Surface::Quadric qua;
    qua.m_a11 = m_elementParameters["expertsParams"]["A11"].as_double();
    qua.m_a12 = m_elementParameters["expertsParams"]["A12"].as_double();
    qua.m_a13 = m_elementParameters["expertsParams"]["A13"].as_double();
    qua.m_a14 = m_elementParameters["expertsParams"]["A14"].as_double();
    qua.m_a22 = m_elementParameters["expertsParams"]["A22"].as_double();
    qua.m_a23 = m_elementParameters["expertsParams"]["A23"].as_double();
    qua.m_a24 = m_elementParameters["expertsParams"]["A24"].as_double();
    qua.m_a33 = m_elementParameters["expertsParams"]["A33"].as_double();
    qua.m_a34 = m_elementParameters["expertsParams"]["A34"].as_double();
    qua.m_a44 = m_elementParameters["expertsParams"]["A44"].as_double();

    return qua;
}

void DesignElement::setExpertsCubic(Surface value) {
    Surface::Cubic cub = variant::get<Surface::Cubic>(value.m_surface);
    m_elementParameters["expertsParams"] = Map();
    m_elementParameters["expertsParams"]["A11"] = cub.m_a11;
    m_elementParameters["expertsParams"]["A12"] = cub.m_a12;
    m_elementParameters["expertsParams"]["A13"] = cub.m_a13;
    m_elementParameters["expertsParams"]["A14"] = cub.m_a14;
    m_elementParameters["expertsParams"]["A22"] = cub.m_a22;
    m_elementParameters["expertsParams"]["A23"] = cub.m_a23;
    m_elementParameters["expertsParams"]["A24"] = cub.m_a24;
    m_elementParameters["expertsParams"]["A33"] = cub.m_a33;
    m_elementParameters["expertsParams"]["A34"] = cub.m_a34;
    m_elementParameters["expertsParams"]["A44"] = cub.m_a44;

    m_elementParameters["expertsParams"]["B12"] = cub.m_b12;
    m_elementParameters["expertsParams"]["B13"] = cub.m_b13;
    m_elementParameters["expertsParams"]["B21"] = cub.m_b21;
    m_elementParameters["expertsParams"]["B23"] = cub.m_b23;
    m_elementParameters["expertsParams"]["B31"] = cub.m_b31;
    m_elementParameters["expertsParams"]["B32"] = cub.m_b32;
}

Surface DesignElement::getExpertsCubic() const {
    Surface::Cubic cub;
    cub.m_a11 = m_elementParameters["expertsParams"]["A11"].as_double();
    cub.m_a12 = m_elementParameters["expertsParams"]["A12"].as_double();
    cub.m_a13 = m_elementParameters["expertsParams"]["A13"].as_double();
    cub.m_a14 = m_elementParameters["expertsParams"]["A14"].as_double();
    cub.m_a22 = m_elementParameters["expertsParams"]["A22"].as_double();
    cub.m_a23 = m_elementParameters["expertsParams"]["A23"].as_double();
    cub.m_a24 = m_elementParameters["expertsParams"]["A24"].as_double();
    cub.m_a33 = m_elementParameters["expertsParams"]["A33"].as_double();
    cub.m_a34 = m_elementParameters["expertsParams"]["A34"].as_double();
    cub.m_a44 = m_elementParameters["expertsParams"]["A44"].as_double();

    cub.m_b12 = m_elementParameters["expertsParams"]["B12"].as_double();
    cub.m_b13 = m_elementParameters["expertsParams"]["B13"].as_double();
    cub.m_b21 = m_elementParameters["expertsParams"]["B21"].as_double();
    cub.m_b23 = m_elementParameters["expertsParams"]["B23"].as_double();
    cub.m_b31 = m_elementParameters["expertsParams"]["B31"].as_double();
    cub.m_b32 = m_elementParameters["expertsParams"]["B32"].as_double();

    return Surface::Cubic{cub};
}

// for the spherical Mirror the radius can be calculated from grazing Inc angle, entrace Armlength and exit Armlength
// copied from RAY-UI
void DesignElement::setCalcRadius() {
    double radius = 2.0 / m_elementParameters["grazingIncAngle"].as_rad().sin() /
                    (1.0 / m_elementParameters["entranceArmLength"].as_double() + 1.0 / m_elementParameters["exitArmLength"].as_double());
    m_elementParameters["radius"] = radius;
}

// for the Spherical Grating the radius is calculated from the deviation angle instead grazing inc angle
// copied from RAY-UI
// TODO: support different types of input Angle : constant inc angle, SMG fix focus
void DesignElement::setCalcRadiusDeviationAngle() {
    double theta = m_elementParameters["deviationAngle"].as_rad().toDeg().deg > 0
                       ? (180 - m_elementParameters["deviationAngle"].as_rad().toDeg().deg) / 2 * PI / 180.0
                       : (90 + m_elementParameters["deviationAngle"].as_rad().toDeg().deg) * PI / 180.0;
    double radius =
        2.0 / sin(theta) / (1.0 / m_elementParameters["entranceArmLength"].as_double() + 1.0 / m_elementParameters["exitArmLength"].as_double());
    m_elementParameters["radius"] = radius;
}

// Azimuthal Angle
void DesignElement::setAzimuthalAngle(Rad r) { m_elementParameters["AzimuthalAngle"] = r; }
Rad DesignElement::getAzimuthalAngle() const { return m_elementParameters["AzimuthalAngle"].as_rad(); }

// Material
void DesignElement::setMaterial(Material m) { m_elementParameters["Material"] = m; }
Material DesignElement::getMaterial() const { return m_elementParameters["Material"].as_material(); }

// Distance Preceding
void DesignElement::setDistancePreceding(double distance) { m_elementParameters["distancePreceding"] = distance; }
double DesignElement::getDistancePreceding() const { return m_elementParameters["distancePreceding"].as_double(); }

// Total Height
void DesignElement::setTotalHeight(double height) { m_elementParameters["totalHeight"] = height; }
double DesignElement::getTotalHeight() const { return m_elementParameters["totalHeight"].as_double(); }

// Opening Shape
void DesignElement::setOpeningShape(CutoutType shape) { m_elementParameters["openingShape"] = shape; }
CutoutType DesignElement::getOpeningShape() const { return m_elementParameters["openingShape"].as_openingShape(); }

// Opening Width
void DesignElement::setOpeningWidth(double width) { m_elementParameters["openingWidth"] = width; }
double DesignElement::getOpeningWidth() const { return m_elementParameters["openingWidth"].as_double(); }

// Opening Height
void DesignElement::setOpeningHeight(double height) { m_elementParameters["openingHeight"] = height; }
double DesignElement::getOpeningHeight() const { return m_elementParameters["openingHeight"].as_double(); }

// Central Beamstop
void DesignElement::setCentralBeamstop(CentralBeamstop value) { m_elementParameters["centralBeamstop"] = value; }
CentralBeamstop DesignElement::getCentralBeamstop() const { return m_elementParameters["centralBeamstop"].as_centralBeamStop(); }

void DesignElement::setStopWidth(double width) { m_elementParameters["stopWidth"] = width; }
double DesignElement::getStopWidth() const { return m_elementParameters["stopWidth"].as_double(); }

void DesignElement::setStopHeight(double height) { m_elementParameters["stopHeight"] = height; }
double DesignElement::getStopHeight() const { return m_elementParameters["stopHeight"].as_double(); }

void DesignElement::setTotalWidth(double width) { m_elementParameters["totalWidth"] = width; }
double DesignElement::getTotalWidth() const { return m_elementParameters["totalWidth"].as_double(); }

void DesignElement::setProfileKind(int value) { m_elementParameters["profileKind"] = value; }
int DesignElement::getProfileKind() const { return m_elementParameters["profileKind"].as_int(); }

void DesignElement::setProfileFile(double filePath) { m_elementParameters["profileFile"] = filePath; }
double DesignElement::getProfileFile() const { return m_elementParameters["profileFile"].as_double(); }

void DesignElement::setTotalLength(double value) { m_elementParameters["totalLength"] = value; }
double DesignElement::getTotalLength() const { return m_elementParameters["totalLength"].as_double(); }

void DesignElement::setGrazingIncAngle(Rad value) { m_elementParameters["grazingIncAngle"] = value; }
Rad DesignElement::getGrazingIncAngle() const { return m_elementParameters["grazingIncAngle"].as_rad(); }

void DesignElement::setDeviationAngle(Rad value) { m_elementParameters["deviationAngle"] = value; }
Rad DesignElement::getDeviationAngle() const { return m_elementParameters["deviationAngle"].as_rad(); }

void DesignElement::setEntranceArmLength(double value) { m_elementParameters["entranceArmLength"] = value; }
double DesignElement::getEntranceArmLength() const { return m_elementParameters["entranceArmLength"].as_double(); }

void DesignElement::setExitArmLength(double value) { m_elementParameters["exitArmLength"] = value; }
double DesignElement::getExitArmLength() const { return m_elementParameters["exitArmLength"].as_double(); }

void DesignElement::setRadiusDirection(CylinderDirection value) { m_elementParameters["bendingRadius"] = value; }
CylinderDirection DesignElement::getRadiusDirection() const { return m_elementParameters["bendingRadius"].as_cylinderDirection(); }

void DesignElement::setRadius(double value) { m_elementParameters["radius"] = value; }
double DesignElement::getRadius() const { return m_elementParameters["radius"].as_double(); }

void DesignElement::setDesignGrazingIncAngle(Rad value) { m_elementParameters["designGrazingIncAngle"] = value; }
Rad DesignElement::getDesignGrazingIncAngle() const { return m_elementParameters["designGrazingIncAngle"].as_rad(); }

void DesignElement::setLongHalfAxisA(double value) { m_elementParameters["longHalfAxisA"] = value; }
double DesignElement::getLongHalfAxisA() const { return m_elementParameters["longHalfAxisA"].as_double(); }

void DesignElement::setShortHalfAxisB(double value) { m_elementParameters["shortHalfAxisB"] = value; }
double DesignElement::getShortHalfAxisB() const { return m_elementParameters["shortHalfAxisB"].as_double(); }

void DesignElement::setParameterA11(double value) { m_elementParameters["parameter_a11"] = value; }
double DesignElement::getParameterA11() const { return m_elementParameters["parameter_a11"].as_double(); }

void DesignElement::setFigureRotation(FigureRotation value) { m_elementParameters["figureRotation"] = value; }
FigureRotation DesignElement::getFigureRotation() const { return m_elementParameters["figureRotation"].as_figureRotation(); }

void DesignElement::setArmLength(double value) { m_elementParameters["armLength"] = value; }
double DesignElement::getArmLength() const { return m_elementParameters["armLength"].as_double(); }

void DesignElement::setParameterP(double value) { m_elementParameters["parameter_P"] = value; }
double DesignElement::getParameterP() const { return m_elementParameters["parameter_P"].as_double(); }

void DesignElement::setParameterPType(double value) { m_elementParameters["parameter_P_type"] = value; }
double DesignElement::getParameterPType() const { return m_elementParameters["parameter_P_type"].as_double(); }

void DesignElement::setLineDensity(double value) { m_elementParameters["lineDensity"] = value; }
double DesignElement::getLineDensity() const { return m_elementParameters["lineDensity"].as_double(); }

void DesignElement::setShortRadius(double value) { m_elementParameters["shortRadius"] = value; }
double DesignElement::getShortRadius() const { return m_elementParameters["shortRadius"].as_double(); }

void DesignElement::setLongRadius(double value) { m_elementParameters["longRadius"] = value; }
double DesignElement::getLongRadius() const { return m_elementParameters["longRadius"].as_double(); }

void DesignElement::setFresnelZOffset(double value) { m_elementParameters["FresnelZOffset"] = value; }
double DesignElement::getFresnelZOffset() const { return m_elementParameters["FresnelZOffset"].as_double(); }

void DesignElement::setDesignAlphaAngle(Rad value) { m_elementParameters["DesignAlphaAngle"] = value; }
Rad DesignElement::getDesignAlphaAngle() const { return m_elementParameters["DesignAlphaAngle"].as_rad(); }

void DesignElement::setDesignBetaAngle(Rad value) { m_elementParameters["DesignBetaAngle"] = value; }
Rad DesignElement::getDesignBetaAngle() const { return m_elementParameters["DesignBetaAngle"].as_rad(); }

void DesignElement::setDesignOrderOfDiffraction(double value) { m_elementParameters["DesignOrderDiffraction"] = value; }
double DesignElement::getDesignOrderOfDiffraction() const { return m_elementParameters["DesignOrderDiffraction"].as_double(); }

void DesignElement::setDesignEnergy(double value) { m_elementParameters["DesignEnergy"] = value; }
double DesignElement::getDesignEnergy() const { return m_elementParameters["DesignEnergy"].as_double(); }

void DesignElement::setDesignSagittalEntranceArmLength(double value) { m_elementParameters["DesignSagittalEntranceArmLength"] = value; }
double DesignElement::getDesignSagittalEntranceArmLength() const { return m_elementParameters["DesignSagittalEntranceArmLength"].as_double(); }

void DesignElement::setDesignSagittalExitArmLength(double value) { m_elementParameters["DesignSagittalExitArmLength"] = value; }
double DesignElement::getDesignSagittalExitArmLength() const { return m_elementParameters["DesignSagittalExitArmLength"].as_double(); }

void DesignElement::setDesignMeridionalEntranceArmLength(double value) { m_elementParameters["DesignMeridionalEntranceArmLength"] = value; }
double DesignElement::getDesignMeridionalEntranceArmLength() const { return m_elementParameters["DesignMeridionalEntranceArmLength"].as_double(); }

void DesignElement::setDesignMeridionalExitArmLength(double value) { m_elementParameters["DesignMeridionalExitArmLength"] = value; }
double DesignElement::getDesignMeridionalExitArmLength() const { return m_elementParameters["DesignMeridionalExitArmLength"].as_double(); }

void DesignElement::setOrderOfDiffraction(double value) { m_elementParameters["OrderDiffraction"] = value; }
double DesignElement::getOrderOfDiffraction() const { return m_elementParameters["OrderDiffraction"].as_double(); }

void DesignElement::setAdditionalOrder(double value) { m_elementParameters["additionalOrder"] = value; }
double DesignElement::getAdditionalOrder() const { return m_elementParameters["additionalOrder"].as_double(); }

void DesignElement::setImageType(double value) { m_elementParameters["imageType"] = value; }
double DesignElement::getImageType() const { return m_elementParameters["imageType"].as_double(); }

void DesignElement::setCurvatureType(CurvatureType value) { m_elementParameters["curvatureType"] = value; }
CurvatureType DesignElement::getCurvatureType() const { return m_elementParameters["curvatureType"].as_curvatureType(); }

void DesignElement::setBehaviourType(BehaviourType value) { m_elementParameters["behaviourType"] = value; }
BehaviourType DesignElement::getBehaviourType() const { return m_elementParameters["behaviourType"].as_behaviourType(); }

void DesignElement::setCrystalType(CrystalType value) { m_elementParameters["crystalType"] = value; }
CrystalType DesignElement::getCrystalType() const { return m_elementParameters["crystalType"].as_crystalType(); }

void DesignElement::setCrystalMaterial(const std::string& value) { m_elementParameters["crystalMaterial"] = value; }
std::string DesignElement::getCrystalMaterial() const { return m_elementParameters["crystalMaterial"].as_string(); }

void DesignElement::setStructureFactorReF0(double value) { m_elementParameters["structureFactorReF0"] = value; }
double DesignElement::getStructureFactorReF0() const { return m_elementParameters["structureFactorReF0"].as_double(); }

void DesignElement::setStructureFactorImF0(double value) { m_elementParameters["structureFactorImF0"] = value; }
double DesignElement::getStructureFactorImF0() const { return m_elementParameters["structureFactorImF0"].as_double(); }

void DesignElement::setStructureFactorReFH(double value) { m_elementParameters["structureFactorReFH"] = value; }
double DesignElement::getStructureFactorReFH() const { return m_elementParameters["structureFactorReFH"].as_double(); }

void DesignElement::setStructureFactorImFH(double value) { m_elementParameters["structureFactorImFH"] = value; }
double DesignElement::getStructureFactorImFH() const { return m_elementParameters["structureFactorImFH"].as_double(); }

void DesignElement::setStructureFactorReFHC(double value) { m_elementParameters["structureFactorReFHC"] = value; }
double DesignElement::getStructureFactorReFHC() const { return m_elementParameters["structureFactorReFHC"].as_double(); }

void DesignElement::setStructureFactorImFHC(double value) { m_elementParameters["structureFactorImFHC"] = value; }
double DesignElement::getStructureFactorImFHC() const { return m_elementParameters["structureFactorImFHC"].as_double(); }

void DesignElement::setUnitCellVolume(double value) { m_elementParameters["unitCellVolume"] = value; }
double DesignElement::getUnitCellVolume() const { return m_elementParameters["unitCellVolume"].as_double(); }

void DesignElement::setDSpacing2(double value) { m_elementParameters["dSpacing2"] = value; }
double DesignElement::getDSpacing2() const { return m_elementParameters["dSpacing2"].as_double(); }

void DesignElement::setOffsetAngle(Rad value) { m_elementParameters["offsetAngle"] = value; }
Rad DesignElement::getOffsetAngle() const { return m_elementParameters["offsetAngle"].as_rad(); }

void DesignElement::setThicknessSubstrate(double value) { m_elementParameters["thicknessSubstrate"] = value; }
double DesignElement::getThicknessSubstrate() const { return m_elementParameters["thicknessSubstrate"].as_double(); }

void DesignElement::setRoughnessSubstrate(double value) { m_elementParameters["roughnessSubstrate"] = value; }
double DesignElement::getRoughnessSubstrate() const { return m_elementParameters["roughnessSubstrate"].as_double(); }

void DesignElement::setDesignPlane(DesignPlane value) { m_elementParameters["designPlane"] = value; }
DesignPlane DesignElement::getDesignPlane() const { return m_elementParameters["designPlane"].as_designPlane(); }

}  // namespace RAYX
