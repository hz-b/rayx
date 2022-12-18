#pragma once

#include <mgl2/mgl.h>

#include <cmath>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "Core.h"
#include "Debug/Instrumentor.h"
#include "Model/Beamline/Beamline.h"
#include "Model/Beamline/OpticalElement.h"
#include "Tracer/Ray.h"

namespace RAYX {
class RAYX_API Plotter {
  public:
    Plotter();
    void plot(int plotType, const std::string& plotName, const std::vector<Ray>& RayList, const std::unique_ptr<RAYX::Beamline>& beamline);
    ~Plotter() = default;

    /**
     * @brief Compact Optical Element Description
     *
     */
    struct OpticalElementMeta {
        std::string name;
        int type;
        double material;
        double height;
        double width;
    };

    /**
     * @brief Grid Struct used for manual data binning
     *
     */
    struct myGrid {
        std::vector<double> xGrid;
        std::vector<double> yGrid;
        std::vector<double> intensity;
    };

  private:
    bool importPlotSettings(const std::string& filename);
    void plotSingle(const std::vector<Ray>& RayList, const std::string& plotName, const std::vector<OpticalElementMeta>& OpticalElementsMeta);
    void plotReducer(const std::vector<double>& x, const std::vector<double>& y, double width, double height, double x_offset, double y_offset,
                     myGrid& grid);

    enum plotTypes { SinglePlot, MultiPlot };
    std::map<std::string, std::variant<std::string, int, bool>> m_plotSettings{
        {"factorX", 10},   {"factorY", 10},    {"scatterColor", "x62C300"}, {"automaticBinning", false}, {"coloredIntensity", false},
        {"plot3D", false}, {"colorBar", true}, {"histArea", false}};
};
}  // namespace RAYX