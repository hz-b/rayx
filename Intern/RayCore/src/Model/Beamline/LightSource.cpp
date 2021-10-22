#include "LightSource.h"
#include <cassert>
#include <cmath>

namespace RAYX
{
    LightSource::LightSource(const char* name, const int spreadType, const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment, const double sourceDepth, const double sourceHeight, const double sourceWidth, const double horDivergence, const double verDivergence)
        : BeamlineObject(name),
        m_sourceDepth(sourceDepth),
        m_sourceHeight(sourceHeight),
        m_sourceWidth(sourceWidth),
        m_horDivergence(horDivergence),
        m_verDivergence(verDivergence),
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

    // ! Temporary code clone
    LightSource::LightSource(const char* name, const int spreadType, const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment)
        : BeamlineObject(name),
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

    double LightSource::getPhotonEnergy()
    {
        return m_photonEnergy;
    }

    double LightSource::getEnergySpread()
    {
        return m_energySpread;
    }

    int LightSource::getSpreadType()
    {
        return m_energySpreadType;
    }

    double LightSource::getLinear0()
    {
        return m_linearPol_0;
    }

    double LightSource::getLinear45()
    {
        return m_linearPol_45;
    }

    double LightSource::getCircular()
    {
        return m_circularPol;
    }

    std::vector<double> LightSource::getMisalignmentParams()
    {
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
            double rn = m_uniformDist(m_randEngine);
            en = m_photonEnergy + (rn - 0.5) * m_energySpread;
        }
        else { // three sources
            double rn = m_uniformDist(m_randEngine);
            en = m_photonEnergy + double(int(rn * 3) - 1) * 0.5 * m_energySpread;
        }
        return en;
    }

    LightSource::~LightSource()
    {
    }

} // namespace RAYX