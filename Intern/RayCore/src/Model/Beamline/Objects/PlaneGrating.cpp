#include "PlaneGrating.h"

#include "Debug.h"

namespace RAYX {

PlaneGrating::PlaneGrating(const DesignObject& dobj) : OpticalElement(dobj) {
    m_additionalOrder = dobj.parseAdditionalOrder();
    m_designEnergyMounting = dobj.parseDesignEnergyMounting();
    m_lineDensity = dobj.parseLineDensity();
    m_orderOfDiffraction = dobj.parseOrderDiffraction();
    m_vls = dobj.parseVls();
    RAYX_VERB << "design wavelength = " << abs(hvlam(m_designEnergyMounting));

    // parameters of quadric surface
    Material mat = dobj.parseMaterial();
    auto matd = (double)static_cast<int>(mat);
    setSurface(std::make_unique<Quadric>(glm::dmat4x4{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 1, 0, matd, 0}));
}

double PlaneGrating::getDesignEnergyMounting() const { return m_designEnergyMounting; }
double PlaneGrating::getLineDensity() const { return m_lineDensity; }
double PlaneGrating::getOrderOfDiffraction() const { return m_orderOfDiffraction; }
std::array<double, 6> PlaneGrating::getVls() { return m_vls; }

glm::dmat4x4 PlaneGrating::getElementParameters() const {
    return {0,
            0,
            m_lineDensity,
            m_orderOfDiffraction,
            abs(hvlam(m_designEnergyMounting)),
            0,
            m_vls[0],
            m_vls[1],
            m_vls[2],
            m_vls[3],
            m_vls[4],
            m_vls[5],
            0,
            0,
            0,
            double(m_additionalOrder)};
}
}  // namespace RAYX
