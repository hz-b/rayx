#pragma once

#include "Beamline/LightSource.h"
#include "Data/xml.h"


namespace RAYX {
struct DesignSource; //TODO Fanny see where the forward declaration has to go
class LightSource;//TODO Fanny see where the forward declaration has to go

class RAYX_API PointSource : public LightSource {
  public:
    PointSource(const DesignSource&);
    virtual ~PointSource() = default;

    std::vector<Ray> getRays(int thread_count = 1) const ;

    //double getHorDivergence() const override;
    //double getSourceHeight() const override;
    //double getSourceWidth() const override;

  private:
    // Geometric Params
    SourceDist m_widthDist;
    SourceDist m_heightDist;
    SourceDist m_horDist;
    SourceDist m_verDist;
    SourcePulseType m_sourceDistributionType;  // TODO unused.

    glm::dvec4 m_Pol;

    double m_verDivergence;
    double m_sourceDepth;
    double m_sourceHeight;
    double m_sourceWidth;
    // std::vector<SOURCE_LENGTH> m_source_lengths;
};

}  // namespace RAYX
