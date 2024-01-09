#include "MainBeam.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "Shader/Constants.h"

namespace RAYX {

MainBeam::MainBeam(const DesignObject& dobj) : LightSource(dobj) {
    m_linearPol_0 = dobj.parseLinearPol0();
    m_linearPol_45 = dobj.parseLinearPol45();
    m_circularPol = dobj.parseCircularPol();
}


std::vector<Ray> MainBeam::getRays([[maybe_unused]]int thread_count) const {
    
    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);

    for (int i = 0; i < n; i++) {

        double en = selectEnergy();  // LightSource.cpp

        glm::dvec4 stokes = glm::dvec4(1, m_linearPol_0, m_linearPol_45, m_circularPol);

        Ray r = {glm::dvec3(0, 0, 0), ETYPE_UNINIT, glm::dvec3(0, 0, 1), en, stokes, 0.0, 0.0, -1.0, -1.0};
        rayList.push_back(r);
    }
    return rayList;

}

}