#pragma once

#include <glm.hpp>

#include "Beamline/Beamline.h"

// Forward declarations for your UI types
class CameraController;
struct UIBeamlineInfo;
struct UIParameters;

class BeamlineOutliner {
  public:
    BeamlineOutliner();
    ~BeamlineOutliner();

    // Show the outline window (to be called every frame in your UI loop)
    void showBeamlineOutlineWindow(UIParameters& uiParams);

  private:
    // Recursive helper that renders the tree starting at a given Group.
    void renderImGuiTreeFromGroup(RAYX::Group* group, RAYX::BeamlineNode*& selected, CameraController& cam, int depth = 0);
};
