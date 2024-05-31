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
        {"Source Parameters", {"sourceDepth", "sourceHeight", "sourceWidth"}}};

    std::vector<std::string> customOrder = {"type",           "name",           "translationXerror", "translationYerror", "translationZerror",
                                            "rotationXerror", "rotationYerror", "rotationZerror"};
};
