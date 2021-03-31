#include "Slit.h"

namespace RAY
{
    
    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @param width              width of mirror (x-dimension in element coord. sys.)
     * @param height             height of mirror (z-dimension in element coord. sys.)
     * @param grazingIncidence   desired incidence angle of the main ray
     * @param azimuthal          rotation of mirror around z-axis
     * @param dist               distance to preceeding element
     * @param misalignmentParams angles and distances for the mirror's misalignment
     * 
    */
    Slit::Slit(const char* name, int beamstop, double width, double height, double azimuthal, double dist, double beamstopWidth, double beamstopHeight, double waveLength, std::vector<double> misalignmentParams) 
    : Quadric(name, {0,0,0,0, width,0,0,0, height,0,0,-1, 3,0,0,0}, {width,height,beamstopWidth,beamstopHeight, waveLength,0,0,0, 0,0,0,0, 0,0,0,0}, 0, rad(azimuthal), 0, dist, misalignmentParams, {0,0,0,0,0,0}) {
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        m_centralBeamstop = beamstop == 0 ? CS_NONE : (beamstop==1 ? CS_RECTANGLE : CS_ELLIPTICAL );
        m_totalWidth = width;
        m_totalHeight = height;
        m_beamstopWidth = beamstopWidth;
        m_beamstopHeight = beamstopHeight;
        m_chi = rad(azimuthal);
        m_waveLength = waveLength;
        m_distanceToPreceedingElement = dist;
    }

    Slit::~Slit()
    {
    }

    double Slit::getWidth() {
        return m_totalWidth;
    }

    double Slit::getHeight() {
        return m_totalHeight;
    }
    
    double Slit::getChi() {
        return m_chi;
    }

    double Slit::getDist() {
        return m_distanceToPreceedingElement;
    }
    
    int Slit::getCentralBeamstop() {
        return m_centralBeamstop;
    }
    double Slit::getBeamstopWidth() {
        return m_beamstopWidth;
    }
    double Slit::getBeamstopHeight() {
        return m_beamstopHeight;
    }
    double Slit::getWaveLength() {
        return m_waveLength;
    }
        
}
