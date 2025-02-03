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

    // Set (or update) the beamline pointer.
    void setBeamline(const RAYX::Beamline* beamline);

    // Show the outline window (to be called every frame in your UI loop)
    void showBeamlineOutlineWindow();

  private:
    // Pointer to the beamline (the Group that is the root of the tree)
    const RAYX::Beamline* m_Beamline = nullptr;

    // Recursive helper that renders the tree starting at a given Group.
    void renderImGuiTreeFromGroup(const RAYX::Group& group, int depth = 0);
};
