#include "Plotter/Plotter.h"

#include <matplotlibcpp.h>

#include <algorithm>
#include <chrono>

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

void testMathGL() {
    mglData dat(30, 40);  // data to for plotting
    for (long i = 0; i < 30; i++)
        for (long j = 0; j < 40; j++) dat.a[i + 30 * j] = 1 / (1 + (i - 15) * (i - 15) / 225. + (j - 20) * (j - 20) / 400.);
    mglGraph gr;                  // class for plot drawing
    gr.Rotate(50, 60);            // rotate axis
    gr.Light(true);               // enable lighting
    gr.Surf(dat);                 // plot surface
    gr.Cont(dat, "y");            // plot yellow contour lines
    gr.Axis();                    // draw axis
    gr.WriteFrame("sample.svg");  // save it
    mglData data;
    std::vector<double> dataVector = {1, 2, 3, 4, 6};

    data.Set(dataVector);

    mglData dataTest(dataVector);
}

/**
 * @brief Plot the Data
 *
 * @param plotType Plot type, please check ENUM
 * @param plotName Plot Name
 * @param RayList Data to be plotted
 */
void Plotter::plot(int plotType, const std::string& plotName, const std::vector<Ray>& RayList, const std::vector<std::string>& OpticalElementNames) {
    RAYX_LOG << "Plotting...";
    if (plotType == plotTypes::LikeRAYUI)  // RAY-UI
        plotLikeRAYUI(RayList, plotName, OpticalElementNames);
    else if (plotType == plotTypes::ForEach)
        RAYX_D_ERR << "Plot Type not supported";
    else if (plotType == plotTypes::Eachsubplot)
        plotforEach(RayList, plotName, OpticalElementNames);
}

/**
 * @brief Get the amount of bins for the histogram
 * FREEDMAN is a different method to get the "optimized" amount of bins
 *
 * @param vec Input Data Vector
 * @return int Bin Amount
 */
int Plotter::getBinAmount(std::vector<double>& vec) {
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
std::pair<std::string, std::string> outputPlotPathHandler(void) {
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
    std::string sampleName = "sample1";  // Change filename
    auto outputSvg = outputDir;
    auto outputPng = outputDir;
    outputSvg.append(sampleName + ".svg");
    outputPng.append(sampleName + ".png");
    return std::make_pair<std::string, std::string>(outputSvg.c_str(), outputPng.c_str());
}

/**
 * @brief Plot final Image Plane
 *
 * @param RayList Data (Rays)
 * @param plotName
 */
void Plotter::plotLikeRAYUI(const std::vector<Ray>& RayList, const std::string& plotName, const std::vector<std::string>& OpticalElementNames) {
    ////////////////// Sort Data for plotting
    std::vector<double> Xpos, Ypos;
    Xpos.reserve(RayList.size());
    Ypos.reserve(RayList.size());
    // Get elements that met Image plane (Last Element)
    auto max = std::max_element(RayList.begin(), RayList.end(), comp);

    auto max_param = last_obj(max->m_extraParam);
    if (max_param != (int)OpticalElementNames.size()) {
        RAYX_ERR << "No ray has hit the final optical element : " << OpticalElementNames.back();
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

    ////////////////// Plotting
    {
        float margin = 0.05;
        struct Ranges {
            float xmin, xmax, ymin, ymax;
        } ranges;  // set nice limites according to margin (matplotlib-like)
        ranges.xmin = minX - abs(minX * margin);
        ranges.xmax = maxX + abs(maxX * margin);
        ranges.ymin = minY - abs(minY * margin);
        ranges.ymax = maxY + (maxY * margin);

        mglData x, y;
        x.Link(Xpos.data(), Xpos.size());
        y.Link(Ypos.data(), Ypos.size());

        // Sanity check
        // for (size_t i = 0; i <= Xpos.size(); i++) {
        //     assert((Xpos[i] == x[i]));
        // }
        mglGraph gr;

        std::string title = plotName;
        title += "\n" + OpticalElementNames.back();
        // Ignore Tex
        while (title.find("_") != std::string::npos) title = title.replace(title.find("_"), 1, "-");

        gr.SetSize(1920, 1080);  // TODO : This should be changed once we have Windowing lib

        gr.MultiPlot(3, 3, 3, 2, 2, "UL");
        gr.SetRanges(ranges.xmin, ranges.xmax, ranges.ymin, ranges.ymax);
        gr.SetMarkSize(0.1);
        gr.Grid("xyzt", "=h");
        gr.SetFontSize(2.5);
        gr.Axis();

        gr.Plot(x, y, " o{x62C300}");  // o-markers  green-yellow
        gr.SetMarkSize(0.9);
        gr.AddLegend("Ray", " o{x62C300}");
        gr.Legend(3, "#", "0.05");
        gr.Label('y', "y / mm", 0);
        gr.Label('x', "x / mm", 0);
        gr.Puts(mglPoint(0, minY), "Generated by RAY-X.^{2022}", ":C", 1);

        gr.MultiPlot(3, 3, 0, 2, 1, "LA");
        // TODO: Find a way to add sqrt(n) bin size
        float n = Xpos.size();
        auto fx = x.Hist(n);
        auto xx = gr.Hist(x, fx);
        xx.Norm(0, 1);
        gr.SetRanges(ranges.xmin, ranges.xmax, 0, 1);
        gr.Axis();
        gr.Box();
        // gr.Step(xx);
        gr.Area(xx, "E{x62C300}");

        gr.MultiPlot(3, 3, 5, 1, 2, "UR");
        gr.Title("Intensity", "", 2);
        gr.Rotate(180, 90);  // TODO: Bug in Barh..
        gr.SetRanges(ranges.ymin, ranges.ymax, 0, 1);
        fx = y.Hist(n);
        xx = gr.Hist(y, fx);
        xx.Norm(0, 1);
        gr.Axis();
        gr.Box();
        gr.Area(xx, "E{x62C300}");

        gr.MultiPlot(3, 3, 1, 3, 1, "#");
        gr.Puts(mglPoint(0, 0.92), title.c_str());

        auto files = outputPlotPathHandler();

        gr.WriteFrame((files.first).c_str(), "Generated by RAY-X");   // save it
        gr.WriteFrame((files.second).c_str(), "Generated by RAY-X");  // save it
        RAYX_VERB << "Saved";
        RAYX_D_LOG << "in \"" << files.first << "\" and \"" << files.second << "\"";
    }
    // Start plot
    // std::string title = plotName;
    // matplotlibcpp::figure_size(1920, 1080);
    // title += "\n" + OpticalElementNames.back();

    // matplotlibcpp::suptitle(title);
    // matplotlibcpp::subplot2grid(4, 4, 0, 0, 1, 3);
    // matplotlibcpp::hist(Xpos, "sqrt", "#0062c3", 0.65, false, {{"density", "False"}, {"histtype", "step"}});

    // matplotlibcpp::subplot2grid(4, 4, 1, 0, 3, 3);
    // matplotlibcpp::scatter(Xpos, Ypos, 1, {{"color", "#62c300"}, {"label", "Ray(" + std::to_string(Xpos.size()) + ")"}});
    // matplotlibcpp::text(minX, minY, "Generated by RAY-X.");
    // matplotlibcpp::xlabel("x / mm");
    // matplotlibcpp::ylabel("y / mm");
    // matplotlibcpp::legend();
    // matplotlibcpp::grid(true);

    // matplotlibcpp::subplot2grid(4, 4, 1, 3, 3, 1);
    // matplotlibcpp::hist(Ypos, "sqrt", "#0062c3", 0.65, false, {{"density", "False"}, {"orientation", "horizontal"}, {"histtype", "step"}});
    // matplotlibcpp::title("Intensity");

    // matplotlibcpp::show();
}

/**
 * @brief Plot for each intersection
 *
 * @param RayList Data (Rays)
 * @param plotName
 */
void Plotter::plotforEach(const std::vector<Ray>& RayList, const std::string& plotName, const std::vector<std::string>& OpticalElementNames) {
    // s is sorted and unique extraParam values extracted
    auto s = RayList;

    std::sort(s.begin(), s.end(), comp);  // Should be a fast enough sort
    s.erase(
        std::unique(s.begin(), s.end(),
                    [](Ray const& lhs, Ray const& rhs) { return abs(lhs.m_extraParam - rhs.m_extraParam) < std::numeric_limits<double>::epsilon(); }),
        s.end());
    auto uniqueCount = s.size();

    // Subplot in cols x cols
    int cols = (int)std::ceil(std::sqrt(uniqueCount));

    std::vector<double> Xpos, Ypos;
    Xpos.reserve(RayList.size());
    Ypos.reserve(RayList.size());

    double _size = 2.0;
    int i = 1;
    float _percent;
    matplotlibcpp::figure_size(cols * 500, (cols - 1) * 500);

    for (auto u : s) {
        if (i / s.size() > 9 / 10) RAYX_VERB << "Almost there...";  // Wakeup up call (Temp)
        for (auto r : RayList) {
            if (int_close(r.m_extraParam, u.m_extraParam)) {
                Xpos.push_back(r.m_position.x);
                Ypos.push_back(r.m_position.y);
            }
        }

        if (Xpos.size() > 50) {
            _size = 1.0;
        }

        _percent = (float)Xpos.size() / (float)RayList.size() * 100;
        // RAYX_LOG << (int)u.m_extraParam << ":" << Xpos.size();
        matplotlibcpp::subplot(cols, cols, i);
        // TODO: A Call to PyObjectCall inside matplotlib-cpp can be slow depending on the data size
        matplotlibcpp::scatter(Xpos, Ypos, _size, {{"color", "#62c300"}, {"label", "Ray"}});
        matplotlibcpp::xlabel("x / mm");
        matplotlibcpp::ylabel("y / mm");
        matplotlibcpp::title(std::to_string((int)u.m_extraParam) + " (" + std::to_string(_percent) + "%)");

        matplotlibcpp::legend();

        i += 1;
        Xpos.clear();
        Ypos.clear();
    }
    matplotlibcpp::subplots_adjust({{"wspace", 0.35}, {"hspace", 0.35}});

    // matplotlibcpp::tight_layout();
    auto title = plotName;
    title = title + "Multiplot \n \n";
    int e = 1;
    for (const auto& element : OpticalElementNames) {
        title += " $\\it{" + std::to_string(e) + ": " + element + ((e % 3 != 0) ? " }$ ," : "}$\n");
        e++;
    }
    matplotlibcpp::suptitle(title);

    matplotlibcpp::show();
}
/**
 * @brief Plot and save custom benchmarked functions with -b arg
 *
 * @param BenchMap
 */
void Plotter::plotBenchmarks(const std::map<std::string, double>& BenchMap) {
    std::vector<double> times;
    std::vector<std::string> labels;
    std::vector<int> ticks(BenchMap.size());
    labels.reserve(BenchMap.size());
    times.reserve(BenchMap.size());

    std::iota(ticks.begin(), ticks.end(), 0);

    for (const auto& i : BenchMap) {
        labels.push_back(i.first);
        times.push_back(i.second);
    }

    // matplotlibcpp::figure_size(1300, 1000);
    RAYX_WARN << "plotBenchmarks not fully implemented";
    return;
    // matplotlibcpp::bar(times);
    // matplotlibcpp::xticks(ticks, labels);
    // matplotlibcpp::xlabel("Function Names");
    // matplotlibcpp::ylabel("Time (ms)");
    // matplotlibcpp::title("Benchmark results");
    // matplotlibcpp::save("Benchres");
}

}  // namespace RAYX
