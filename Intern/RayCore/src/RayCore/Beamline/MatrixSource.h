#pragma once
#include "LightSource.h"

namespace RAY
{

    class RAY_API MatrixSource : public LightSource
    {
    public:
        
        MatrixSource(int id, std::string name, int numberOfRays, double sourceWidth, double sourceHeight,
    double sourceDepth, double horDivergence, double verDivergence, std::vector<double> misalignment);
        
        MatrixSource();
        ~MatrixSource();

        std::vector<Ray> getRays();
        double getSourceDepth();
        double getSourceHeight();
        double getSourceWidth();
        double getVerDivergence();
        double getHorDivergence();
        
    private:
        double m_sourceWidth;
        double m_sourceHeight;
        double m_sourceDepth;
        double m_verDivergence;
        double m_horDivergence;
        
    };

} // namespace RAY