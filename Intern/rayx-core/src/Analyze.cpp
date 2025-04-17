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
    return static_cast<double>(std::reduce(begin, end)) / std::ranges::size(r);
}

template <std::ranges::range R>
auto rootMeanSquare(R&& r) {
    const auto squared = r | std::views::transform([](auto&& v) { return v * v; });
    return std::sqrt(average(squared));
}

template <std::ranges::range R>
auto minMax(R&& r) -> std::pair<std::ranges::range_value_t<R>, std::ranges::range_value_t<R>> {
    auto [minIt, maxIt] = std::minmax_element(std::ranges::begin(r), std::ranges::end(r));
    return {*minIt, *maxIt};
}

int valueToHistogramBinIndex(const double value, const double min, const double max, const int numBins) {
    // TODO: currently using EPS to make indices are in bounds if value == max. use another approach that is more stable
    constexpr double EPS = 1.0 + 1e-6;
    return static_cast<int>(std::floor((value - min) / (max * EPS - min) * numBins));
}

std::pair<double, double> histogramBinIndexToBinInterval(const int index, const double min, const double max, const int numBins) {
    const auto interval = (max - min) / numBins;
    return {index * interval + min, (index + 1) * interval + min};
}

template <std::ranges::range R>
auto createHistogram(R&& r, const double min, const double max, const int numBins) {
    const auto histogramIndices = r | std::views::transform([=](double v) { return valueToHistogramBinIndex(v, min, max, numBins); });
    const auto inHistogramBounds = [=](int i) { return 0 <= i && i < numBins; };

#if defined(RAYX_DEBUG_MODE)
    int numOutOfBounds = 0;
    std::ranges::for_each(histogramIndices, [&](int i) {
        if (!inHistogramBounds(i)) ++numOutOfBounds;
    });
    if (numOutOfBounds) RAYX_WARN << "number of discarded events, that are out of bounds of histogram: " << numOutOfBounds;
#endif

    auto histogram = std::vector<int>(numBins, 0);
    std::ranges::for_each(histogramIndices | std::views::filter(inHistogramBounds), [&](int i) { ++histogram[i]; });

    return histogram;
}

template <std::ranges::range R>
auto fullWidthHalfMaxBins(R&& histogram, const int numBins) -> std::pair<std::ranges::range_value_t<R>, std::ranges::range_value_t<R>> {
    const auto histogramReverse = histogram | std::views::reverse;

    const auto maxElement = *std::max_element(std::ranges::begin(histogram), std::ranges::end(histogram));
    const auto lessThanHalfMaxElement = [=](const double a) { return a < maxElement / 2.0; };
    const auto firstBinIt = std::ranges::find_if_not(histogram, lessThanHalfMaxElement);
    const auto lastBinIt = std::ranges::find_if_not(histogramReverse, lessThanHalfMaxElement);
    const auto firstBin = static_cast<std::ranges::range_value_t<R>>(std::distance(std::ranges::begin(histogram), firstBinIt));
    const auto lastBin = static_cast<std::ranges::range_value_t<R>>(numBins - 1 - std::distance(std::ranges::begin(histogramReverse), lastBinIt));

    return {firstBin, lastBin};
}

template <std::ranges::range R>
Analysis::Parameter analyzeParameter(R&& r, const int numBins) {
    const auto avg = average(r);
    const auto rms = rootMeanSquare(r);
    const auto [min, max] = minMax(r);

    // edge case where min and max energy are equal. in order to be able to create a histogram, adjust min and max by the same distance
    // TODO: consider instead creating a single bin with min=max, so that it is clear, that all energies are identical
    const auto isMinMaxEqual = min == max;
    const auto histogramMinMaxOffset = 1.0;  // TODO: should we use an absolute value here? cannot use multiples of min in case min == 0
    const auto histogramMin = min - (isMinMaxEqual ? histogramMinMaxOffset : 0.0);
    const auto histogramMax = max + (isMinMaxEqual ? histogramMinMaxOffset : 0.0);
    const auto histogramInterval = (histogramMax - histogramMin) / numBins;
    if (isMinMaxEqual) RAYX_D_VERB << "adjusting min and max for histogram because min == max";

    const auto bins = createHistogram(r, histogramMin, histogramMax, numBins);
    const auto [fwhmFirstBinIndex, fwhmLastBinIndex] = fullWidthHalfMaxBins(std::views::all(bins), numBins);
    const auto fwhmFirst = histogramBinIndexToBinInterval(fwhmFirstBinIndex, histogramMin, histogramMax, numBins).first;
    const auto fwhmLast = histogramBinIndexToBinInterval(fwhmLastBinIndex, histogramMin, histogramMax, numBins).second;

    return {
        .avg = avg,
        .rms = rms,
        .min = min,
        .max = max,
        .fwhmFirst = fwhmFirst,
        .fwhmLast = fwhmLast,
        .histogram =
            {
                .bins = bins,
                .binInterval = histogramInterval,
                .min = histogramMin,
                .max = histogramMax,
            },
    };
}

void dump(const std::string name, const Analysis::Parameter& param) {
    RAYX_D_VERB << "analyzing events for parameter: " << name;
    if (getDebugVerbose()) RAYX_D_DBG(param.histogram.bins);
    RAYX_D_VERB << "\thistogram bin interval: " << param.histogram.binInterval;
    RAYX_D_VERB << "\thistogram min: " << param.histogram.min;
    RAYX_D_VERB << "\thistogram max: " << param.histogram.max;
    RAYX_D_VERB << "\tavg: " << param.avg;
    RAYX_D_VERB << "\trms: " << param.rms;
    RAYX_D_VERB << "\tmin: " << param.min;
    RAYX_D_VERB << "\tmax: " << param.max;
    RAYX_D_VERB << "\tfwhm value (left): " << param.fwhmFirst;
    RAYX_D_VERB << "\tfwhm value (right): " << param.fwhmLast;
}

}  // namespace

Analysis analyze(const Group& beamline) {
    RAYX_PROFILE_FUNCTION_STDOUT();

    Analysis analysis;

    // const auto sources = beamline.getSources();
    // std::transform(sources.begin(), sources.end(), std::back_inserter(analysis.sourcesAnalytic), [](const DesignSource* source) {
    //     return Analysis::Source{
    //         .energy = source->getElectronEnergy(),
    //         .bandwidth = 0.0,  // TODO
    //         .flux = source->getPhotonFlux(),
    //     };
    // });

    const int numBins = 100;
    const auto rays = beamline.compileSources(1);

    dump("energy", analyzeParameter(rays | std::views::transform([](const Ray& r) { return r.m_energy; }), numBins));
    dump("position.x", analyzeParameter(rays | std::views::transform([](const Ray& r) { return r.m_position.x; }), numBins));
    dump("direction.x", analyzeParameter(rays | std::views::transform([](const Ray& r) { return r.m_direction.x; }), numBins));

    // TODO: compare performance using std::execution::par

    return analysis;
}

}  // namespace RAYX
