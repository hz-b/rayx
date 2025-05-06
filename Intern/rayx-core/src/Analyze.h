#pragma once

#include "Beamline/Beamline.h"
#include "Tracer/DeviceTracer.h"

namespace RAYX {

struct Analysis {
    struct Parameter {
        struct Histogram {
            using Bins = std::vector<int>;

            Bins bins;
            double interval;
            double min;
            double max;
        };

        struct FullWidthHalfMax {
            double left;
            double right;
        };

        double avg;
        double rms;
        double min;
        double max;
        FullWidthHalfMax fwhm;
        Histogram histogram;
    };

    struct Histogram2D {
        using Bins = std::vector<int>;

        Bins bins;
        glm::dvec2 interval;
        glm::dvec2 min;
        glm::dvec2 max;
        int width;
    };

    Parameter energy;
    Parameter positionX;
    Parameter positionY;
    Histogram2D positionHistogram;
};

std::vector<Analysis> analyze(const Group& beamline, const BundleHistory& bundleHistory);

}  // namespace RAYX
