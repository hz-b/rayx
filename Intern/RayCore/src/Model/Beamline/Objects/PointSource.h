#pragma once

#include <Data/xml.h>

#include <filesystem>

#include "Model/Beamline/LightSource.h"

namespace RAYX {

enum class SourceDist { Uniform, Gaussian };

class RAYX_API PointSource : public LightSource {
  public:
    PointSource(const std::string name, EnergyDistribution dist,
                const double sourceWidth, const double sourceHeight,
                const double sourceDepth, const double horDivergence,
                const double verDivergence, const SourceDist widthDist,
                const SourceDist heightDist, const SourceDist horDist,
                const SourceDist verDist, const double linPol0,
                const double linPol45, const double circPol,
                const std::array<double, 6> misalignment);

    PointSource();
    ~PointSource();

    static std::shared_ptr<PointSource> createFromXML(xml::Parser);

    std::vector<Ray> getRays();
    double getCoord(const SourceDist l, const double extent);

  private:
    // Geometric Parameters
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX