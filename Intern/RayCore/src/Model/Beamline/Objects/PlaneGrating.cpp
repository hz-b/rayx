#include "PlaneGrating.h"

#include "Debug/Debug.h"
#include "Shared/Constants.h"

namespace RAYX {

PlaneGrating::PlaneGrating(const DesignObject& dobj) : OpticalElement(dobj) {
    m_additionalOrder = (int)dobj.parseAdditionalOrder();
    m_designEnergyMounting = dobj.parseDesignEnergyMounting();
    m_lineDensity = dobj.parseLineDensity();
    m_orderOfDiffraction = dobj.parseOrderDiffraction();
    m_vls = dobj.parseVls();
    RAYX_VERB << "design wavelength = " << abs(hvlam(m_designEnergyMounting));

    m_surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 0,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,
        .m_a22 = 0,
        .m_a23 = 0,
        .m_a24 = -1,
        .m_a33 = 0,
        .m_a34 = 0,
        .m_a44 = 0,
    });
}

double PlaneGrating::getDesignEnergyMounting() const { return m_designEnergyMounting; }
double PlaneGrating::getLineDensity() const { return m_lineDensity; }
double PlaneGrating::getOrderOfDiffraction() const { return m_orderOfDiffraction; }
std::array<double, 6> PlaneGrating::getVls() { return m_vls; }

std::array<double, 16> PlaneGrating::getBehaviourParams() const {
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
