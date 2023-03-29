#include "SphereGrating.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Shared/Constants.h"
namespace RAYX {

SphereGrating::SphereGrating(const DesignObject& dobj) : OpticalElement(dobj) {
    m_designEnergyMounting = dobj.parseDesignEnergy();
    m_lineDensity = dobj.parseLineDensity();
    m_orderOfDiffraction = dobj.parseOrderDiffraction();
    m_vls = dobj.parseVls();

    m_gratingMount = dobj.parseGratingMount();
    auto radius = dobj.parseRadius();
    m_surface = serializeQuadric({
        .m_icurv = 1,
        .m_a11 = 1,
        .m_a12 = 0,
        .m_a13 = 0,
        .m_a14 = 0,

        .m_a22 = 1,
        .m_a23 = 0,
        .m_a24 = -radius,
        .m_a33 = 1,
        .m_a34 = 0,
        .m_a44 = 0,
    });

    m_behaviour = serializeGrating({
        .m_vls = {m_vls[0], m_vls[1], m_vls[2], m_vls[3], m_vls[4], m_vls[5]},
        .m_lineDensity = m_lineDensity,
        .m_orderOfDiffraction = m_orderOfDiffraction,
    });
}

double SphereGrating::getRadius() const { return m_radius; }

double SphereGrating::getExitArmLength() const { return m_exitArmLength; }
double SphereGrating::getEntranceArmLength() const { return m_entranceArmLength; }

double SphereGrating::getDeviation() const { return m_deviation; }
GratingMount SphereGrating::getGratingMount() const { return m_gratingMount; }
double SphereGrating::getDesignEnergyMounting() const { return m_designEnergyMounting; }
double SphereGrating::getLineDensity() const { return m_lineDensity; }
double SphereGrating::getOrderOfDiffraction() const { return m_orderOfDiffraction; }
double SphereGrating::getA() const { return m_a; }

}  // namespace RAYX
