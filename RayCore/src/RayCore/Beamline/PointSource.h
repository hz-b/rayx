#pragma once

#include "LightSource.h"

namespace RAY
{

    class RAY_API PointSource : public LightSource
    {
    public:
        
        PointSource(int id, std::string name, int numberOfRays, double sourceWidth, double sourceHeight,
    double sourceDepth, double horDivergence, double verDivergence, int widthLength, int heightLength, int horLength, int verLength);
        
        PointSource();
        ~PointSource();

        enum SOURCE_LENGTH { SL_HARDEDGE, SL_GAUSSIAN };
        std::vector<Ray> getRays();
        double getCoord(SOURCE_LENGTH l, double extent);
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
        SOURCE_LENGTH m_widthLength;
        SOURCE_LENGTH m_heightLength;
        SOURCE_LENGTH m_horLength;
        SOURCE_LENGTH m_verLength;
        //std::vector<SOURCE_LENGTH> m_source_lengths;
    };

} // namespace RAY