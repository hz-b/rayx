#pragma once
#include "Surface/Quadric.h"
#include "Beamline/OpticalElement.h"

namespace RAYX
{

    class RAYX_API Slit : public OpticalElement {

    public:

        Slit(const char* name, int shape, int beamstop, double width, double height, double azimuthal, double dist, double beamstopWidth, double beamstopHeight, double sourceEnergy, std::vector<double> misalignmentParams, const std::shared_ptr<OpticalElement> previous, bool global);
        Slit();
        ~Slit();

        int getShape() const;
        int getCentralBeamstop() const;
        double getBeamstopWidth() const;
        double getBeamstopHeight() const;
        double getWaveLength() const;

    private:
        enum GEOMETRICAL_SHAPE { GS_RECTANGLE, GS_ELLIPTICAL };
        GEOMETRICAL_SHAPE m_shape;
        enum CENTRAL_BEAMSTOP { CS_NONE, CS_RECTANGLE, CS_ELLIPTICAL }; // central beamstop shape
        CENTRAL_BEAMSTOP m_centralBeamstop;
        double m_beamstopWidth;
        double m_beamstopHeight;
        double m_waveLength;  // from lightsource
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;

    };

} // namespace RAYX