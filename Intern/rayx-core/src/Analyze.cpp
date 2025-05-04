#include "Analyze.h"

#include <algorithm>
#include <numeric>
#include <ranges>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"

namespace RAYX {
namespace {

template <std::ranges::range R>
double average(R&& r) {
    const auto begin = std::ranges::begin(r);
    const auto end = std::ranges::end(r);
    return static_cast<double>(std::reduce(begin, end)) / std::ranges::distance(r);
}

template <std::ranges::range R>
auto rootMeanSquare(R&& r) {
    const auto squared = r | std::views::transform([](auto&& v) { return v * v; });
    return std::sqrt(average(squared));
}

template <std::ranges::range R>
auto minMax(R&& r) -> std::pair<std::ranges::range_value_t<R>, std::ranges::range_value_t<R>> {
    auto [minIt, maxIt] = std::ranges::minmax_element(r);
    return {*minIt, *maxIt};
}

int valueToHistogramBinIndex(const double value, const double min, const double max, const int numBins) {
    // TODO: currently using EPS to make indices are in bounds if value == max. use another approach that is more stable
    constexpr double EPS = 1.0 + 1e-6;
    return static_cast<int>(std::floor((value - min) / (max * EPS - min) * numBins));
}

std::pair<double, double> histogramBinIndexToInterval(const int index, const double min, const double max, const int numBins) {
    const auto interval = (max - min) / numBins;
    return {index * interval + min, (index + 1) * interval + min};
}

double histogramBinIndexToValue(const int index, const double min, const double max, const int numBins) {
    const auto [first, second] = histogramBinIndexToInterval(index, min, max, numBins);
    return (first + second) / 2.0;
}

/*
 * histogram
 */

auto sanitizeHistogramMinMax(const double min, const double max) -> std::pair<double, double> {
    // edge case: if min and max is equal, the histogram has zero width. To be able to have bins, we adjust the histogram boundaries to give it some
    // width
    const auto isMinMaxEqual = min == max;
    if (isMinMaxEqual) RAYX_D_VERB << "adjusting min and max for histogram because min == max";
    const auto histogramMinMaxOffset = 1.0;  // adjust by absolute value
    const auto sanitizedMin = min - (isMinMaxEqual ? histogramMinMaxOffset : 0.0);
    const auto sanitizedMax = max + (isMinMaxEqual ? histogramMinMaxOffset : 0.0);
    return {sanitizedMin, sanitizedMax};
}

auto sanitizeHistogramMinMax(const glm::dvec2 min, const glm::dvec2 max) -> std::pair<glm::dvec2, glm::dvec2> {
    const auto [sanitizedMinX, sanitizedMaxX] = sanitizeHistogramMinMax(min.x, max.x);
    const auto [sanitizedMinY, sanitizedMaxY] = sanitizeHistogramMinMax(min.y, max.y);
    const auto sanitizedMin = glm::dvec2(sanitizedMinX, sanitizedMinY);
    const auto sanitizedMax = glm::dvec2(sanitizedMaxX, sanitizedMaxY);
    return {sanitizedMin, sanitizedMax};
}

template <std::ranges::range R, typename F>
void debugPromptIndexOutOfHistogramBounds(R&& histogramIndices, F&& inHistogramBounds) {
#if defined(RAYX_DEBUG_MODE)
    // depending on how histogram min and max are chosen, values may be outside of the histogram bounds
    const auto negate = [](const bool b) { return !b; };
    const auto numOutOfBounds = std::ranges::distance(histogramIndices | std::views::transform(inHistogramBounds) | std::views::filter(negate));
    if (numOutOfBounds)
        RAYX_VERB << "events discarded while creating histogram. number of discarded events, that are out of bounds of histogram: " << numOutOfBounds;
#endif
}

template <std::ranges::range R>
Analysis::Parameter::Histogram::Bins createHistogramBins(R&& r, const double min, const double max, const int numBins) {
    const auto valueToHistogramBinIndexBaked = [=](double v) { return valueToHistogramBinIndex(v, min, max, numBins); };
    const auto histogramIndices = r | std::views::transform(valueToHistogramBinIndexBaked);
    const auto inHistogramBounds = [=](int i) { return 0 <= i && i < numBins; };

    debugPromptIndexOutOfHistogramBounds(histogramIndices, inHistogramBounds);

    auto bins = std::vector<int>(numBins, 0);
    const auto accumulate = [&](int i) { ++bins[i]; };
    std::ranges::for_each(histogramIndices | std::views::filter(inHistogramBounds), accumulate);
    return bins;
}

template <std::ranges::range R>
Analysis::Parameter::Histogram createHistogram(R&& r, const double min, const double max, const int numBins) {
    const auto [sanitizedMin, sanitizedMax] = sanitizeHistogramMinMax(min, max);
    const auto interval = (sanitizedMax - sanitizedMin) / numBins;
    const auto bins = createHistogramBins(r, sanitizedMin, sanitizedMax, numBins);
    return {
        .bins = bins,
        .interval = interval,
        .min = sanitizedMin,
        .max = sanitizedMax,
    };
}

template <std::ranges::range R>
Analysis::Histogram2D::Bins createPositionsHistogramBins(R&& events, const glm::vec2 min, const glm::dvec2 max, const glm::ivec2 numBins) {
    const auto getPosition = [](const Ray& ray) { return ray.m_position; };
    const auto toVec2 = [](const glm::dvec3 v) { return glm::dvec2{v.x, v.y}; };
    const auto valueToHistogramBinCoord = [=](const glm::dvec2 v) {
        return glm::ivec2{
            valueToHistogramBinIndex(v.x, min.x, max.x, numBins.x),
            valueToHistogramBinIndex(v.y, min.y, max.y, numBins.y),
        };
    };

    const auto histogramCoords =
        events | std::views::transform(getPosition) | std::views::transform(toVec2) | std::views::transform(valueToHistogramBinCoord);
    const auto inHistogramBounds = [=](const glm::ivec2 v) { return 0 <= v.x && v.x < numBins.x && 0 <= v.y && v.y < numBins.y; };

    debugPromptIndexOutOfHistogramBounds(histogramCoords, inHistogramBounds);

    const auto coordToHistogramIndex = [=](const glm::ivec2 v) { return v.x + v.y * numBins.x; };
    auto bins = std::vector<int>(numBins.x * numBins.y, 0);
    const auto accumulate = [&](int i) { ++bins[i]; };
    std::ranges::for_each(histogramCoords | std::views::filter(inHistogramBounds) | std::views::transform(coordToHistogramIndex), accumulate);
    return bins;
}

template <std::ranges::range R>
Analysis::Histogram2D createPositionsHistogram(R&& events, const glm::dvec2 min, const glm::dvec2 max, const glm::ivec2 numBins) {
    const auto [sanitizedMin, sanitizedMax] = sanitizeHistogramMinMax(min, max);
    const auto interval = (sanitizedMax - sanitizedMin) / glm::dvec2(numBins);
    const auto bins = createPositionsHistogramBins(events, sanitizedMin, sanitizedMax, numBins);
    return {
        .bins = bins,
        .interval = interval,
        .min = sanitizedMin,
        .max = sanitizedMax,
        .width = numBins.x,
    };
}

Analysis::Parameter::FullWidthHalfMax fullWidthHalfMax(const Analysis::Parameter::Histogram& histogram) {
    const auto numBins = std::ranges::distance(histogram.bins);
    const auto binsReverse = histogram.bins | std::views::reverse;

    const auto max = *std::ranges::max_element(histogram.bins);
    const auto isLessThanHalfMax = [=](const double a) { return a < max / 2.0; };
    const auto leftBinIt = std::ranges::find_if_not(histogram.bins, isLessThanHalfMax);
    const auto rightBinIt = std::ranges::find_if_not(binsReverse, isLessThanHalfMax);
    const auto leftBinIndex = static_cast<int>(std::ranges::distance(std::ranges::begin(histogram.bins), leftBinIt));
    const auto rightBinIndex = static_cast<int>(numBins - 1 - std::distance(std::ranges::begin(binsReverse), rightBinIt));
    const auto left = histogramBinIndexToValue(leftBinIndex, histogram.min, histogram.max, histogram.bins.size());
    const auto right = histogramBinIndexToValue(rightBinIndex, histogram.min, histogram.max, histogram.bins.size());

    return {
        .left = left,
        .right = right,
    };
}

template <std::ranges::range R>
Analysis::Parameter analyzeParameter(R&& r, const int numBins) {
    const auto avg = average(r);
    const auto rms = rootMeanSquare(r);
    const auto [min, max] = minMax(r);
    const auto histogram = createHistogram(r, min, max, numBins);

    return {
        .avg = avg,
        .rms = rms,
        .min = min,
        .max = max,
        .fwhm = fullWidthHalfMax(histogram),
        .histogram = histogram,
    };
}

void dump(const std::string name, const Analysis::Parameter& param) {
    RAYX_D_VERB << "analyzing events for parameter: " << name;
    if (getDebugVerbose()) RAYX_D_DBG(param.histogram.bins);
    RAYX_D_VERB << "\thistogram bin interval: " << param.histogram.interval;
    RAYX_D_VERB << "\thistogram min: " << param.histogram.min;
    RAYX_D_VERB << "\thistogram max: " << param.histogram.max;
    RAYX_D_VERB << "\tavg: " << param.avg;
    RAYX_D_VERB << "\trms: " << param.rms;
    RAYX_D_VERB << "\tmin: " << param.min;
    RAYX_D_VERB << "\tmax: " << param.max;
    RAYX_D_VERB << "\tfwhm value (left): " << param.fwhm.left;
    RAYX_D_VERB << "\tfwhm value (right): " << param.fwhm.right;
}

void dump(const std::string name, const Analysis::Histogram2D& histogram) {
    RAYX_D_VERB << "analyzing events for 2D histogram: " << name;
    if (getDebugVerbose()) RAYX_D_DBG(histogram.bins);
    RAYX_D_VERB << "\thistogram bin interval x: " << histogram.interval.x;
    RAYX_D_VERB << "\thistogram bin interval y: " << histogram.interval.y;
    RAYX_D_VERB << "\thistogram min x: " << histogram.min.x;
    RAYX_D_VERB << "\thistogram min y: " << histogram.min.y;
    RAYX_D_VERB << "\thistogram max x: " << histogram.max.x;
    RAYX_D_VERB << "\thistogram max y: " << histogram.max.y;
    RAYX_D_VERB << "\thistogram width: " << histogram.width;
}

}  // unnamed namespace

template <std::ranges::range R>
Analysis analyzeElement(R&& r) {
    Analysis analysis;

    constexpr int numBins = 100;

    analysis.energy = analyzeParameter(r | std::views::transform([](const Ray& r) { return r.m_energy; }), numBins);
    analysis.positionX = analyzeParameter(r | std::views::transform([](const Ray& r) { return r.m_position.x; }), numBins);
    analysis.positionY = analyzeParameter(r | std::views::transform([](const Ray& r) { return r.m_position.y; }), numBins);
    analysis.position = createPositionsHistogram(r, {analysis.positionX.min, analysis.positionY.min},
                                                 {analysis.positionX.max, analysis.positionY.max}, {10, 10});

    dump("energy", analysis.energy);
    dump("position.x", analysis.positionX);
    dump("position.y", analysis.positionY);
    dump("position", analysis.position);

    // TODO: compare performance using std::execution::par

    return analysis;
}

std::vector<Analysis> analyze(const Group& beamline, const BundleHistory& bundleHistory) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<Analysis> analysis;

    const auto sourceRays = beamline.compileSources(1);
    RAYX_D_VERB << "analyzing sources";
    analysis.push_back(analyzeElement(std::views::all(sourceRays)));

    // const auto element0 = std::views::all(bundleHistory) | std::views::filter([] (const RayHistory& events) { return true; }) | std::views::transform([] (const RayHistory& events) { return events[0]; });
    // analyzeElement(element0);

    // const auto events = std::views::all(bundleHistory);
    // for (int i = 0; i < beamline.numElements(); ++i) {
    //     RAYX_D_VERB << "analyzing element: " << i;
    //     analysis.push_back(analyzeElement(events | std::views::filter([=] (const RayHistory& events) { return i < events.size(); }) | std::views::transform([=] (const RayHistory& events) { return events[i]; })));
    // }

    return analysis;
}

}  // namespace RAYX
