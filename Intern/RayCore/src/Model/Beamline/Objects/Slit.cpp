#include "Slit.h"
#include <assert.h>

namespace RAYX
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (quadric)
     * sets mirror-specific parameters in this class
     * @param name                  name of optical element
     * @param geometricalShape      0/1 rectangle or elliptical
     * @param beamstop              0/1 whether there is a central beamstop
     * @param width                 width of mirror (x-dimension in element coord. sys.)
     * @param height                height of mirror (z-dimension in element coord. sys.)
     * @param position              position in world coordinates
     * @param orientation           orientation in world coordinates 
     * @param beamstopWidth         width of central beamstop
     * @param beamstopHeight        height of central beamstop
     * @param sourceEnergy          energy of source
     * @param misalignmentParams    angles and distances for the mirror's misalignment
     * 
    */
    Slit::Slit(const char* name, int geometricalShape, int beamstop, double width, double height, glm::dvec4 position, glm::dmat4x4 orientation, double beamstopWidth, double beamstopHeight, double sourceEnergy)
        : OpticalElement(name, geometricalShape, width, height, position, orientation, { 0,0,0,0,0,0,0 }),
        m_waveLength(abs(hvlam(sourceEnergy)))
    {
        m_centralBeamstop = beamstop == 0 ? CS_NONE : (beamstop == 1 ? CS_RECTANGLE : CS_ELLIPTICAL);

        // if no beamstop -> set to zero
        // if elliptical set width (xStop) to negative value to encode the shape (xStop < 0 -> Elliptical, xStop > 0 -> rectangle, xStop = yStop = 0 -> none)
        m_beamstopWidth = m_centralBeamstop == CS_NONE ? 0 : (m_centralBeamstop == CS_ELLIPTICAL ? -abs(beamstopWidth) : abs(beamstopWidth));
        m_beamstopHeight = m_centralBeamstop == CS_NONE ? 0 : (m_centralBeamstop == CS_ELLIPTICAL ? abs(beamstopHeight) : abs(beamstopHeight));

        setSurface(std::make_unique<Quadric>(std::vector<double>{0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0}));
        setElementParameters({ m_beamstopWidth / 2,m_beamstopHeight / 2,0,0, m_waveLength,0,0,0, 0,0,0,0, 0,0,0,0 });
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
    }

    Slit::Slit() {}
    Slit::~Slit() {}

    std::shared_ptr<Slit> Slit::createFromXML(rapidxml::xml_node<>* node, double sourceEnergy) {
        const char* name = node->first_attribute("name")->value();

        int geometricalShape;
        if (!xml::paramInt(node, "geometricalShape", &geometricalShape)) { return nullptr; }

        int beamstop;
        if (!xml::paramInt(node, "centralBeamstop", &beamstop)) { return nullptr; }

        double width;
        if (!xml::paramDouble(node, "totalWidth", &width)) { return nullptr; }

        double height;
        if (!xml::paramDouble(node, "totalHeight", &height)) { return nullptr; }

        glm::dvec4 position;
        if (!xml::paramPosition(node, &position)) { return nullptr; }

        glm::dmat4x4 orientation;
        if (!xml::paramOrientation(node, &orientation)) { return nullptr; }

        double beamstopWidth;
        if (!xml::paramDouble(node, "totalWidthStop", &beamstopWidth)) { return nullptr; }

        double beamstopHeight;
        if (!xml::paramDouble(node, "totalHeightStop", &beamstopHeight)) { return nullptr; }

        return std::make_shared<Slit>(name, geometricalShape, beamstop, width, height, position, orientation, beamstopWidth, beamstopHeight, sourceEnergy);
    }

    int Slit::getCentralBeamstop() const {
        return m_centralBeamstop;
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
