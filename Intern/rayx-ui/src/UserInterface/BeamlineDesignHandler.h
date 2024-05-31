#pragma once
#include "Settings.h"

class BeamlineDesignHandler {
  public:
    // BeamlineDesignHandler();
    // ~BeamlineDesignHandler();

    void showBeamlineDesignWindow(UIBeamlineInfo& uiBeamlineInfo);

  private:
    void createInputField(const std::string& key, DesignMap& element, bool& changed);
};
