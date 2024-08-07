#include "DipoleSource.h"

#include "Shader/EventType.h"

namespace RAYX {

constexpr double get_factorCriticalEnergy() {
    double planc =
        3 * PLANCK_BAR / (2 * glm::pow(SPEED_OF_LIGHT, 5) * glm::pow(ELECTRON_MASS, 3)) * glm::pow(ELECTRON_VOLT, 2) * 1.0e24;  // nach RAY-UI
    return planc;  // 2.5050652873563215 , 2.2182868570172918
}

constexpr double get_factorMagneticField() {
    double magElectricField = ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE) * 1.0e9;
    return magElectricField;
}

constexpr double get_factorElectronEnergy() {
    double factorElectronEnergy = ELECTRON_VOLT * 1.0e9 / (ELECTRON_MASS * glm::pow(SPEED_OF_LIGHT, 2));
    return factorElectronEnergy;
}

RAYX_API
constexpr double get_factorOmega() {
    double factorOmega =
        3 * FINE_STRUCTURE_CONSTANT /
        (4.0 * glm::pow(PI, 2) * ELEMENTARY_CHARGE * glm::pow(SPEED_OF_LIGHT, 4) * glm::pow(ELECTRON_MASS, 2) / glm::pow(ELECTRON_VOLT * 1.0e9, 2));
    return factorOmega;
}

constexpr double get_factorDistribution() { return 3 * FINE_STRUCTURE_CONSTANT / (4.0 * glm::pow(PI, 2) * ELEMENTARY_CHARGE); }

constexpr double get_factorTotalPowerDipol() {
    double totalPower = glm::pow(ELEMENTARY_CHARGE, 2) / (3 * ELECTRIC_PERMITTIVITY * glm::pow(SPEED_OF_LIGHT, 8) * glm::pow(ELECTRON_MASS, 4)) *
                        glm::pow(ELECTRON_VOLT * 1.0E9, 3) / (2 * PI) / (ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE));
    return totalPower;
}

DipoleSource::DipoleSource(const DesignSource& designSource)
    : LightSource(designSource),
      m_bendingRadius(designSource.getBendingRadius()),
      m_electronEnergyOrientation(designSource.getElectronEnergyOrientation()),
      m_photonFlux(designSource.getPhotonFlux()),
      m_sourceHeight(designSource.getSourceHeight()),
      m_sourceWidth(designSource.getSourceWidth()),
      m_electronEnergy(designSource.getElectronEnergy()),
      m_criticalEnergy(RAYX::get_factorCriticalEnergy()),
      m_photonEnergy(designSource.getEnergy()),
      m_verEbeamDivergence(designSource.getVerEBeamDivergence()),
      m_bandwidth(1.0e-3),
      m_gamma(std::fabs(m_electronEnergy) * get_factorElectronEnergy()),
      m_photonWaveLength(calcPhotonWavelength(m_photonEnergy)),
      m_energySpread(designSource.getEnergySpread()),
      m_energySpreadUnit(designSource.getEnergySpreadUnit()) {
    m_verDivergence = vDivergence(m_photonEnergy, m_verEbeamDivergence);
    m_stokes = calcStokesSyn(m_photonEnergy, -3 * m_verDivergence, 3 * m_verDivergence);
    Rand rand;  // TODO(Sven): why do we need random numbers to calc max intensity?
    setMaxIntensity(rand);
    setMaxFlux();
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
RAYX_FN_ACC
Ray DipoleSource::getRay(int32_t lightSourceId, Rand& rand) const {
    double phi, en;  // phi=horizontal Angle, en=energy

    PsiAndStokes psiandstokes;  // psi=vertical Angle, stokes=light-polarisation

    // create n rays with random position and divergence within the given span
    // for width, height, horizontal and vertical divergence
    phi = (rand.randomDouble() - 0.5) * m_horDivergence;  // chooses phi in given Divergence

    glm::dvec3 position = getXYZPosition(phi, rand);

    en = getEnergy(rand);  // Verteilung nach Schwingerfunktion

    psiandstokes = getPsiandStokes(en, rand);

    phi = phi + m_misalignment.m_rotationXerror.rad;

    psiandstokes.psi = psiandstokes.psi + m_misalignment.m_rotationYerror.rad;

    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psiandstokes.psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto rotation = glm::dmat3(m_orientation);
    const auto field = rotation * stokesToElectricField(psiandstokes.stokes);

    return Ray{position, ETYPE_UNINIT, direction, en, field, 0.0, 0.0, -1.0, static_cast<double>(lightSourceId)};
}

/**
 * calculates x, z position based on given horizontal angle
 * takes bending radius in the dipole into account
 * chooses y position in given source hight
 * */
RAYX_FN_ACC
glm::dvec3 DipoleSource::getXYZPosition(double phi, Rand& rand) const {
    double x1 = getNormalFromRange(m_sourceWidth, rand);

    double sign = DipoleSource::m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? -1.0 : 1.0;

    double x = sign * (x1 * cos(phi) + (m_bendingRadius * 1000 * (1 - cos(phi))));  // bendingRadius in mm
    x = x + m_position.x + m_misalignment.m_translationXerror;

    double y = getNormalFromRange(m_sourceHeight, rand);
    y = y + m_position.y + m_misalignment.m_translationYerror;

    double z = sign * (m_bendingRadius * 1000 - x1) * sin(phi) + m_misalignment.m_translationZerror;

    return glm::dvec3(x, y, z);
}

/**
 * chooses photon energy according to the natural energy distribution spectrum by schwinger
 */
RAYX_FN_ACC
double DipoleSource::getEnergy(Rand& rand) const {
    double flux = 0.0;
    double energy = 0.0;

    do {
        energy = m_photonEnergy + (rand.randomDouble() - 0.5) * m_energySpread;
        flux = schwinger(energy);
    } while ((flux / m_maxFlux - rand.randomDouble()) < 0);
    return energy;
}

/**
 * chooses psi and stokes-vector according to the natural distribution spectrum
 */
RAYX_FN_ACC
PsiAndStokes DipoleSource::getPsiandStokes(double en, Rand& rand) const {
    PsiAndStokes psiandstokes;
    do {
        psiandstokes.psi = (rand.randomDouble() - 0.5) * 6 * m_verDivergence;
        psiandstokes = dipoleFold(psiandstokes.psi, en, m_verEbeamDivergence, rand);
    } while ((psiandstokes.stokes[0]) / m_maxIntensity < rand.randomDouble());

    psiandstokes.psi = psiandstokes.psi * 1e-3;  // psi in rad

    return psiandstokes;
}

/**
 * calculate probability for chosen energy with edge-cases according to H.Wiedemann Synchrotron Radiation P. 259 (D.21)
 */
RAYX_FN_ACC
double DipoleSource::schwinger(double energy) const {
    double preFactor = FACTOR_SCHWINGER_RAY * 1.e-3;

    double Y0 = energy / m_criticalEnergy;
    Y0 = Y0 / 1000;
    double yg0 = 0.0;
    double flux;

    if (Y0 > 0) {
        if (Y0 > 10) {
            yg0 = 0.777 * sqrt(Y0) * glm::pow(ELEMENTARY_CHARGE, -Y0);
        }
        if (Y0 < 1.e-4) {
            yg0 = 1.333 * glm::pow(Y0, (1.0 / 3.0));

        } else {
            double y = log(Y0);
            yg0 = exp(getInterpolation(y));
        }
    }
    flux = preFactor * m_gamma * yg0;

    return flux;
}

RAYX_FN_ACC
double DipoleSource::vDivergence(double energy, double sigv) const {
    double gamma = fabs(m_electronEnergy) * get_factorElectronEnergy();
    if (gamma == 0.0 || m_criticalEnergy == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * glm::pow(m_criticalEnergy * 1000.0 / energy, 0.43);
    return glm::sqrt(glm::pow(psi, 2) + glm::pow(sigv * 0.001, 2));
}

/// monte-Carlo-method to get normal-distributed x and y Values for getXYZPosition()
RAYX_FN_ACC
double DipoleSource::getNormalFromRange(double range, Rand& rand) const {
    double value;
    double Distribution;

    double expanse = -0.5 / range / range;

    do {
        value = (rand.randomDouble() - 0.5) * 9 * range;
        Distribution = exp(expanse * value * value);
    } while (Distribution < rand.randomDouble());

    return value;
}

RAYX_FN_ACC
double DipoleSource::bessel(double hnue, double zeta) {
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

RAYX_FN_ACC
double DipoleSource::getInterpolation(double energy) {
    double functionOne = 0.0;
    double functionTwo = 0.0;
    double result = 0.0;
    int x0Position = 0;

    for (int i = 0; i < int(schwingerLogX.size()) && x0Position < int(schwingerLogX.size()); i++) {
        if (energy < schwingerLogX[i]) {
            break;
        }
        x0Position++;  // TODO: out of bounds checken
    }

    double dx0 = energy - schwingerLogX[x0Position - 1];
    double dx1 = energy - schwingerLogX[x0Position];
    double dx2 = energy - schwingerLogX[x0Position + 1];

    functionOne = (dx0 * schwingerLogY[x0Position] - dx1 * schwingerLogY[x0Position - 1]) / (dx0 - dx1);
    functionTwo = (dx0 * schwingerLogY[x0Position + 1] - dx2 * schwingerLogY[x0Position - 1]) / (dx0 - dx2);

    result = (dx1 * functionTwo - dx2 * functionOne) / (dx1 - dx2);

    return result;
}

RAYX_FN_ACC
PsiAndStokes DipoleSource::dipoleFold(double psi, double photonEnergy, double sigpsi, Rand& rand) const {
    int ln = (int)sigpsi;
    double trsgyp = 0.0;
    double sgyp = 0.0;
    double sy = 0.0;
    double zw = 0.0;
    double wy = 0.0;
    double newpsi = 0.0;

    glm::dvec4 ST = glm::dvec4(0.0, 0.0, 0.0, 0.0);
    glm::dvec4 stokes;

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
            sy = (rand.randomDouble() - 0.5) * sgyp;
            zw = trsgyp * sy * sy;
            wy = exp(zw);
        } while (wy - rand.randomDouble() < 0);

        newpsi = psi + sy;
        stokes = calcStokesSyn(photonEnergy, newpsi, newpsi);

        for (int i = 0; i < 4; i++) {
            ST[i] = ST[i] + stokes[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        stokes[i] = ST[i] / ln;
    }
    PsiAndStokes psiandstokes;
    psiandstokes.psi = newpsi;

    ST[0] = stokes[2] + stokes[3];
    ST[1] = stokes[0];
    ST[2] = 0;
    ST[3] = stokes[1];

    psiandstokes.stokes = ST;

    return psiandstokes;
}

RAYX_FN_ACC
glm::dvec4 DipoleSource::calcStokesSyn(double energy, double psi1, double psi2) const {
    double fak = 3453345200000000.0;  // getFactorDistribution

    double gamma = fabs(m_electronEnergy) * 1957;  // getFactorElectronEnergy
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
        double sign1 = (m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? PI : -PI) / 2;
        double sign2 = psi >= 0.0 ? 1.0 : -1.0;
        double phase = -(sign1 * sign2);
        double x = gamma * psi * 0.001;
        double zeta = glm::pow(1.0 + glm::pow(x, 2), (3.0 / 2.0)) * 0.5 * y0;
        double xkn2 = bessel(xnue2, zeta);
        double xkn1 = bessel(xnue1, zeta);
        double xint = fak * glm::pow(gamma, 2.0) * glm::pow(y0, 2.0) * glm::pow(1.0 + glm::pow(x, 2.0), 2.0);
        double xintp = xint * glm::pow(xkn2, 2.0);
        double xints = xint * (glm::pow(x, 2.0) / (1.0 + glm::pow(x, 2.0)) * glm::pow(xkn1, 2.0));
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

void DipoleSource::setMaxIntensity(Rand& rand) {
    double smax = 0.0;
    double psi = -m_verDivergence;

    for (int i = 1; i < 250; i++) {
        psi = psi + 0.05;
        auto S = dipoleFold(psi, m_photonEnergy, 1.0, rand);
        if (smax < (S.stokes[2] + S.stokes[3])) {
            smax = S.stokes[2] + S.stokes[3];
        } else {
            break;
        }
    }
    m_maxIntensity = smax;
}

// TODO(Sven): this function is weird. it overrides prev member variables
// double DipoleSource::calcMagneticField() {
//     m_magneticFieldStrength = get_factorMagneticField() * fabs(m_electronEnergy) / m_bendingRadius;
//     m_criticalEnergy = get_factorCriticalEnergy() * glm::pow(fabs(m_electronEnergy), 3) / m_bendingRadius;
//
//     m_totalglm::power = get_factorTotalglm::powerDipol() * 0.1 * glm::pow(fabs(m_electronEnergy), 3) * m_magneticFieldStrength *
//     fabs(m_horDivergence) / 1000.0; m_gamma = m_electronEnergy / (ELECTRON_MASS * glm::pow(SPEED_OF_LIGHT, 2) / (ELECTRON_VOLT) * 1.e-9);
//     assert(gamma == m_gamma);
//
//     if (m_gamma >= 1) {
//         return = sqrt(glm::pow(m_gamma, 2) - 1) / m_gamma;
//     } else {
//         return = 1;
//     }
// }

double DipoleSource::calcSourcePathLength() const { return fabs(m_sourcePulseLength) * 1000 * 0.3; }

double DipoleSource::calcPhaseJitter() const { return m_photonEnergy == 0 ? 0 : fabs(m_sourcePulseLength * 0.3) / m_photonWaveLength * 2000000 * PI; }

double DipoleSource::calcHorDivDegrees() const { return m_horDivergence * 180 / PI; }

double DipoleSource::calcHorDivSeconds() const { return m_horDivergence * 180 / PI * 3600; }

double DipoleSource::calcFluxOrg() const {
    double bandwidth = 0.001;

    if (m_energySpread != 0.0) {
        if (m_energySpreadUnit == RAYX::EnergySpreadUnit::EU_PERCENT) {
            bandwidth = m_energySpread / 100.0;
        } else if (m_energySpreadUnit == RAYX::EnergySpreadUnit::EU_eV) {
            bandwidth = m_energySpread / m_photonEnergy;
        }
        bandwidth = m_energySpread / 100;
    }

    return (m_stokes[2] + m_stokes[3]) * (m_horDivergence * 1e-3) * bandwidth * 100.0;
}

void DipoleSource::setMaxFlux() {
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

}  // namespace RAYX
