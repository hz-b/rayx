#pragma once
#include <map>

#include "Settings.h"

class BeamlineDesignHandler {
  public:
    // BeamlineDesignHandler();
    // ~BeamlineDesignHandler();

    void showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo);

  private:
    void showParameters(DesignMap& parameters, bool& changed);
    void createInputField(const std::string& key, DesignMap& element, bool& changed);

    // Define groups
    std::map<std::string, std::vector<std::string>> groups = {
        {"Coordinate Errors", {"translationXerror", "translationYerror", "translationZerror", "rotationXerror", "rotationYerror", "rotationZerror"}},
        {"Position",
         {"worldPosition", "alignmentError", "sourceDepth", "sourceHeight", "sourceWidth", "sourcePulseType", "positionType", "heightDistribution",
          "widthDistribution"}},
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
        {"Stokes", {"linPol"}}};

    std::vector<std::string> customOrder = {
        "type",           "name",           "numberOfRays",  "translationXerror", "translationYerror", "translationZerror",
        "rotationXerror", "rotationYerror", "rotationZerror"};
};
