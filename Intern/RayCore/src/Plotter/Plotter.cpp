#include "Plotter/Plotter.h"

#include <matplotlibcpp.h>

#include <algorithm>

#include "CanonicalizePath.h"
#include "Debug/Debug.h"

// #define FREEDMAN
// #define LONGEST_PATH

namespace RAYX {
inline int last_obj(int extraParam) {
    if (std::to_string(extraParam).length() > 10) RAYX_ERR << "Extra Param is too long for plotting.";  // TODO: Use a different encoding
    while (extraParam > 10) {
        extraParam /= 10;
    }
    return extraParam;
}
bool last_comp(Ray const& lhs, Ray const& rhs) { return last_obj(lhs.m_extraParam) < last_obj(rhs.m_extraParam); }
bool comp(Ray const& lhs, Ray const& rhs) { return lhs.m_extraParam < rhs.m_extraParam; }
bool abs_comp(double const& lhs, double const& rhs) { return abs(lhs) < abs(rhs); }
inline bool int_close(double x, double y) { return abs(x - y) < std::numeric_limits<double>::epsilon(); }
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
 * @brief Get all beamline optical elememt meta data
 *
 * @return std::vector<std::string> list of (names, type etc.)
 */
std::vector<OpticalElementMeta> getBeamlineOpticalElementMeta(const std::unique_ptr<RAYX::Beamline>& beamline) {
    std::vector<OpticalElementMeta> meta;
    meta.reserve(beamline->m_OpticalElements.size());

    for (const auto& opticalElement : beamline->m_OpticalElements) {
        meta.push_back({opticalElement->m_name, (int)opticalElement->getSurfaceParams()[3][0], (double)opticalElement->getSurfaceParams()[3][2],
                        opticalElement->getHeight(), opticalElement->getWidth()});
    }

    return meta;
}

/**
 * @brief Get the amount of bins for the histogram
 * FREEDMAN is a different method to get the "optimized" amount of bins
 *
 * @param vec Input Data Vector
 * @return int Bin Amount
 */
[[deprecated]] int getBinAmount(std::vector<double>& vec) {
#ifdef FREEDMAN
    std::vector<double>::iterator b = vec.begin();
    std::vector<double>::iterator e = vec.end();
    std::vector<double>::iterator n3 = b;
    std::vector<double>::iterator n1 = b;
    const std::size_t q1 = 0.25 * std::distance(b, e);
    const std::size_t q3 = 0.75 * std::distance(b, e);
    std::advance(n1, q1);
    std::advance(n3, q3);

    double width = 2 * (*n3 - *n1) / std::pow(vec.size(), 1.0 / 3);
    return (int)std::ceil((*(std::max_element(vec.begin(), vec.end())) - *(std::min_element(vec.begin(), vec.end()))) / width);
#endif

    const double binwidth = 0.00125;
    auto xymax = std::max(*(std::max_element(vec.begin(), vec.end())), *(std::min_element(vec.begin(), vec.end())), abs_comp);
    return (int)(xymax / binwidth) + 1;
}

// Output Folder Path handling and naming scheme
std::pair<std::string, std::string> outputPlotPathHandler(const std::string outputPath) {
#ifdef RAYX_DEBUG_MODE
    std::string mode = "debug";
#else
    std::string mode = "release";
#endif
    std::string outputFolderName = "build/bin/" + mode + "/Plots";  // Change this if you want to change directory output name

    std::filesystem::path outputDir = canonicalizeRepositoryPath(outputFolderName);
    if (!std::filesystem::is_directory(outputDir) || !std::filesystem::exists(outputDir)) {
        RAYX_D_LOG << "Creating directory " << outputDir.c_str();
        std::filesystem::create_directory(outputDir);
    }

    std::filesystem::path filePath = outputPath;
    auto outFileName = filePath.filename();
    auto pos = ((std::string)(outFileName)).find(".");
    auto rawName = (std::string::npos == pos) ? ((std::string)(outFileName)) : ((std::string)(outFileName)).substr(0, pos);
    std::string sampleName = rawName + "_plot";
    auto outputSvg = outputDir;
    auto outputPng = outputDir;
    outputSvg.append(sampleName + ".svg");
    outputPng.append(sampleName + ".png");
    return std::make_pair<std::string, std::string>(outputSvg.c_str(), outputPng.c_str());
}

/**
 * @brief Grid Struct used for manual data binning
 *
 */
struct myGrid {
    std::vector<double> xGrid;
    std::vector<double> yGrid;
    std::vector<int> intensity;
};

/**
 * @brief Make a grid of pixels with intensity and positions
 *
 * @param x Original X pos
 * @param y Original Y pos
 * @param width width of grid / figure
 * @param height height
 * @param x_offset x offset eg [3,-3.5] -> [6.5, 0]
 * @param y_offset y offset
 * @param grid The Grid to be reproduced
 */
void plotReducer(const std::vector<double>& x, const std::vector<double>& y, double width, double height, double x_offset, double y_offset,
                 myGrid& grid) {
    const int GRIDX_FACTOR = 50;
    const int GRIDY_FACTOR = 30;

    /**
     * Stored row-wise grid W*H, For X, Y and intensity
     */
    int GRID_W = int(width * GRIDX_FACTOR + 1);
    int GRID_H = int(height * GRIDY_FACTOR + 1);
    RAYX_D_LOG << "Grid size[W*H]: " << GRID_W << " x " << GRID_H;

    grid.xGrid.resize(GRID_H * GRID_W);
    grid.yGrid.resize(GRID_H * GRID_W);
    grid.intensity.resize(GRID_H * GRID_W);

    int cell_x, cell_y;

    for (auto i = 0; i < (int)x.size(); i++) {
        cell_x = int((x[i] + x_offset) * GRIDX_FACTOR);
        cell_y = int((y[i] + y_offset) * GRIDY_FACTOR);
        assert(cell_x + cell_y * GRID_W <= GRID_H * GRID_W);
        grid.xGrid[cell_x + cell_y * GRID_W] = x[i];
        grid.yGrid[cell_x + cell_y * GRID_W] = y[i];
        grid.intensity[cell_x + cell_y * GRID_W]++;
    }
}

inline void autoPlotReducer(const mglData& x, const mglData& y, std::vector<double>& xV, std::vector<double>& yV, mglData& xD, mglData& yD) {
    mglData minimalXRepart = x.Hist(x.GetNx());
    mglData minimalYRepart = y.Hist(y.GetNx());

    double* arrayX = minimalXRepart.a;
    std::vector<double> minimalXRepartV(arrayX, arrayX + minimalXRepart.GetNx());
    std::erase_if(minimalXRepartV, [](double x) { return std::abs(x) < std::numeric_limits<double>::epsilon(); });

    double* arrayY = minimalYRepart.a;
    std::vector<double> minimalYRepartV(arrayY, arrayY + minimalYRepart.GetNx());
    std::erase_if(minimalYRepartV, [](double y) { return std::abs(y) < std::numeric_limits<double>::epsilon(); });

    mglData* workDataArray = minimalYRepartV.size() >= minimalYRepartV.size() ? &minimalXRepart : &minimalYRepart;

    xV.reserve(workDataArray->GetNx());
    yV.reserve(workDataArray->GetNx());
    for (auto t = 0; t <= workDataArray->GetNx(); t++) {
        if (std::abs(workDataArray->a[t]) > std::numeric_limits<double>::epsilon()) {
            xV.push_back(x.a[t]);
            yV.push_back(y.a[t]);
        }
    }
    xV.shrink_to_fit();
    yV.shrink_to_fit();
    xD.Link(xV.data(), xV.size());
    yD.Link(yV.data(), yV.size());
}

/**
 * @brief Plot final Image Plane
 *
 * @param RayList Data (Rays)
 * @param plotName
 */
void plotSingle(const std::vector<Ray>& RayList, const std::string& plotName, const std::vector<OpticalElementMeta>& OpticalElementsMeta) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    ////////////////// Sort Data for plotting
    std::vector<double> Xpos, Ypos;
    Xpos.reserve(RayList.size());
    Ypos.reserve(RayList.size());

    // Fetch names
    std::vector<std::string> opticalElementNames;
    for (const auto& i : OpticalElementsMeta) {
        opticalElementNames.push_back(i.name);
    }

    // Get elements that met Image plane (Last Element)
    auto max = std::max_element(RayList.begin(), RayList.end(), comp);

    auto max_param = last_obj(max->m_extraParam);
    if (max_param != (int)opticalElementNames.size()) {
        RAYX_ERR << "No ray has hit the final optical element : " << opticalElementNames.back();
    }
    // Create new RayList with right order
    for (auto r : RayList) {
#if defined(LONGEST_PATH)
        if (int_close(r.m_extraParam, max->m_extraParam)) {
#else
        if (last_obj(r.m_extraParam) == max_param) {
#endif
            Xpos.push_back(r.m_position.x);
            Ypos.push_back(r.m_position.y);
        }
    }
    auto minX = *(std::min_element(Xpos.begin(), Xpos.end()));
    auto minY = *(std::min_element(Ypos.begin(), Ypos.end()));
    auto maxX = *(std::max_element(Xpos.begin(), Xpos.end()));
    auto maxY = *(std::max_element(Ypos.begin(), Ypos.end()));

    ////////////////// Data processing and binning

    mglData x_mgl, y_mgl;
    x_mgl.Link(Xpos.data(), Xpos.size());
    y_mgl.Link(Ypos.data(), Ypos.size());

    // Data Distribution
    mglData minimalXRepart = x_mgl.Hist(Xpos.size());
    mglData minimalYRepart = y_mgl.Hist(Ypos.size());
    // Binning
    mglData xmini_mgl, ymini_mgl;
    myGrid grid;
    plotReducer(Xpos, Ypos, abs(minX) + abs(maxX), abs(minY) + abs(maxY), abs(minX), abs(minY), grid);
    // autoPlotReducer(x_mgl,y_mgl,xminiVec, yminiVec, xmini_mgl,ymini_mgl);

    xmini_mgl.Link(&grid.xGrid[0], grid.xGrid.size());
    ymini_mgl.Link(&grid.yGrid[0], grid.yGrid.size());

    // xmini_mgl.Link(xminiVec.data(), xminiVec.size());
    // ymini_mgl.Link(yminiVec.data(), yminiVec.size());

    ////////////////// Plotting
    {
        float margin = 0.05;
        struct Ranges {
            float xmin, xmax, ymin, ymax;
        } ranges;  // set nice limites according to margin (matplotlib-like)
        ranges.xmin = minX - abs(minX * margin);
        ranges.xmax = maxX + abs(maxX * margin);
        ranges.ymin = minY - abs(minY * margin);
        ranges.ymax = maxY + abs(maxY * margin);

        // Sanity check
        // for (size_t i = 0; i <= Xpos.size(); i++) {
        //     assert((Xpos[i] == x[i]));
        // }
        mglGraph gr;

        std::string title = plotName;

        // Ignore Tex symbols
        while (title.find("_") != std::string::npos) title = title.replace(title.find("_"), 1, "-");
        // gr.SetQuality(8);
        gr.SetSize(1920, 1080);  // TODO : This should be changed once we have Windowing lib

        gr.MultiPlot(3, 3, 3, 2, 2, "UL");
        gr.SetRanges(ranges.xmin, ranges.xmax, ranges.ymin, ranges.ymax);
        gr.SetMarkSize(0.1);
        gr.Grid("xyzt", "=h");
        gr.SetFontSize(2.5);
        gr.Axis();
        gr.Plot(xmini_mgl, ymini_mgl, " o{x62C300}");

        // gr.Plot(x, y, " o{x62C300}");  // o-markers  green-yellow [Full Plot]
        gr.SetMarkSize(0.9);
        gr.Label('y', "y / mm", 0);
        gr.Label('x', "x / mm", 0);
        gr.Puts(mglPoint(maxX, minY), "Generated by RAY-X.^{2022}", ":C", 1);

        gr.MultiPlot(3, 3, 0, 2, 1, "LA");
        // TODO: Find a way to add sqrt(n) bin size
        auto xx = gr.Hist(x_mgl, minimalXRepart);
        xx.Norm(0, 1);
        gr.SetRanges(ranges.xmin, ranges.xmax, 0, 1);
        gr.Axis();
        gr.Box();
        gr.Step(xx);
        // gr.Area(xx, "E{x62C300}");

        gr.MultiPlot(3, 3, 2, 1, 1, "UR");
        auto outString = "Numbers of rays hit :" + std::to_string(Xpos.size()) + "\nImage Plane Name: " + opticalElementNames.back();
        gr.Puts(mglPoint(0, 0.5), outString.c_str(), "@", 4);

        gr.MultiPlot(3, 3, 5, 1, 2, "UR");
        gr.Title("Intensity", "", 2);
        gr.Rotate(180, 90);  // Bug in Barh..
        gr.SetRanges(ranges.ymin, ranges.ymax, 0, 1);
        xx = gr.Hist(y_mgl, minimalYRepart);
        xx.Norm(0, 1);
        gr.Axis();
        gr.Box();
        gr.Step(xx);
        // gr.Area(xx, "E{x62C300}");

        gr.MultiPlot(3, 3, 1, 3, 1, "#");
        gr.Puts(mglPoint(0, 0.92), title.c_str());

        auto files = outputPlotPathHandler(plotName);

        gr.WriteFrame((files.first).c_str(), "Generated by RAY-X");   // save it
        gr.WriteFrame((files.second).c_str(), "Generated by RAY-X");  // save it
        RAYX_VERB << "Saved";
        RAYX_D_LOG << "in \"" << files.first << "\" and \"" << files.second << "\"";
    }
}

/**
 * @brief Plot for each intersection
 *
 * @param RayList Data (Rays)
 * @param plotName
 */
void plotMulti(const std::vector<Ray>& RayList, const std::string& plotName, const std::vector<OpticalElementMeta>& OpticalElementsMeta) {
    // s is sorted and unique extraParam values extracted
    auto s = RayList;

    std::sort(s.begin(), s.end(), comp);  // Should be a fast enough sort
    s.erase(
        std::unique(s.begin(), s.end(),
                    [](Ray const& lhs, Ray const& rhs) { return abs(lhs.m_extraParam - rhs.m_extraParam) < std::numeric_limits<double>::epsilon(); }),
        s.end());
    auto uniqueCount = s.size();

    // Subplot in cols x cols
    int cols = (int)std::ceil(std::sqrt(uniqueCount)) + 1;

    std::vector<double> Xpos, Ypos, Zpos;
    Xpos.reserve(RayList.size());
    Ypos.reserve(RayList.size());
    Zpos.reserve(RayList.size());

    double _size = 0.5;
    int i = 0;
    float _percent;
    // matplotlibcpp::figure_size(cols * 500, (cols - 1) * 500);

    double minX, minY, minZ, maxX, maxY, maxZ;

    float margin = 0.05;
    struct Ranges {
        float xmin, xmax, ymin, ymax, zmin, zmax;
    } ranges;  // set nice limites according to margin (matplotlib-like)

    mglData x, y, z;
    mglGraph gr;
    gr.SetSize(cols * 400, (cols - 1) * 400);  // TODO : This should be changed once we have Windowing lib
    gr.SetFontSize(2.5);

    enum PlotAxes { XY, XZ };
    PlotAxes axes;
    for (auto u : s) {
        if ((last_obj(u.m_extraParam) == 0) || (OpticalElementsMeta[last_obj(u.m_extraParam) - 1].material == -2))
            axes = XZ;
        else
            axes = XY;

        for (auto r : RayList) {
            if (int_close(r.m_extraParam, u.m_extraParam)) {
                Xpos.push_back(r.m_position.x);
                Ypos.push_back(r.m_position.y);
                Zpos.push_back(r.m_position.z);
            }
        }

        if (Xpos.size() > 50) {
            _size = 0.2;
        } else
            _size = 0.5;

        _percent = (float)Xpos.size() / (float)RayList.size() * 100;

        minX = *(std::min_element(Xpos.begin(), Xpos.end()));
        minY = *(std::min_element(Ypos.begin(), Ypos.end()));
        minZ = *(std::min_element(Zpos.begin(), Zpos.end()));
        maxX = *(std::max_element(Xpos.begin(), Xpos.end()));
        maxY = *(std::max_element(Ypos.begin(), Ypos.end()));
        maxZ = *(std::max_element(Zpos.begin(), Zpos.end()));

        ranges.xmin = minX - abs(minX * margin);
        ranges.xmax = maxX + abs(maxX * margin);
        ranges.ymin = minY - abs(minY * margin);
        ranges.ymax = maxY + abs(maxY * margin);
        ranges.zmin = minZ - abs(minZ * margin);
        ranges.zmax = maxZ + abs(maxZ * margin);

        x.Link(Xpos.data(), Xpos.size());
        y.Link(Ypos.data(), Ypos.size());
        z.Link(Zpos.data(), Zpos.size());

        gr.SubPlot(cols, cols, cols + i, "_");
        gr.Title(std::string(std::to_string((int)u.m_extraParam) + " (" + std::to_string(_percent) + "%)").c_str(), "", 4);
        gr.SetMarkSize(_size);
        gr.Grid("xyzt", "=h");
        gr.Axis();

        // No intersection or is reflective
        if (axes == XZ) {
            gr.SetRanges(ranges.xmin, ranges.xmax, ranges.zmin, ranges.zmax);
            gr.Plot(x, z, " o{x62C300}");  // o-markers  green-yellow
        } else {
            gr.SetRanges(ranges.xmin, ranges.xmax, ranges.ymin, ranges.ymax);
            gr.Plot(x, y, " o{x62C300}");  // o-markers  green-yellow
        }

        i += 1;
        Xpos.clear();
        Ypos.clear();
        Zpos.clear();
    }

    gr.MultiPlot(1, cols, 0, 1, 1, "#");
    auto title = plotName;
    title = title + "\nMultiplot\n";
    int e = 1;
    title += "0: No intersection, ";
    for (const auto& element : OpticalElementsMeta) {
        title += std::to_string(e) + ": " + element.name + ((e % 5 != 0) ? ", " : ",\n");
        e++;
    }
    // Ignore Tex symbols
    while (title.find('_') != std::string::npos) title = title.replace(title.find('_'), 1, "-");
    gr.Title(title.c_str());

    auto files = outputPlotPathHandler(plotName);

    gr.WriteFrame((files.first).c_str(), "Generated by RAY-X");   // save it
    gr.WriteFrame((files.second).c_str(), "Generated by RAY-X");  // save it
    RAYX_VERB << "Saved";
    RAYX_D_LOG << "in \"" << files.first << "\" and \"" << files.second << "\"";
}

/**
 * @brief Plot the Data
 *
 * @param plotType Plot type, please check ENUM
 * @param plotName Plot Name
 * @param RayList Data to be plotted
 */
void Plotter::plot(int plotType, const std::string& plotName, const std::vector<Ray>& RayList, const std::unique_ptr<RAYX::Beamline>& beamline) {
    RAYX_LOG << "Plotting...";

    // Get Beamline Meta Data (Name, type etc.)
    auto OpticalElementsMeta = getBeamlineOpticalElementMeta(beamline);

    // Plot with correct option
    if (plotType == plotTypes::SinglePlot)  // RAY-UI
        plotSingle(RayList, plotName, OpticalElementsMeta);
    else if (plotType == plotTypes::MultiPlot)
        plotMulti(RayList, plotName, OpticalElementsMeta);
    else
        RAYX_D_ERR << "Plot Type not supported";
}

}  // namespace RAYX
