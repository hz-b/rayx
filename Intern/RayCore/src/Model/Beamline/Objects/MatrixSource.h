#pragma once
#include "Model/Beamline/LightSource.h"
#include <Data/xml.h>

namespace RAYX
{

    class RAYX_API MatrixSource : public LightSource
    {
    public:

        MatrixSource(const std::string name, EnergyDistribution dist, const double sourceWidth, const double sourceHeight,
            const double sourceDepth, const double horDivergence, const double verDivergence, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment);

        MatrixSource();
        ~MatrixSource();

        static std::shared_ptr<MatrixSource> createFromXML(rapidxml::xml_node<>*);

        std::vector<Ray> getRays();
        double getSourceDepth() { return m_sourceDepth; }
        double getSourceHeight() { return m_sourceHeight; }
        double getSourceWidth() { return m_sourceWidth; }
        double getVerDivergence() { return m_verDivergence; }
        double getHorDivergence() { return m_horDivergence; }
    };

} // namespace RAYX