#pragma once

#include "LightSource.h"

namespace RAY
{

    class RAY_API PointSource : public LightSource
    {
    public:
        
        PointSource(int id, std::string name, int numberOfRays, int spreadType, double sourceWidth, double sourceHeight,
    double sourceDepth, double horDivergence, double verDivergence, int widthDist, int heightDist, int horDist, int verDist, double photonEnergy, double energySpread, std::vector<double> misalignment);
        
        PointSource();
        ~PointSource();

        enum SOURCE_DIST { SD_HARDEDGE, SD_GAUSSIAN };
        std::vector<Ray> getRays();
        double getCoord(SOURCE_DIST l, double extent);
        double getSourceDepth();
        double getSourceHeight();
        double getSourceWidth();
        double getVerDivergence();
        double getHorDivergence();
        
    private:
        double m_sourceWidth;
        double m_sourceHeight;
        double m_sourceDepth;
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

} // namespace RAY