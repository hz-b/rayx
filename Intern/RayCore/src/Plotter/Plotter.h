#pragma once

#include <mgl2/mgl.h>

#include <map>
#include <string>
#include <vector>

#include "Core.h"
#include "Model/Beamline/Beamline.h"
#include "Model/Beamline/OpticalElement.h"
#include "Tracer/Ray.h"

namespace RAYX {
class RAYX_API Plotter {
  public:
    Plotter() = default;
    static void plot(int plotType, const std::string& plotName, const std::vector<Ray>& RayList, const std::unique_ptr<RAYX::Beamline>& beamline);
    ~Plotter() = default;

  private:
    enum plotTypes { SinglePlot, MultiPlot };
};
}  // namespace RAYX