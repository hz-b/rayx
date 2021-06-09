#pragma once
#include "LightSource.h"

namespace RAY
{

    class RAY_API MatrixSource : public LightSource
    {
    public:

        MatrixSource(int id, std::string name, int numberOfRays, int spreadType, double sourceWidth, double sourceHeight,
            double sourceDepth, double horDivergence, double verDivergence, double photonEnergy, double energySpread, std::vector<double> misalignment);

        MatrixSource();
        ~MatrixSource();

        std::vector<Ray> getRays();
        double getSourceDepth() { return m_sourceDepth; }
        double getSourceHeight() { return m_sourceHeight; }
        double getSourceWidth() { return m_sourceWidth; }
        double getVerDivergence() { return m_verDivergence; }
        double getHorDivergence() { return m_horDivergence; }

    private:
        double m_sourceDepth;
        double m_sourceHeight;
        double m_sourceWidth;
        double m_horDivergence;
        double m_verDivergence;


    };

} // namespace RAY