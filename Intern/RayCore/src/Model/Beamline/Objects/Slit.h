#pragma once
#include <Data/xml.h>

#include "Model/Beamline/OpticalElement.h"
#include "Model/Surface/Quadric.h"

namespace RAYX {

class RAYX_API Slit : public OpticalElement {
  public:
    Slit(const char* name, Geometry::GeometricalShape geometricalShape,
         int beamstop, double width, double height, glm::dvec4 position,
         glm::dmat4x4 orientation, double beamstopWidth, double beamstopHeight,
         double sourceEnergy);

    Slit();
    ~Slit();

    static std::shared_ptr<Slit> createFromXML(rapidxml::xml_node<>*,
                                               double sourceEnergy);

    int getCentralBeamstop() const;
    double getBeamstopWidth() const;
    double getBeamstopHeight() const;
    double getWaveLength() const;

    enum CENTRAL_BEAMSTOP {
        CS_NONE,
        CS_RECTANGLE,
        CS_ELLIPTICAL
    };  ///< central beamstop shape

  private:
    double m_waveLength;  ///< from lightsource

    // TODO(Jannis): Extra class maybe?
    CENTRAL_BEAMSTOP m_centralBeamstop;
    double m_beamstopWidth;
    double m_beamstopHeight;
};

}  // namespace RAYX