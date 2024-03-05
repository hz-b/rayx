#include "DesignElement.h"

#include "Debug/Debug.h"
#include "Beamline/Objects/SurfaceType.h"
#include "Beamline/Objects/BehaviourType.h"

namespace RAYX {
    
Element DesignElement::compile() const {
    Surface surface;
    Behaviour behav;

    if (getType() == "Experts Optics") {
        return makeElement(*this, serializeMirror(), makeQuadric(*this));
    } else {
        surface = makeSurface(*this);
        behav = makeBehaviour(*this);
        if (getType() == "Slit") {
            return makeElement(*this, behav, surface, {}, DesignPlane::XY);
        } else if (getType() == "ImagePlane") {
            return makeElement(*this, behav, surface, serializeUnlimited(), DesignPlane::XY);
        } else {
            return makeElement(*this, behav, surface);
        }
    }
    
    
}

void DesignElement::setName(std::string s) { v["name"] = s; }
void DesignElement::setType(std::string s) { v["type"] = s; }

std::string DesignElement::getName() const { return v["name"].as_string(); }
std::string DesignElement::getType() const { return v["type"].as_string(); }

void DesignElement::setWorldPosition(glm::dvec4 p) {
    v["worldPosition"] = Map();
    v["worldPosition"]["x"] = p.x;
    v["worldPosition"]["y"] = p.y;
    v["worldPosition"]["z"] = p.z;
    v["worldPosition"]["w"] = p.w;
}

glm::dvec4 DesignElement::getWorldPosition() const {
    glm::dvec4 d;
    d[0] = v["worldPosition"]["x"].as_double();
    d[1] = v["worldPosition"]["y"].as_double();
    d[2] = v["worldPosition"]["z"].as_double();
    d[3] = 0;
    return d;
}

void DesignElement::setWorldOrientation(glm::dmat4x4 o) {
    v["worldXDirection"] = Map();
    v["worldXDirection"]["x"] = o[0][0];
    v["worldXDirection"]["y"] = o[0][1];
    v["worldXDirection"]["z"] = o[0][2];
    v["worldXDirection"]["w"] = o[0][3];

    v["worldYDirection"] = Map();
    v["worldYDirection"]["x"] = o[1][0];
    v["worldYDirection"]["y"] = o[1][1];
    v["worldYDirection"]["z"] = o[1][2];
    v["worldYDirection"]["w"] = o[1][3];

    v["worldZDirection"] = Map();
    v["worldZDirection"]["x"] = o[2][0];
    v["worldZDirection"]["y"] = o[2][1];
    v["worldZDirection"]["z"] = o[2][2];
    v["worldZDirection"]["w"] = o[2][3];
}

glm::dmat4x4 DesignElement::getWorldOrientation() const {
    glm::dmat4x4 o;

    o[0][0] = v["worldXDirection"]["x"].as_double();
    o[0][1] = v["worldXDirection"]["y"].as_double();
    o[0][2] = v["worldXDirection"]["z"].as_double();
    o[0][3] = 0;

    o[1][0] = v["worldYDirection"]["x"].as_double();
    o[1][1] = v["worldYDirection"]["y"].as_double();
    o[1][2] = v["worldYDirection"]["z"].as_double();
    o[1][3] = 0;

    o[2][0] = v["worldZDirection"]["x"].as_double();
    o[2][1] = v["worldZDirection"]["y"].as_double();
    o[2][2] = v["worldZDirection"]["z"].as_double();
    o[2][3] = 0;

    return o;
}

void DesignElement::setMisalignment(Misalignment m) {
    v["rotationXerror"] = m.m_rotationXerror.rad;
    v["rotationYerror"] = m.m_rotationYerror.rad;
    v["rotationZerror"] = m.m_rotationZerror.rad;

    v["translationXerror"] = m.m_translationXerror;
    v["translationYerror"] = m.m_translationYerror;
    v["translationZerror"] = m.m_translationZerror;
}


Misalignment DesignElement::getMisalignment() const {
    Misalignment m;
    m.m_rotationXerror.rad = v["rotationXerror"].as_double();
    m.m_rotationYerror.rad = v["rotationYerror"].as_double();
    m.m_rotationZerror.rad = v["rotationZerror"].as_double();

    m.m_translationXerror = v["translationXerror"].as_double();
    m.m_translationYerror = v["translationYerror"].as_double();
    m.m_translationZerror = v["translationZerror"].as_double();

    return m;
}
void DesignElement::setSlopeError(SlopeError s) {
    v["SlopeError"] = Map();
    v["SlopeError"]["slopeErrorSag"] = s.m_sag;
    v["SlopeError"]["slopeErrorMer"] = s.m_mer;
    v["SlopeError"]["thermalDistortionAmp"] = s.m_thermalDistortionAmp;
    v["SlopeError"]["thermalDistortionSigmaX"] = s.m_thermalDistortionSigmaX;
    v["SlopeError"]["thermalDistortionSigmaZ"] = s.m_thermalDistortionSigmaZ;
    v["SlopeError"]["cylindricalBowingAmp"] = s.m_cylindricalBowingAmp;
    v["SlopeError"]["cylindricalBowingRadius"] = s.m_cylindricalBowingRadius;
}
SlopeError DesignElement::getSlopeError() const {
    SlopeError s;
    s.m_sag = v["SlopeError"]["slopeErrorSag"].as_double();
    s.m_mer = v["SlopeError"]["slopeErrorMer"].as_double();
    s.m_thermalDistortionAmp = v["SlopeError"]["thermalDistortionAmp"].as_double();
    s.m_thermalDistortionSigmaX = v["SlopeError"]["thermalDistortionSigmaX"].as_double();
    s.m_thermalDistortionSigmaZ = v["SlopeError"]["thermalDistortionSigmaZ"].as_double();
    s.m_cylindricalBowingAmp = v["SlopeError"]["cylindricalBowingAmp"].as_double();
    s.m_cylindricalBowingRadius = v["SlopeError"]["cylindricalBowingRadius"].as_double();

    return s;
}


void DesignElement::setCutout(Cutout c) {
    v["geometricalShape"] = c.m_type;
    if (c.m_type == CTYPE_RECT) {
        RectCutout rect = deserializeRect(c);
        v["CutoutWidth"] = rect.m_width;
        v["CutoutLength"] = rect.m_length;
    } else if (c.m_type == CTYPE_ELLIPTICAL) {
        EllipticalCutout elli = deserializeElliptical(c);
        v["CutoutDiameterX"] = elli.m_diameter_x;
        v["CutoutDiameterZ"] = elli.m_diameter_z;
    } else if (c.m_type == CTYPE_TRAPEZOID) {
        TrapezoidCutout trapi = deserializeTrapezoid(c);
        v["CutoutWidthA"] = trapi.m_widthA;
        v["CutoutWidthB"] = trapi.m_widthB;
        v["CutoutLength"] = trapi.m_length;
    }
}
Cutout DesignElement::getCutout() const {
    Cutout c;

    c.m_type = v["geometricalShape"].as_double();

    if (c.m_type == CTYPE_RECT) { // Rectangle
        RectCutout rect;
        rect.m_width = v["CutoutWidth"].as_double();
        rect.m_length = v["CutoutLength"].as_double();
        c = serializeRect(rect);
    } else if (c.m_type == CTYPE_ELLIPTICAL) { //Ellipsoid
        EllipticalCutout elli;
        elli.m_diameter_x = v["CutoutDiameterX"].as_double();
        elli.m_diameter_z = v["CutoutDiameterZ"].as_double();
        c = serializeElliptical(elli);
    } else if (c.m_type == CTYPE_TRAPEZOID) { //Trapezoid
        TrapezoidCutout trapi;
        trapi.m_widthA = v["CutoutWidthA"].as_double();
        trapi.m_widthB = v["CutoutWidthB"].as_double();
        trapi.m_length = v["CutoutLength"].as_double();
        c = serializeTrapezoid(trapi);
    }

    return c;
}

Cutout DesignElement::getGlobalCutout() const {
    return serializeUnlimited();
}

void DesignElement::setVLSParameters(const std::array<double, 6>& values) {
    v["vlsParams"] = Map();

    v["vlsParams"]["vlsParameterB2"] = values[0];
    v["vlsParams"]["vlsParameterB3"] = values[1];
    v["vlsParams"]["vlsParameterB4"] = values[2];
    v["vlsParams"]["vlsParameterB5"] = values[3];
    v["vlsParams"]["vlsParameterB6"] = values[4];
    v["vlsParams"]["vlsParameterB7"] = values[5];
}

std::array<double, 6> DesignElement::getVLSParameters() const {
    return {
        v["vlsParams"]["vlsParameterB2"].as_double(),
        v["vlsParams"]["vlsParameterB3"].as_double(),
        v["vlsParams"]["vlsParameterB4"].as_double(),
        v["vlsParams"]["vlsParameterB5"].as_double(),
        v["vlsParams"]["vlsParameterB6"].as_double(),
        v["vlsParams"]["vlsParameterB7"].as_double()
    };
}



void DesignElement::setExpertsOptics(Surface value) {
    QuadricSurface qua = deserializeQuadric(value);
    v["expertsParams"] = Map();
    v["expertsParams"]["A11"] = qua.m_a11;
    v["expertsParams"]["A12"] = qua.m_a12;
    v["expertsParams"]["A13"] = qua.m_a13;
    v["expertsParams"]["A14"] = qua.m_a14;
    v["expertsParams"]["A22"] = qua.m_a22;
    v["expertsParams"]["A23"] = qua.m_a23;
    v["expertsParams"]["A24"] = qua.m_a24;
    v["expertsParams"]["A33"] = qua.m_a33;
    v["expertsParams"]["A34"] = qua.m_a34;
    v["expertsParams"]["A44"] = qua.m_a44;

}

Surface DesignElement::getExpertsOptics() const {
    QuadricSurface qua;
    qua.m_a11 = v["expertsParams"]["A11"].as_double();
    qua.m_a12 = v["expertsParams"]["A12"].as_double();
    qua.m_a13 = v["expertsParams"]["A13"].as_double();
    qua.m_a14 = v["expertsParams"]["A14"].as_double();
    qua.m_a22 = v["expertsParams"]["A22"].as_double();
    qua.m_a23 = v["expertsParams"]["A23"].as_double();
    qua.m_a24 = v["expertsParams"]["A24"].as_double();
    qua.m_a33 = v["expertsParams"]["A33"].as_double();
    qua.m_a34 = v["expertsParams"]["A34"].as_double();
    qua.m_a44 = v["expertsParams"]["A44"].as_double();

    return serializeQuadric(qua);
}

void DesignElement::setExpertsCubic(Surface value) {
    CubicSurface cub = deserializeCubic(value);
    v["expertsParams"] = Map();
    v["expertsParams"]["A11"] = cub.m_a11;
    v["expertsParams"]["A12"] = cub.m_a12;
    v["expertsParams"]["A13"] = cub.m_a13;
    v["expertsParams"]["A14"] = cub.m_a14;
    v["expertsParams"]["A22"] = cub.m_a22;
    v["expertsParams"]["A23"] = cub.m_a23;
    v["expertsParams"]["A24"] = cub.m_a24;
    v["expertsParams"]["A33"] = cub.m_a33;
    v["expertsParams"]["A34"] = cub.m_a34;
    v["expertsParams"]["A44"] = cub.m_a44;

    v["expertsParams"]["B12"] = cub.m_b12;
    v["expertsParams"]["B13"] = cub.m_b13;
    v["expertsParams"]["B21"] = cub.m_b21;
    v["expertsParams"]["B23"] = cub.m_b23;
    v["expertsParams"]["B31"] = cub.m_b31;
    v["expertsParams"]["B32"] = cub.m_b32;
}

Surface DesignElement::getExpertsCubic() const {
    CubicSurface cub;
    cub.m_a11 = v["expertsParams"]["A11"].as_double();
    cub.m_a12 = v["expertsParams"]["A12"].as_double();
    cub.m_a13 = v["expertsParams"]["A13"].as_double();
    cub.m_a14 = v["expertsParams"]["A14"].as_double();
    cub.m_a22 = v["expertsParams"]["A22"].as_double();
    cub.m_a23 = v["expertsParams"]["A23"].as_double();
    cub.m_a24 = v["expertsParams"]["A24"].as_double();
    cub.m_a33 = v["expertsParams"]["A33"].as_double();
    cub.m_a34 = v["expertsParams"]["A34"].as_double();
    cub.m_a44 = v["expertsParams"]["A44"].as_double();

    cub.m_b12 = v["expertsParams"]["B12"].as_double();
    cub.m_b13 = v["expertsParams"]["B13"].as_double();
    cub.m_b21 = v["expertsParams"]["B21"].as_double();
    cub.m_b23 = v["expertsParams"]["B23"].as_double();
    cub.m_b31 = v["expertsParams"]["B31"].as_double();
    cub.m_b32 = v["expertsParams"]["B32"].as_double();

    return serializeCubic(cub);
}



// Azimuthal Angle
void DesignElement::setAzimuthalAngle(Rad r) { v["AzimuthalAngle"] = r; }
Rad DesignElement::getAzimuthalAngle() const { return v["AzimuthalAngle"].as_rad(); }

// Material
void DesignElement::setMaterial(Material m) { v["Material"] = m; }
Material DesignElement::getMaterial() const { return v["Material"].as_material(); }

// Distance Preceding
void DesignElement::setDistancePreceding(double distance) { v["distancePreceding"] = distance; }
double DesignElement::getDistancePreceding() const { return v["distancePreceding"].as_double(); }

// Total Height
void DesignElement::setTotalHeight(double height) { v["totalHeight"] = height; }
double DesignElement::getTotalHeight() const { return v["totalHeight"].as_double(); }

// Opening Shape
void DesignElement::setOpeningShape(double shape) { v["openingShape"] = shape; }
double DesignElement::getOpeningShape() const { return v["openingShape"].as_double(); }

// Opening Width
void DesignElement::setOpeningWidth(double width) { v["openingWidth"] = width; }
double DesignElement::getOpeningWidth() const { return v["openingWidth"].as_double(); }

// Opening Height
void DesignElement::setOpeningHeight(double height) { v["openingHeight"] = height; }
double DesignElement::getOpeningHeight() const { return v["openingHeight"].as_double(); }

// Central Beamstop
void DesignElement::setCentralBeamstop(CentralBeamstop value) { v["centralBeamstop"] = value; }
CentralBeamstop DesignElement::getCentralBeamstop() const { return v["centralBeamstop"].as_centralBeamStop(); }

// Stop Width
void DesignElement::setStopWidth(double width) { v["stopWidth"] = width; }
double DesignElement::getStopWidth() const { return v["stopWidth"].as_double(); }

// Stop Height
void DesignElement::setStopHeight(double height) { v["stopHeight"] = height; }
double DesignElement::getStopHeight() const { return v["stopHeight"].as_double(); }

// Total Width
void DesignElement::setTotalWidth(double width) { v["totalWidth"] = width; }
double DesignElement::getTotalWidth() const { return v["totalWidth"].as_double(); }

// Profile Kind
void DesignElement::setProfileKind(int value) { v["profileKind"] = value; }
int DesignElement::getProfileKind() const { return v["profileKind"].as_int(); }

// Profile File
void DesignElement::setProfileFile(double filePath) { v["profileFile"] = filePath; }
double DesignElement::getProfileFile() const { return v["profileFile"].as_double(); }

// Total Length
void DesignElement::setTotalLength(double value) { v["totalLength"] = value; }
double DesignElement::getTotalLength() const { return v["totalLength"].as_double(); }

// Grazing Inc Angle
void DesignElement::setGrazingIncAngle(Rad value) { v["grazingIncAngle"] = value; }
Rad DesignElement::getGrazingIncAngle() const { return v["grazingIncAngle"].as_rad(); }

// Entrance Arm Length
void DesignElement::setEntranceArmLength(double value) { v["entranceArmLength"] = value; }
double DesignElement::getEntranceArmLength() const { return v["entranceArmLength"].as_double(); }

// Exit Arm Length
void DesignElement::setExitArmLength(double value) { v["exitArmLength"] = value; }
double DesignElement::getExitArmLength() const { return v["exitArmLength"].as_double(); }

// bendingRadius
void DesignElement::setRadiusDirection(CylinderDirection value) { v["bendingRadius"] = value; }
CylinderDirection DesignElement::getRadiusDirection() const { return v["bendingRadius"].as_cylinderDirection(); }

// radius
void DesignElement::setRadius(double value) { v["radius"] = value; }
double DesignElement::getRadius() const { return v["radius"].as_double(); }


void DesignElement::setDesignGrazingIncAngle(Rad value) { v["designGrazingIncAngle"] = value; }
Rad DesignElement::getDesignGrazingIncAngle() const { return v["designGrazingIncAngle"].as_rad(); }

// longHalfAxisA
void DesignElement::setLongHalfAxisA(double value) { v["longHalfAxisA"] = value; }
double DesignElement::getLongHalfAxisA() const { return v["longHalfAxisA"].as_double(); }

// shortHalfAxisB
void DesignElement::setShortHalfAxisB(double value) { v["shortHalfAxisB"] = value; }
double DesignElement::getShortHalfAxisB() const { return v["shortHalfAxisB"].as_double(); }

void DesignElement::setParameterA11(double value) { v["parameter_a11"] = value; }
double DesignElement::getParameterA11() const { return v["parameter_a11"].as_double(); }

// figureRotation
void DesignElement::setFigureRotation(FigureRotation value) { v["figureRotation"] = value; }
FigureRotation DesignElement::getFigureRotation() const { return v["figureRotation"].as_figureRotation(); }

// armLength
void DesignElement::setArmLength(double value) { v["armLength"] = value; }
double DesignElement::getArmLength() const { return v["armLength"].as_double(); }

//parameter_P
void DesignElement::setParameterP(double value) { v["parameter_P"] = value; }
double DesignElement::getParameterP() const { return v["parameter_P"].as_double(); }

//parameter_P_type
void DesignElement::setParameterPType(double value) { v["parameter_P_type"] = value; }
double DesignElement::getParameterPType() const { return v["parameter_P_type"].as_double(); }

// Setter and Getter for lineDensity
void DesignElement::setLineDensity(double value) { v["lineDensity"] = value; }
double DesignElement::getLineDensity() const { return v["lineDensity"].as_double(); }

void DesignElement::setShortRadius(double value) { v["shortRadius"] = value;}
double DesignElement::getShortRadius() const {return v["shortRadius"].as_double();}

// Setter and Getter for longRadius
void DesignElement::setLongRadius(double value) {v["longRadius"] = value;}
double DesignElement::getLongRadius() const {return v["longRadius"].as_double();}

void DesignElement::setFresnelZOffset(double value) { v["FresnelZOffset"] = value; }
double DesignElement::getFresnelZOffset() const { return v["FresnelZOffset"].as_double(); }

void DesignElement::setDesignAlphaAngle(Rad value) { v["DesignAlphaAngle"] = value; }
Rad DesignElement::getDesignAlphaAngle() const { return v["DesignAlphaAngle"].as_rad(); }

void DesignElement::setDesignBetaAngle(Rad value) { v["DesignBetaAngle"] = value; }
Rad DesignElement::getDesignBetaAngle() const { return v["DesignBetaAngle"].as_rad(); }

void DesignElement::setDesignOrderOfDiffraction(double value) { v["DesignOrderDiffraction"] = value; }
double DesignElement::getDesignOrderOfDiffraction() const { return v["DesignOrderDiffraction"].as_double(); }

void DesignElement::setDesignEnergy(double value) { v["DesignEnergy"] = value; }
double DesignElement::getDesignEnergy() const { return v["DesignEnergy"].as_double(); }

void DesignElement::setDesignSagittalEntranceArmLength(double value) { v["DesignSagittalEntranceArmLength"] = value; }
double DesignElement::getDesignSagittalEntranceArmLength() const { return v["DesignSagittalEntranceArmLength"].as_double(); }

void DesignElement::setDesignSagittalExitArmLength(double value) { v["DesignSagittalExitArmLength"] = value; }
double DesignElement::getDesignSagittalExitArmLength() const { return v["DesignSagittalExitArmLength"].as_double(); }

void DesignElement::setDesignMeridionalEntranceArmLength(double value) { v["DesignMeridionalEntranceArmLength"] = value; }
double DesignElement::getDesignMeridionalEntranceArmLength() const { return v["DesignMeridionalEntranceArmLength"].as_double(); }

void DesignElement::setDesignMeridionalExitArmLength(double value) { v["DesignMeridionalExitArmLength"] = value; }
double DesignElement::getDesignMeridionalExitArmLength() const { return v["DesignMeridionalExitArmLength"].as_double(); }

void DesignElement::setOrderOfDiffraction(double value) { v["OrderDiffraction"] = value; }
double DesignElement::getOrderOfDiffraction() const { return v["OrderDiffraction"].as_double(); }

void DesignElement::setAdditionalOrder(double value) {v["additionalOrder"] = value;}
double DesignElement::getAdditionalOrder() const { return v["additionalOrder"].as_double();}

void DesignElement::setImageType(double value) {v["imageType"] = value;}
double DesignElement::getImageType() const {return v["imageType"].as_double();}

void DesignElement::setCurvatureType(CurvatureType value) {v["curvatureType"] = value;}
CurvatureType DesignElement::getCurvatureType() const {return v["curvatureType"].as_curvatureType();}

void DesignElement::setBehaviourType(BehaviourType value) {v["behaviourType"] = value;}
BehaviourType DesignElement::getBehaviourType() const {return v["behaviourType"].as_behaviourType();}


}  // namespace RAYX