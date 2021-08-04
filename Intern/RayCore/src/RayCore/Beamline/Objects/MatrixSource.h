#pragma once
#include "Beamline/LightSource.h"

namespace RAYX
{

    class RAYX_API MatrixSource : public LightSource
    {
    public:

        MatrixSource(const int id, const std::string name, const int numberOfRays, const int spreadType, const double sourceWidth, const double sourceHeight,
            const double sourceDepth, const double horDivergence, const double verDivergence, const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment);

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

} // namespace RAYX