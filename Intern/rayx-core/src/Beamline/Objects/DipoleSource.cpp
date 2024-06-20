#include "DipoleSource.h"

#include <fstream>

#include <DesignElement/DesignSource.h>
#include <Data/xml.h>
#include <Debug/Debug.h>
#include <Debug/Instrumentor.h>
#include <Random.h>
#include <Shader/Constants.h>
#include <Shader/EventType.h>

#ifndef NO_OMP
#include <omp.h>
#endif

namespace RAYX {

double get_factorCriticalEnergy() {
    double planc = 3 * PLANCK_BAR / (2 * pow(SPEED_OF_LIGHT, 5) * pow(ELECTRON_MASS, 3)) * pow(ELECTRON_VOLT, 2) * 1.0e24;  // nach RAY-UI
    return planc;  // 2.5050652873563215 , 2.2182868570172918
}

double get_factorMagneticField() {
    double magField = ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE) * 1.0e9;
    return magField;
}

double get_factorElectronEnergy() {
    double factorElectronEnergy = ELECTRON_VOLT * 1.0e9 / (ELECTRON_MASS * pow(SPEED_OF_LIGHT, 2));
    return factorElectronEnergy;
}

double get_factorOmega() {
    double factorOmega = 3 * FINE_STRUCTURE_CONSTANT /
                         (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE * pow(SPEED_OF_LIGHT, 4) * pow(ELECTRON_MASS, 2) / pow(ELECTRON_VOLT * 1.0e9, 2));
    return factorOmega;
}

double get_factorDistribution() { return 3 * FINE_STRUCTURE_CONSTANT / (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE); }

double get_factorTotalPowerDipol() {
    double totalPower = pow(ELEMENTARY_CHARGE, 2) / (3 * ELECTRIC_PERMITTIVITY * pow(SPEED_OF_LIGHT, 8) * pow(ELECTRON_MASS, 4)) *
                        pow(ELECTRON_VOLT * 1.0E9, 3) / (2 * PI) / (ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE));
    return totalPower;
}

DipoleSource::DipoleSource(const DesignSource& dSource) : LightSource(dSource), 
    m_bendingRadius(dSource.getBendingRadius()),
    m_electronEnergyOrientation(dSource.getElectronEnergyOrientation()),
    m_photonFlux(dSource.getPhotonFlux()),
    m_sourceHeight(dSource.getSourceHeight()),
    m_sourceWidth(dSource.getSourceWidth()),
    m_electronEnergy(dSource.getElectronEnergy()),
    m_criticalEnergy(RAYX::get_factorCriticalEnergy()),
    m_photonEnergy(dSource.getEnergy()),
    m_verEbeamDivergence(dSource.getVerEBeamDivergence()),
    m_bandwidth(1.0e-3),
    m_gamma(std::fabs(m_electronEnergy) * get_factorElectronEnergy()),
    m_photonWaveLength(calcPhotonWavelength(m_photonEnergy)),
    m_energySpread(dSource.getEnergySpread()),
    m_energySpreadUnit(dSource.getEnergySpreadUnit())
{   
    
    m_horDivergence = dSource.getHorDivergence();
    m_verDivergence = DipoleSource::vDivergence(m_photonEnergy, m_verEbeamDivergence);
    m_stokes = DipoleSource::getStokesSyn(m_photonEnergy, -3 * m_verDivergence, 3 * m_verDivergence);

    setLogInterpolation();
    setMaxIntensity();
    setMaxFlux();

    calcFluxOrg();
    calcHorDivDegSec();
    calcSourcePath();
}

/**
 * Creates random rays from dipole source
 *
 * with natural X, Z Position on the bending radius
 * with natural energy distribution by Schwinger (see Doku)
 * with natural psi and polarisation distribution (see Doku)
 *
 * @returns list of rays
 */

std::vector<Ray> DipoleSource::getRays(int thread_count) const {
    RAYX_PROFILE_FUNCTION();

    /**
     * initialize parallelization when counter is positive
     * with special OMP use case for num_threads(1)
     * */
    if (thread_count > 1) {
#define DIPOLE_OMP
    } else if (thread_count == 0) {
        thread_count = 1;
#define DIPOLE_OMP
    }

    double phi, en;  // phi=horizontal Angle, en=energy

    PsiAndStokes psiandstokes;  // psi=vertical Angle, stokes=light-polarisation

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

// create n rays with random position and divergence within the given span
// for width, height, horizontal and vertical divergence
#if defined(DIPOLE_OMP)
#pragma omp parallel for num_threads(thread_count)
#endif
    for (int i = 0; i < n; i++) {
        phi = (randomDouble() - 0.5) * m_horDivergence;  // chooses phi in given Divergence

        glm::dvec3 position = getXYZPosition(phi);

        en = getEnergy();  // Verteilung nach Schwingerfunktion

        psiandstokes = getPsiandStokes(en);

        phi = phi + getMisalignmentParams().m_rotationXerror.rad;

        psiandstokes.psi = psiandstokes.psi + getMisalignmentParams().m_rotationYerror.rad;

        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psiandstokes.psi);
        glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
        direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

        const auto rotation = glm::dmat3(m_orientation);
        const auto field = rotation * stokesToField(psiandstokes.stokes);

        Ray r = {position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, -1.0};
#if defined(DIPOLE_OMP)
#pragma omp critical  // thread-safety for writing rayList
        { rayList.push_back(r); }
#else
        rayList.push_back(r);
#endif
    }

    return rayList;
}

/**
 * calculates x, z position based on given horizontal angle
 * takes bending radius in the dipole into account
 * chooses y position in given source hight
 * */
glm::dvec3 DipoleSource::getXYZPosition(double phi) const {
    // RAYX_PROFILE_SCOPE("getxyz");

    double x1 = getNormalFromRange(m_sourceWidth);

    double sign = DipoleSource::m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? -1.0 : 1.0;

    double x = sign * (x1 * cos(phi) + (m_bendingRadius * 1000 * (1 - cos(phi))));  // bendingRadius in mm
    x = x + m_position.x + getMisalignmentParams().m_translationXerror;

    double y = getNormalFromRange(m_sourceHeight);
    y = y + m_position.y + getMisalignmentParams().m_translationYerror;

    double z = sign * (m_bendingRadius * 1000 - x1) * sin(phi) + getMisalignmentParams().m_translationZerror;

    return glm::dvec3(x, y, z);
}

/// monte-Carlo-method to get normal-distributed x and y Values for getXYZPosition()
double DipoleSource::getNormalFromRange(double range) const {
    // RAYX_PROFILE_SCOPE("getNormalFromRange");

    double value;
    double Distribution;

    double expanse = -0.5 / range / range;

    do {
        value = (randomDouble() - 0.5) * 9 * range;
        Distribution = exp(expanse * value * value);
    } while (Distribution < randomDouble());

    return value;
}

/**
 * chooses photon energy according to the natural energy distribution spectrum by schwinger
 */
double DipoleSource::getEnergy() const {
    // RAYX_PROFILE_SCOPE("getEnergy");

    double flux = 0.0;
    double energy = 0.0;

    do {
        energy = m_photonEnergy + (randomDouble() - 0.5) * m_energySpread;
        flux = schwinger(energy);
    } while ((flux / m_maxFlux - randomDouble()) < 0);
    return energy;
}

/**
 * calculate probability for chosen energy with edge-cases according to H.Wiedemann Synchrotron Radiation P. 259 (D.21)
 */
double DipoleSource::schwinger(double energy) const {
    // RAYX_PROFILE_SCOPE("schwinger");

    double preFactor = FACTOR_SCHWINGER_RAY * 1.e-3;

    double Y0 = energy / m_criticalEnergy;
    Y0 = Y0 / 1000;
    double yg0 = 0.0;
    double flux;

    if (Y0 > 0) {
        if (Y0 > 10) {
            yg0 = 0.777 * sqrt(Y0) * pow(ELEMENTARY_CHARGE, -Y0);
        }
        if (Y0 < 1.e-4) {
            yg0 = 1.333 * pow(Y0, (1.0 / 3.0));

        } else {
            double y = log(Y0);
            yg0 = exp(getInterpolation(y));
        }
    }
    flux = preFactor * m_gamma * yg0;

    return flux;
}

double DipoleSource::getInterpolation(double energy) const {
    // RAYX_PROFILE_SCOPE("getInterpolation");

    double functionOne = 0.0;
    double functionTwo = 0.0;
    double result = 0.0;
    int x0Position = 0;

    for (int i = 0; i < int(m_schwingerX.size()) && x0Position < int(m_schwingerX.size()); i++) {
        if (energy < m_schwingerX[i]) {
            break;
        }
        x0Position++;  // TODO: out of bounds checken
    }

    double dx0 = energy - m_schwingerX[x0Position - 1];
    double dx1 = energy - m_schwingerX[x0Position];
    double dx2 = energy - m_schwingerX[x0Position + 1];

    functionOne = (dx0 * m_schwingerY[x0Position] - dx1 * m_schwingerY[x0Position - 1]) / (dx0 - dx1);
    functionTwo = (dx0 * m_schwingerY[x0Position + 1] - dx2 * m_schwingerY[x0Position - 1]) / (dx0 - dx2);

    result = (dx1 * functionTwo - dx2 * functionOne) / (dx1 - dx2);

    return result;
}

/**
 * chooses psi and stokes-vector according to the natural distribution spectrum
 */
PsiAndStokes DipoleSource::getPsiandStokes(double en) const {
    // RAYX_PROFILE_SCOPE("getPsiStokes");

    PsiAndStokes psiandstokes;
    do {
        psiandstokes.psi = (randomDouble() - 0.5) * 6 * m_verDivergence;
        psiandstokes = dipoleFold(psiandstokes.psi, en, m_verEbeamDivergence);
    } while ((psiandstokes.stokes[0]) / m_maxIntensity < randomDouble());

    psiandstokes.psi = psiandstokes.psi * 1e-3;  // psi in rad

    return psiandstokes;
}

PsiAndStokes DipoleSource::dipoleFold(double psi, double photonEnergy, double sigpsi) const {
    // RAYX_PROFILE_SCOPE("dipolefold");

    int ln = (int)sigpsi;
    double trsgyp = 0.0;
    double sgyp = 0.0;
    double sy = 0.0;
    double zw = 0.0;
    double wy = 0.0;
    double newpsi = 0.0;

    glm::dvec4 ST = glm::dvec4(0.0, 0.0, 0.0, 0.0);
    glm::dvec4 Stokes;

    if (sigpsi != 0) {
        if (ln > 10) {
            ln = 10;
        }
        if (ln == 0) {
            ln = 10;
        }
        trsgyp = -0.5 / sigpsi / sigpsi;
        sgyp = 4.0e-3 * sigpsi;
    } else {
        trsgyp = 0;
        ln = 1;
    }

    for (int i = 1; i <= ln; i++) {
        do {
            sy = (randomDouble() - 0.5) * sgyp;
            zw = trsgyp * sy * sy;
            wy = exp(zw);
        } while (wy - randomDouble() < 0);

        newpsi = psi + sy;
        Stokes = getStokesSyn(photonEnergy, newpsi, newpsi);

        for (int i = 0; i < 4; i++) {
            ST[i] = ST[i] + Stokes[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        Stokes[i] = ST[i] / ln;
    }
    PsiAndStokes psiandstokes;
    psiandstokes.psi = newpsi;

    ST[0] = Stokes[2] + Stokes[3];
    ST[1] = Stokes[0];
    ST[2] = 0;
    ST[3] = Stokes[1];

    psiandstokes.stokes = ST;

    return psiandstokes;
}

glm::dvec4 DipoleSource::getStokesSyn(double energy, double psi1, double psi2) const {
    // RAYX_PROFILE_SCOPE("getStokesSyn");

    double fak = 3453345200000000.0;  // getFactorDistribution

    double gamma = fabs(m_electronEnergy) * 1957;  // getFactorElectronEnergy
    // double ec = m_criticalEnergy * 1000 * pow(m_electronEnergy, 3) / m_bendingRadius;
    double y0 = energy / m_criticalEnergy / 1000.0;
    double xnue1 = 1.0 / 3.0;
    double xnue2 = 2.0 / 3.0;

    double dpsi = (psi2 - psi1) / 101.0;
    double psi = psi1 + dpsi / 2.0;

    if (dpsi < 0.001) {
        dpsi = 0.001;
    }

    glm::dvec4 stokes = glm::dvec4(0.0, 0.0, 0.0, 0.0);

    while (psi <= psi2) {
        double sign1 = (DipoleSource::m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? PI : -PI) / 2;
        double sign2 = psi >= 0.0 ? 1.0 : -1.0;
        double phase = -(sign1 * sign2);
        double x = gamma * psi * 0.001;
        double zeta = pow(1.0 + pow(x, 2), (3.0 / 2.0)) * 0.5 * y0;
        double xkn2 = bessel(xnue2, zeta);
        double xkn1 = bessel(xnue1, zeta);
        double xint = fak * pow(gamma, 2.0) * pow(y0, 2.0) * pow(1.0 + pow(x, 2.0), 2.0);
        double xintp = xint * pow(xkn2, 2.0);
        double xints = xint * (pow(x, 2.0) / (1.0 + pow(x, 2.0)) * pow(xkn1, 2.0));
        xintp = xintp * dpsi * 1e-6;
        xints = xints * dpsi * 1e-6;

        stokes[0] = stokes[0] + xintp - xints;
        stokes[1] = stokes[1] + 2.0 * sqrt(xintp * xints) * sin(phase);
        stokes[2] = stokes[2] + xintp;
        stokes[3] = stokes[3] + xints;
        psi = psi + dpsi;
    }

    return stokes;
}

double DipoleSource::bessel(double hnue, double zeta) const {
    // RAYX_PROFILE_SCOPE("bessel");

    double h = 0.1;
    double result = h / 2.0 * exp(-zeta);
    double c1 = 1;
    double c2 = 0;
    for (int i = 1; c1 > c2; i++) {
        double cosh1 = (exp(h * i) + exp(-h * i)) / 2.0;
        double cosh2 = (exp(h * i * hnue) + exp(-h * i * hnue)) / 2.0;
        c1 = h * exp(-zeta * cosh1) * cosh2;

        if ((zeta * cosh1) > 225) {
            return result;
        }
        result = result + c1;
        c2 = result / 1e6;
    }

    return result;
}

void DipoleSource::setLogInterpolation() {
    for (uint32_t i = 0; i < m_schwingerX.size(); i++) {
        m_schwingerY[i] = m_schwingerX[i] * m_schwingerY[i];
        m_schwingerX[i] = log(m_schwingerX[i]);
        m_schwingerY[i] = log(m_schwingerY[i]);
    }
}

double DipoleSource::vDivergence(double energy, double sigv) const {
    double gamma = fabs(m_electronEnergy) * get_factorElectronEnergy();  // factorElectronEnergy
    if (gamma == 0.0 || m_criticalEnergy == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(m_criticalEnergy * 1000.0 / energy, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2));
}

void DipoleSource::setMaxIntensity() {
    double smax = 0.0;
    double psi = -m_verDivergence;

    for (int i = 1; i < 250; i++) {
        psi = psi + 0.05;
        auto S = dipoleFold(psi, m_photonEnergy, 1.0);
        if (smax < (S.stokes[2] + S.stokes[3])) {
            smax = S.stokes[2] + S.stokes[3];
        } else {
            break;
        }
    }
    m_maxIntensity = smax;
}

void DipoleSource::calcMagneticField() {
    m_magneticFieldStrength = get_factorMagneticField() * fabs(m_electronEnergy) / m_bendingRadius;
    m_criticalEnergy = get_factorCriticalEnergy() * pow(fabs(m_electronEnergy), 3) / m_bendingRadius;

    m_totalPower = get_factorTotalPowerDipol() * 0.1 * pow(fabs(m_electronEnergy), 3) * m_magneticFieldStrength * fabs(m_horDivergence) / 1000.0;
    m_gamma = m_electronEnergy / (ELECTRON_MASS * pow(SPEED_OF_LIGHT, 2) / (ELECTRON_VOLT)*1.e-9);

    if (m_gamma >= 1) {
        m_beta = sqrt(pow(m_gamma, 2) - 1) / m_gamma;
    } else {
        m_beta = 1;
    }
}

void DipoleSource::calcSourcePath() {
    m_sourcePathLength = fabs(m_sourcePulseLength) * 1000 * 0.3;
    m_phaseJitter = m_photonEnergy == 0 ? 0 : fabs(m_sourcePulseLength * 0.3) / m_photonWaveLength * 2000000 * PI;
}

void DipoleSource::calcHorDivDegSec() {
    m_horDivDegrees = m_horDivergence * 180 / PI;
    m_horDivSeconds = m_horDivergence * 180 / PI * 3600;
}

void DipoleSource::calcFluxOrg() {
    double bandwidth = 0.001;

    if (m_energySpread != 0.0) {
        if (m_energySpreadUnit == RAYX::EnergySpreadUnit::EU_PERCENT) {
            bandwidth = m_energySpread / 100.0;
        } else if (m_energySpreadUnit == RAYX::EnergySpreadUnit::EU_eV) {
            bandwidth = m_energySpread / m_photonEnergy;
        }
        bandwidth = m_energySpread / 100;
    }

    m_flux = (m_stokes[2] + m_stokes[3]) * (m_horDivergence * 1e-3) * bandwidth * 100.0;
}

void DipoleSource::setMaxFlux() {
    // RAYX_PROFILE_SCOPE("setMaxFlux");
    double EMAXS = 285.81224786 * m_criticalEnergy;  // welche Zahl ist das?
    double Emax = m_photonEnergy + m_energySpread / 2;
    double Emin = m_photonEnergy - m_energySpread / 2;

    if (Emax < EMAXS) {
        m_maxFlux = schwinger(Emax);
    } else if (Emin > EMAXS) {
        m_maxFlux = schwinger(Emin);
    } else {
        m_maxFlux = schwinger(EMAXS);
    }
}

/*double DipoleSource::getHorDivergence() const { return m_horDivergence; }

double DipoleSource::getSourceHeight() const { return m_sourceHeight; }

double DipoleSource::getSourceWidth() const { return m_sourceWidth; }
*/
}  // namespace RAYX
