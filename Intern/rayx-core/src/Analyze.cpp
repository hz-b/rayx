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
double variance(R&& r, const double avg) {
    const auto distanceToAverage = [=] (const auto v) { return v - avg; };
    const auto square = [] (const auto v) { return v * v; };
    auto distanceToAverageSquared = r | std::views::transform(distanceToAverage) | std::views::transform(square);
    return average(distanceToAverageSquared);
}

template <std::ranges::range R>
double rootMeanSquare(R&& r) {
    auto squared = r | std::views::transform([](const auto v) { return v * v; });
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
void debugPromptIndexOutOfHistogramBounds([[maybe_unused]] R&& histogramIndices, [[maybe_unused]] F&& inHistogramBounds) {
#if defined(RAYX_DEBUG_MODE)
    // depending on how histogram min and max are chosen, values may be outside of the histogram bounds
    const auto negate = [](const bool b) { return !b; };
    auto numOutOfBounds = std::ranges::distance(histogramIndices | std::views::transform(inHistogramBounds) | std::views::filter(negate));
    if (numOutOfBounds)
        RAYX_VERB << "events discarded while creating histogram. number of discarded events, that are out of bounds of histogram: " << numOutOfBounds;
#endif
}

template <std::ranges::range R>
Analysis::Parameter::Histogram::Bins createHistogramBins(R&& r, const double min, const double max, const int numBins) {
    const auto valueToHistogramBinIndexBaked = [=](double v) { return valueToHistogramBinIndex(v, min, max, numBins); };
    auto histogramIndices = r | std::views::transform(valueToHistogramBinIndexBaked);
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
Analysis::Histogram2D::Bins createHistogram2DBins(R&& r, const glm::vec2 min, const glm::dvec2 max, const glm::ivec2 numBins) {
    const auto valueToHistogramBinCoord = [=](const glm::dvec2 v) {
        return glm::ivec2{
            valueToHistogramBinIndex(v.x, min.x, max.x, numBins.x),
            valueToHistogramBinIndex(v.y, min.y, max.y, numBins.y),
        };
    };

    auto histogramCoords = r | std::views::transform(valueToHistogramBinCoord);
    const auto inHistogramBounds = [=](const glm::ivec2 v) { return 0 <= v.x && v.x < numBins.x && 0 <= v.y && v.y < numBins.y; };

    debugPromptIndexOutOfHistogramBounds(histogramCoords, inHistogramBounds);

    const auto coordToHistogramIndex = [=](const glm::ivec2 v) { return v.x + v.y * numBins.x; };
    auto bins = std::vector<int>(numBins.x * numBins.y, 0);
    const auto accumulate = [&](int i) { ++bins[i]; };
    std::ranges::for_each(histogramCoords | std::views::filter(inHistogramBounds) | std::views::transform(coordToHistogramIndex), accumulate);
    return bins;
}

template <std::ranges::range R>
Analysis::Histogram2D createHistogram2D(R&& r, const glm::dvec2 min, const glm::dvec2 max, const glm::ivec2 numBins) {
    const auto [sanitizedMin, sanitizedMax] = sanitizeHistogramMinMax(min, max);
    const auto interval = (sanitizedMax - sanitizedMin) / glm::dvec2(numBins);
    const auto bins = createHistogram2DBins(r, sanitizedMin, sanitizedMax, numBins);
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
    auto binsReverse = histogram.bins | std::views::reverse;

    const auto max = *std::ranges::max_element(histogram.bins);
    const auto isLessThanHalfMax = [=](const double a) { return a < max / 2.0; };
    const auto leftBinIt = std::ranges::find_if_not(histogram.bins, isLessThanHalfMax);
    const auto rightBinIt = std::ranges::find_if_not(binsReverse, isLessThanHalfMax);
    const auto leftBinIndex = static_cast<int>(std::distance(std::ranges::begin(histogram.bins), leftBinIt));
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
    const auto var = variance(r, avg);
    const auto rms = rootMeanSquare(r);
    const auto [min, max] = minMax(r);
    const auto histogram = createHistogram(r, min, max, numBins);

    return {
        .avg = avg,
        .var = var,
        .rms = rms,
        .min = min,
        .max = max,
        .fwhm = fullWidthHalfMax(histogram),
        .histogram = histogram,
    };
}

char minimalisticHistogramValueToChar(const int value, const int maxValue) {
    const auto MinimalisticHistogramChars = std::string("0123456789");
    const auto numChars = static_cast<int>(MinimalisticHistogramChars.size());
    return MinimalisticHistogramChars[std::min(numChars - 1, (value * numChars) / maxValue)];
}

void densityPlot(const Analysis::Parameter::Histogram& histogram) {
    auto bins = std::views::all(histogram.bins);
    const auto max = *std::ranges::max_element(bins);
    const auto toChar = [=](const int v) { return minimalisticHistogramValueToChar(v, max); };
    const auto plot = [](const char c) mutable { std::cout << c; };
    std::ranges::for_each(bins | std::views::transform(toChar), plot);
    std::cout << std::endl;
}

void densityPlot(const Analysis::Histogram2D& histogram) {
    auto bins = std::views::all(histogram.bins);
    const auto max = *std::ranges::max_element(bins);
    const auto toChar = [=](const int v) { return minimalisticHistogramValueToChar(v, max); };
    const auto plot = [i = 0, width = histogram.width](const char c) mutable {
        std::cout << c;
        if (++i % width == 0) std::cout << std::endl;
    };
    std::ranges::for_each(bins | std::views::transform(toChar), plot);
}

template <std::ranges::range R>
void dump(const std::string name, const Analysis::Parameter& param, R&& r) {
    RAYX_VERB << "analyzing events for parameter: " << name;
    RAYX_VERB << "\tavg: " << param.avg;
    RAYX_VERB << "\tvar: " << param.var;
    RAYX_VERB << "\trms: " << param.rms;
    RAYX_VERB << "\tmin: " << param.min;
    RAYX_VERB << "\tmax: " << param.max;
    RAYX_VERB << "\tfwhm value (left): " << param.fwhm.left;
    RAYX_VERB << "\tfwhm value (right): " << param.fwhm.right;
    RAYX_VERB << "\tbandwidth: " << (param.fwhm.right - param.fwhm.left);
    RAYX_VERB << "\thistogram bin interval: " << param.histogram.interval;
    RAYX_VERB << "\thistogram min: " << param.histogram.min;
    RAYX_VERB << "\thistogram max: " << param.histogram.max;
    RAYX_VERB << "\tminimalistic histogram (values from '0' to 'highest value in histogram' map to values from '0' to '9' here): ";
    if (getDebugVerbose()) {
        const auto minimalisticHistogram = createHistogram(r, param.histogram.min, param.histogram.max, 80);
        assert(minimalisticHistogram.min == param.histogram.min);
        assert(minimalisticHistogram.max == param.histogram.max);
        densityPlot(minimalisticHistogram);
    }
}

template <std::ranges::range R>
void dump(const std::string name, const Analysis::Histogram2D& histogram, R&& r) {
    RAYX_VERB << "analyzing events for 2D histogram: " << name;
    RAYX_VERB << "\thistogram bin interval x: " << histogram.interval.x;
    RAYX_VERB << "\thistogram bin interval y: " << histogram.interval.y;
    RAYX_VERB << "\thistogram min x: " << histogram.min.x;
    RAYX_VERB << "\thistogram min y: " << histogram.min.y;
    RAYX_VERB << "\thistogram max x: " << histogram.max.x;
    RAYX_VERB << "\thistogram max y: " << histogram.max.y;
    RAYX_VERB << "\thistogram width: " << histogram.width;
    RAYX_D_VERB << "\tminimalistic histogram (values from '0' to 'highest value in histogram' map to values from '0' to '9' here): ";
    if (getDebugVerbose()) {
        const auto numBins2D = glm::ivec2(10);
        const auto minimalisticHistogram = createHistogram2D(r, histogram.min, histogram.max, numBins2D);
        assert(minimalisticHistogram.min == histogram.min);
        assert(minimalisticHistogram.max == histogram.max);
        densityPlot(minimalisticHistogram);
    }
}

}  // unnamed namespace

#if 0

template <std::ranges::range R>
Analysis analyzeElement(R&& r) {
    Analysis analysis;

    constexpr int numBins = 100;

    {
        auto attrEnergy = r | std::views::transform([](const Ray& r) { return r.m_energy; });
        analysis.energy = analyzeParameter(attrEnergy, numBins);
        dump("energy", analysis.energy, attrEnergy);
    }
    {
        auto attrPositionX = r | std::views::transform([](const Ray& r) { return r.m_position.x; });
        analysis.positionX = analyzeParameter(attrPositionX, numBins);
        dump("position.x", analysis.positionX, attrPositionX);
    }
    {
        auto attrPositionY = r | std::views::transform([](const Ray& r) { return r.m_position.y; });
        analysis.positionY = analyzeParameter(attrPositionY, numBins);
        dump("position.y", analysis.positionY, attrPositionY);
    }
    {
        const auto getAttrPosition = [](const Ray& r) { return r.m_position; };
        const auto toVec2 = [](const glm::dvec3 v) { return glm::dvec2{v.x, v.y}; };
        auto attrPosition = r | std::views::transform(getAttrPosition) | std::views::transform(toVec2);
        const auto min2D = glm::dvec2(analysis.positionX.min, analysis.positionY.min);
        const auto max2D = glm::dvec2(analysis.positionX.max, analysis.positionY.max);
        const auto numBins2D = glm::ivec2(numBins);
        analysis.positionHistogram = createHistogram2D(attrPosition, min2D, max2D, numBins2D);
        dump("position", analysis.positionHistogram, attrPosition);
    }

    // TODO: compare performance using std::execution::par

    return analysis;
}

std::vector<Analysis> analyze(const Group& beamline, const BundleHistory& bundleHistory) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<Analysis> analysis;

    const auto sourceRays = beamline.compileSources(1);
    RAYX_VERB << "analyzing sources";
    analysis.push_back(analyzeElement(std::views::all(sourceRays)));

    auto elementNames = std::vector<std::string>(beamline.numElements());
    beamline.ctraverse([i = 0, &elementNames](const BeamlineNode& node) mutable {
        if (node.isElement()) elementNames[i++] = static_cast<const DesignElement*>(&node)->getName();
        return false;
    });

    for (int i = 0; i < static_cast<int>(beamline.numElements()); ++i) {
        RAYX_VERB << "analyzing element(" << i << "): " << elementNames[i];
        const auto events = std::views::all(bundleHistory);
        const auto hasEventForElement = [=](const RayHistory& events) { return i < static_cast<int>(events.size()); };
        const auto getEventForElement = [=](const RayHistory& events) { return events[i]; };
        const auto analysisForElement = analyzeElement(events | std::views::filter(hasEventForElement) | std::views::transform(getEventForElement));
        analysis.push_back(analysisForElement);
    }

    return analysis;
}

#else

struct RayData {
    std::vector<double> energy;
    std::vector<double> positionX;
    std::vector<double> positionY;
};

Analysis analyzeElement(const RayData& events) {
    Analysis analysis;

    constexpr int numBins = 100;

    {
        auto attrEnergy = std::views::all(events.energy);
        analysis.energy = analyzeParameter(attrEnergy, numBins);
        dump("energy", analysis.energy, attrEnergy);
    }
    {
        auto attrPositionX = std::views::all(events.positionX);
        analysis.positionX = analyzeParameter(attrPositionX, numBins);
        dump("position.x", analysis.positionX, attrPositionX);
    }
    {
        auto attrPositionY = std::views::all(events.positionY);
        analysis.positionY = analyzeParameter(attrPositionY, numBins);
        dump("position.y", analysis.positionY, attrPositionY);
    }
    {
        // until we use c++23, we use a custom little zip view adaptor
        const auto getAttrPosition = [&](const int i) { return glm::dvec2(events.positionX[i], events.positionY[i]); };
        auto indices = std::views::iota(0, static_cast<int>(events.positionX.size()));
        auto attrPosition = indices | std::views::transform(getAttrPosition);
        const auto min2D = glm::dvec2(analysis.positionX.min, analysis.positionY.min);
        const auto max2D = glm::dvec2(analysis.positionX.max, analysis.positionY.max);
        const auto numBins2D = glm::ivec2(numBins);
        analysis.positionHistogram = createHistogram2D(attrPosition, min2D, max2D, numBins2D);
        dump("position", analysis.positionHistogram, attrPosition);
    }

    // TODO: compare performance using std::execution::par

    return analysis;
}

std::vector<RayData> transpose(const int numElements, const BundleHistory& bundleHistory) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    auto result = std::vector<RayData>(beamline.numElements());

    for (int b = 0; b < static_cast<int>(bundleHistory.size()); ++b) {
        const RayHistory& path = bundleHistory[b];
        for (int h = 0; h < static_cast<int>(path.size()); ++h) {
            const Ray& ray = path[h];
            result[h].energy.push_back(ray.m_energy);
            result[h].positionX.push_back(ray.m_position.x);
            result[h].positionY.push_back(ray.m_position.y);
        }
    }

    return result;
}

std::vector<Analysis> analyze(const Group& beamline, const BundleHistory& bundleHistory) {
    const auto numElements = static_cast<int>(beamline.numElements());
    const auto data = transpose(numElements, bundleHistory);

    RAYX_PROFILE_FUNCTION_STDOUT();

    std::vector<Analysis> analysis;

    // const auto sourceRays = beamline.compileSources(1);
    // RAYX_VERB << "analyzing sources";
    // analysis.push_back(analyzeElement(std::views::all(sourceRays)));

    auto elementNames = std::vector<std::string>(numElements);
    // beamline.ctraverse([i = 0, &elementNames](const BeamlineNode& node) mutable {
    //     if (node.isElement()) elementNames[i++] = static_cast<const DesignElement*>(&node)->getName();
    //     return false;
    // });

    for (int i = 0; i < static_cast<int>(numElements); ++i) {
        RAYX_VERB << "analyzing element(" << i << "): " << elementNames[i];
        // const auto hasEventForElement = [=](const RayHistory& events) { return i < static_cast<int>(events.size()); };
        // const auto getEventForElement = [=](const RayHistory& events) { return events[i]; };
        const auto analysisForElement = analyzeElement(data[i]);
        analysis.push_back(analysisForElement);
    }

    return analysis;
}

#endif

}  // namespace RAYX
