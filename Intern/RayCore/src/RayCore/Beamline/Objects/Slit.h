#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API Slit : public OpticalElement {

    public:

        Slit(const char* name, int geometricalShape, int beamstop, double width, double height, glm::dvec4 position, glm::dmat4x4 orientation, double beamstopWidth, double beamstopHeight, double sourceEnergy);
        //Slit(const char* name, int geometricalShape, int beamstop, double width, double height, double azimuthal, double dist, double beamstopWidth, double beamstopHeight, double sourceEnergy, std::vector<double> misalignmentParams, const std::shared_ptr<OpticalElement> previous, bool global);
        Slit();
        ~Slit();

        int getCentralBeamstop() const;
        double getBeamstopWidth() const;
        double getBeamstopHeight() const;
        double getWaveLength() const;

        enum CENTRAL_BEAMSTOP { CS_NONE, CS_RECTANGLE, CS_ELLIPTICAL }; ///< central beamstop shape

    private:
        // TODO(Jannis): Is this necessary?
        double m_waveLength;  ///< from lightsource

        // TODO(Jannis): Extra class maybe?
        CENTRAL_BEAMSTOP m_centralBeamstop;
        double m_beamstopWidth;
        double m_beamstopHeight;

    };

} // namespace RAYX