#include "LightSource.h"
#include <cassert>
#include <cmath>

namespace RAYX
{
    LightSource::LightSource(const int id, const int numberOfRays, const char* name, const int spreadType,
        const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment)
        : BeamlineObject(name),
        m_id(id),
        m_numberOfRays(numberOfRays),
        m_misalignmentParams(misalignment),
        m_energySpread(energySpread),
        m_photonEnergy(photonEnergy),
        m_linearPol_0(linPol0),
        m_linearPol_45(linPol45),
        m_circularPol(circPol)
    {
        std::uniform_real_distribution<double> m_unif(0, 1);
        m_energySpreadType = spreadType == 0 ? ES_WHITE_BAND : ES_THREE_ENERGIES;
    }

    int LightSource::getNumberOfRays() {
        return m_numberOfRays;
    }
    double LightSource::getPhotonEnergy() {
        return m_photonEnergy;
    }
    double LightSource::getEnergySpread() {
        return m_energySpread;
    }
    int LightSource::getSpreadType() {
        return m_energySpreadType;
    }
    double LightSource::getLinear0() {
        return m_linearPol_0;
    }
    double LightSource::getLinear45() {
        return m_linearPol_45;
    }
    double LightSource::getCircular() {
        return m_circularPol;
    }
    void LightSource::setNumberOfRays(const int numberOfRays) {
        m_numberOfRays = numberOfRays;
    }
    int LightSource::getId() {
        return m_id;
    }
    std::vector<double> LightSource::getMisalignmentParams() {
        return m_misalignmentParams;
    }

    // needed for many of the light sources, from two angles to one direction vector
    glm::dvec3 LightSource::getDirectionFromAngles(const double phi, const double psi) {
        double al = cos(psi) * sin(phi);
        double am = -sin(psi);
        double an = cos(psi) * cos(phi);
        return glm::dvec3(al, am, an);
    }

    //  (see RAYX.FOR select_energy)
    double LightSource::selectEnergy() {
        double en;
        if (m_energySpread == 0) { // if there is no spread specified use photonenergy directly
            en = m_photonEnergy;
            // else distinguish between two sprad types 
        }
        else if (m_energySpreadType == ES_WHITE_BAND) { //  energy uniform random in [photonEnergy - 0.5*spread, photonEnergy + 0.5*spread]
            double rn = m_unif(m_re);
            en = m_photonEnergy + (rn - 0.5) * m_energySpread;
        }
        else { // three sources
            double rn = m_unif(m_re);
            en = m_photonEnergy + double(int(rn * 3) - 1) * 0.5 * m_energySpread;
        }
        return en;
    }

    LightSource::~LightSource()
    {
    }

} // namespace RAYX