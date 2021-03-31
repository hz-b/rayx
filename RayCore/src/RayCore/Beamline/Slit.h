#pragma once
#include "Quadric.h"

namespace RAY
{

    class RAY_API Slit : public Quadric {

    public:
        
        Slit(const char* name, int beamstop, double width, double height, double azimuthal, double dist, double beamstopWidth, double beamstopHeight, double waveLength, std::vector<double> misalignmentParams); 
        Slit();
        ~Slit();

        double getWidth();
        double getHeight();
        double getChi();
        double getDist();
        int getCentralBeamstop();
        double getBeamstopWidth();
        double getBeamstopHeight();
        double getWaveLength();

    private:
        double m_totalWidth;
        double m_totalHeight;
        // grazing incidence, in rad
        enum CENTRAL_BEAMSTOP {CS_NONE, CS_RECTANGLE, CS_ELLIPTICAL}; // central beamstop shape
        CENTRAL_BEAMSTOP m_centralBeamstop;
        double m_beamstopWidth;
        double m_beamstopHeight;
        double m_waveLength;  // from lightsource
        double m_chi;
        double m_distanceToPreceedingElement;
        //double m_Depth;
        //double m_verDivergence;
        //double m_horDivergence;
        
    };

} // namespace RAY