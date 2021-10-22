#pragma once

#include "Model/Beamline/LightSource.h"

namespace RAYX
{

    class RAYX_API PointSource : public LightSource
    {
    public:

        PointSource(const std::string name, const int spreadType, const double sourceWidth, const double sourceHeight,
            const double sourceDepth, const double horDivergence, const double verDivergence, const int widthDist, const int heightDist, const int horDist, const int verDist, const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment);

        PointSource();
        ~PointSource();

        static std::shared_ptr<PointSource> createFromXML(rapidxml::xml_node<>*);

        enum SOURCE_DIST { SD_HARDEDGE, SD_GAUSSIAN };

        std::vector<Ray> getRays();
        double getCoord(const SOURCE_DIST l, const double extent);
        double getSourceDepth() const;
        double getSourceHeight() const;
        double getSourceWidth() const;
        double getVerDivergence() const;
        double getHorDivergence() const;

    private:

        // Geometric Parameters
        SOURCE_DIST m_widthDist;
        SOURCE_DIST m_heightDist;
        SOURCE_DIST m_horDist;
        SOURCE_DIST m_verDist;
        //std::vector<SOURCE_LENGTH> m_source_lengths;
    };

} // namespace RAYX