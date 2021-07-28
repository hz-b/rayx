#include "Slit.h"
#include <assert.h>

namespace RAYX
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
    Slit::Slit(const char* name, int shape, int beamstop, double width, double height, double azimuthal, double dist, double beamstopWidth, double beamstopHeight, double sourceEnergy, std::vector<double> misalignmentParams, std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, { 0,0,0,0,0,0,0 }, previous),
        m_waveLength(abs(hvlam(sourceEnergy))),
        m_chi(rad(azimuthal)),
        m_distanceToPreceedingElement(dist)
    {
        m_shape = shape == 0 ? GS_RECTANGLE : GS_ELLIPTICAL;
        m_centralBeamstop = beamstop == 0 ? CS_NONE : (beamstop == 1 ? CS_RECTANGLE : CS_ELLIPTICAL);
        // if elliptical encode width and height with negative sign, if rectangle -> positive sign
        m_totalWidth = m_shape == GS_ELLIPTICAL ? -abs(width) : abs(width);
        m_totalHeight = m_shape == GS_ELLIPTICAL ? -abs(height) : abs(height);
        setDimensions(m_totalWidth, m_totalHeight);
        // if no beamstop -> set to zero
        // if elliptical set width (xStop) to negative value to encode the shape (xStop < 0 -> Elliptical, xStop > 0 -> rectangle, xStop = yStop = 0 -> none)
        m_beamstopWidth = m_centralBeamstop == CS_NONE ? 0 : (m_centralBeamstop == CS_ELLIPTICAL ? -abs(beamstopWidth) : abs(beamstopWidth));
        m_beamstopHeight = m_centralBeamstop == CS_NONE ? 0 : (m_centralBeamstop == CS_ELLIPTICAL ? abs(beamstopHeight) : abs(beamstopHeight));

        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 3, 0, 0, 0}));
        calcTransformationMatrices(0, m_chi, 0, m_distanceToPreceedingElement, misalignmentParams, global);
        setElementParameters({ m_beamstopWidth / 2,m_beamstopHeight / 2,0,0, m_waveLength,0,0,0, 0,0,0,0, 0,0,0,0 });
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
    }

    Slit::Slit() {}
    Slit::~Slit() {}

    double Slit::getWidth() const {
        return m_totalWidth;
    }

    double Slit::getHeight() const {
        return m_totalHeight;
    }

    double Slit::getChi() const {
        return m_chi;
    }

    double Slit::getDist() const {
        return m_distanceToPreceedingElement;
    }

    int Slit::getCentralBeamstop() const {
        return m_centralBeamstop;
    }

    int Slit::getShape() const {
        return m_shape;
    }

    double Slit::getBeamstopWidth() const {
        return m_beamstopWidth;
    }
    double Slit::getBeamstopHeight() const {
        return m_beamstopHeight;
    }
    double Slit::getWaveLength() const {
        return m_waveLength;
    }

}
