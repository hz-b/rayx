#pragma once

#include "Beamline/LightSource.h"

namespace RAYX
{

    class RAY_API PointSource : public LightSource
    {
    public:

        PointSource(const int id, const std::string name, const int numberOfRays, const int spreadType, const double sourceWidth, const double sourceHeight,
            const double sourceDepth, const double horDivergence, const double verDivergence, const int widthDist, const int heightDist, const int horDist, const int verDist, const double photonEnergy, const double energySpread, const std::vector<double> misalignment);

        PointSource();
        ~PointSource();

        enum SOURCE_DIST { SD_HARDEDGE, SD_GAUSSIAN };
        std::vector<Ray> getRays();
        double getCoord(const SOURCE_DIST l, const double extent);
        double getSourceDepth() const;
        double getSourceHeight() const;
        double getSourceWidth() const;
        double getVerDivergence() const;
        double getHorDivergence() const;

    private:
        double m_sourceDepth;
        double m_sourceHeight;
        double m_sourceWidth;
        double m_horDivergence;
        double m_verDivergence;
        std::vector<double> m_a;
        std::uniform_real_distribution<double> m_uniform;
        std::normal_distribution<double> m_stdnorm;
        std::default_random_engine m_re;
        SOURCE_DIST m_widthDist;
        SOURCE_DIST m_heightDist;
        SOURCE_DIST m_horDist;
        SOURCE_DIST m_verDist;
        //std::vector<SOURCE_LENGTH> m_source_lengths;
    };

} // namespace RAYX