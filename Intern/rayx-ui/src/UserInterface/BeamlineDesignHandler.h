#pragma once
#include <map>

#include "Settings.h"

class BeamlineDesignHandler {
  public:
    // BeamlineDesignHandler();
    // ~BeamlineDesignHandler();

    void showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo);

  private:
    void showParameters(DesignMap& parameters, bool& changed, uint32_t type);
    void createInputField(const std::string& key, DesignMap& element, bool& changed, uint32_t type, int nestingLevel);

    // Define groups
    std::map<std::string, std::vector<std::string>> groups = {
        {"Position",
         {"worldPosition", "alignmentError", "sourceDepth", "sourceHeight", "sourceWidth", "sourcePulseType", "positionType", "heightDistribution",
          "widthDistribution"}},
        {"Coordinate Errors", {"translationXerror", "translationYerror", "translationZerror", "rotationXerror", "rotationYerror", "rotationZerror"}},
        {"Direction",
         {
             "horDivergence",
             "verDivergence",
             "numberOfCircles",
             "maxOpeningAngle",
             "minOpeningAngle",
             "deltaOpeningHandle",
         }},
        {"WorldDirection", {"worldXDirection", "worldYDirection", "worldZDirection"}},
        {"Energy", {"energy", "energySpread", "energyDistributionType", "energyDistribution", "photonEnergyDistributionFile", "separateEnergies"}},
        {"Stokes", {"linPol"}},
        {"Behaviour", {"geometricalShape", "behaviourType", "curvatureType", "CuoutLength", "CutoutWidth", "Material", "totalLength"}}};

    std::vector<std::string> customOrder = {
        "type",           "name",           "numberOfRays",  "translationXerror", "translationYerror", "translationZerror",
        "rotationXerror", "rotationYerror", "rotationZerror"};
};
