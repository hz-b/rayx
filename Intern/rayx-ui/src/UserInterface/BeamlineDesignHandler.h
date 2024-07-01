#pragma once
#include <map>

#include "Settings.h"

class BeamlineDesignHandler {
  public:
    void showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo);

  private:
    void showParameters(DesignMap& parameters, bool& changed, uint32_t type);
    void createInputField(const std::string& key, DesignMap& element, bool& changed, uint32_t type, int nestingLevel);

    bool caseInsensitiveCompare(const std::string& a, const std::string& b);

    // Define groups
    std::map<std::string, std::vector<std::string>> groups = {
        {"Position",
         {"worldPosition", "alignmentError", "sourceDepth", "sourceHeight", "sourceWidth", "sourcePulseType", "positionType", "heightDist",
          "widthDist", "sigmaType", "electronSigmaX", "electronSigmaY", "undulatorLength", "electronEnergy", "bendingRadius",
          "electronEnergyOrientation"}},
        {"Coordinate Errors",
         {"translationXerror", "translationYerror", "translationZerror", "rotationXerror", "rotationYerror", "rotationZerror", "AzimuthalAngle",
          "grazingIncAngle"}},
        {"Direction",
         {"horDivergence", "verDivergence", "horDist", "verDist", "numberOfCircles", "maxOpeningAngle", "minOpeningAngle", "deltaOpeningHandle",
          "electronSigmaXs", "electronSigmaYs", "deltaOpeningAngle", "numOfCircles", "verEBeamDivergence"}},
        {"WorldDirection", {"worldXDirection", "worldYDirection", "worldZDirection"}},
        {"Energy",
         {"energy", "energySpread", "energyDistributionType", "SeparateEnergies", "energyDistribution", "photonEnergyDistributionFile",
          "separateEnergies", "energySpreadUnit"}},
        {"Stokes", {"linPol"}},
        {"Slit Settings",
         {"centralBeamstop", "stopHeight", "stopWidth", "totalHeight", "totalWidth", "openingHeight", "openingShape", "openingWidth"}},
        {"RZP Settings",
         {"additionalOrder", "DesignAlphaAngle", "DesignBetaAngle", "DesignEnergy", "DesignMeridionalEntranceArmLength",
          "DesignMeridionalExitArmLength", "DesignOrderDiffraction", "DesignSagittalEntranceArmLength", "DesignSagittalExitArmLength",
          "FresnelZOffset", "imageType"}},
        {"Surface", {"geometricalShape", "behaviourType",    "curvatureType",     "CutoutLength",   "CutoutWidth",
                     "Material",         "totalLength",      "lineDensity",       "deviationAngle", "radius",
                     "OrderDiffraction", "lognRadius",       "shortRadius",       "armLength",      "parameter_a11",
                     "parameter_P",      "parameter_P_type", "entranceArmLength", "exitArmLength",  "designGrazingIncAngle",
                     "figureRotation",   "longHalfAxisA",    "shortHalfAxisB",    "longRadius"}}};

    std::vector<std::string> customOrder = {"type",
                                            "name",
                                            "numberOfRays",
                                            "translationXerror",
                                            "translationYerror",
                                            "translationZerror",
                                            "rotationXerror",
                                            "rotationYerror",
                                            "rotationZerror",
                                            "energy",
                                            "energySpread",
                                            "energyDistribution",
                                            "photonEnergyDistributionFile",
                                            "energyDistributionType",
                                            "SeparateEnergies",
                                            "worldPosition",
                                            "geometricalShape",
                                            "totalLength",
                                            "curvatureType",
                                            "radius",
                                            "behaviourType",
                                            "Material",
                                            "CutoutWidth",
                                            "CutoutLength",
                                            "undulatorLength",
                                            "sigmaType",
                                            "electronSigmaX",
                                            "electronSigmaY",
                                            "centralBeamstop",
                                            "stopHeight",
                                            "stopWidth",
                                            "totalHeight",
                                            "totalWidth",
                                            "openingShape",
                                            "openingHeight",
                                            "openingWidth"};

    const char* sourceItems[6] = {"Matrix Source", "Point Source", "Dipole Source", "Pixel Source", "Circle Source", "Simple Undulator"};
    const char* opticalElementItems[13] = {
        "ImagePlane",           "Cone", "Cylinder",          "Ellipsoid", "Paraboloid",       "Plane Grating", "Plane Mirror",
        "Reflection Zoneplate", "Slit", "Spherical Grating", "Sphere",    "Spherical Mirror", "Toroid"};  //"Experts Optics" currently not supported
};
