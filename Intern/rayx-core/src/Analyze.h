#pragma once

#include "Beamline/Beamline.h"
#include "Tracer/DeviceTracer.h"

namespace RAYX {

struct Analysis {
    struct Histogram {
        std::vector<int> bins;
        double binInterval;
        double min;
        double max;
    };

    struct Parameter {
        double avg;
        double rms;
        double min;
        double max;
        double fwhmFirst;
        double fwhmLast;
        Histogram histogram;
    };
};

Analysis analyze(const Group& beamline);

}  // namespace RAYX
