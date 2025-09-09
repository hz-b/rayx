#include "DipoleSource.h"

#include <fstream>

#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Design/DesignSource.h"
#include "Random.h"
#include "Rml/xml.h"
#include "Shader/Constants.h"
#include "Shader/EventType.h"
#include "Shader/Utils.h"

namespace RAYX {

// TODO: why does this source get seeded with randomDouble in the constructor?
// TODO: do we only use schwinger log?
// TODO: what about unused functions?

// get the Energydistribution with arrays of the functioncurve
// H. Winick, S. Doniach, Synchrotron Radiation Research P.23f (y) and (G0(y))
/*
 * unused
 *
constexpr __device__ std::array<double, 59> m_schwingerX = {
    1.e-4, 1.e-3, 2.e-3, 4.e-3, 6.e-3, 8.e-3, 1.e-2, 2.e-2, 3.e-2, 4.e-2, 5.e-2, 6.e-2, 7.e-2, 8.e-2, 9.e-2, 1.e-1, 0.15, 0.2,  0.25, 0.3,
    0.35,  0.4,   0.45,  0.5,   0.55,  0.6,   0.65,  0.7,   0.75,  0.8,   0.85,  0.9,   1.0,   1.25,  1.5,   1.75,  2.0,  2.25, 2.5,  2.75,
    3.0,   3.25,  3.5,   3.75,  4.0,   4.25,  4.5,   4.75,  5.0,   5.5,   6.0,   6.5,   7.0,   7.5,   8.0,   8.5,   9.0,  9.5,  10.0};

constexpr __device__ std::array<double, 59> m_schwingerY = {
    973.0,    213.6,    133.6,    83.49,    63.29,    51.92,    44.5,     27.36,    20.45,    16.57,    14.03,    12.22,
    10.85,    9.777,    8.905,    8.182,    5.832,    4.517,    3.663,    3.059,    2.607,    2.255,    1.973,    1.742,
    1.549,    1.386,    1.246,    1.126,    1.02,     9.28e-1,  8.465e-1, 7.74e-1,  6.514e-1, 4.359e-1, 3.004e-1, 2.113e-1,
    1.508e-1, 1.089e-1, 7.926e-2, 5.811e-2, 4.286e-2, 3.175e-2, 2.362e-2, 1.764e-2, 1.321e-2, 9.915e-3, 7.461e-3, 5.626e-3,
    4.25e-3,  2.436e-3, 1.404e-3, 8.131e-4, 4.842e-4, 2.755e-4, 1.611e-4, 9.439e-5, 5.543e-5, 3.262e-5, 1.922e-5};
*/

RAYX_CONSTEXPR_ACC std::array<double, 59> m_schwingerLogX = {
    -9.210340371976182,   -6.907755278982137,   -6.214608098422191,
    -5.521460917862246,   -5.115995809754082,   -4.8283137373023015,
    -4.605170185988091,   -3.912023005428146,   -3.506557897319982,
    -3.2188758248682006,  -2.995732273553991,   -2.8134107167600364,
    -2.659260036932778,   -2.5257286443082556,  -2.4079456086518722,
    -2.3025850929940455,  -1.8971199848858813,  -1.6094379124341003,
    -1.3862943611198906,  -1.2039728043259361,  -1.0498221244986778,
    -0.916290731874155,   -0.7985076962177716,  -0.6931471805599453,
    -0.5978370007556204,  -0.5108256237659907,  -0.4307829160924542,
    -0.35667494393873245, -0.2876820724517809,  -0.2231435513142097,
    -0.16251892949777494, -0.10536051565782628, 0,
    0.22314355131420976,  0.4054651081081644,   0.5596157879354227,
    0.6931471805599453,   0.8109302162163288,   0.9162907318741551,
    1.0116009116784799,   1.0986122886681098,   1.1786549963416462,
    1.252762968495368,    1.3217558399823195,   1.3862943611198906,
    1.4469189829363254,   1.5040773967762742,   1.55814461804655,
    1.6094379124341003,   1.7047480922384253,   1.791759469228055,
    1.8718021769015913,   1.9459101490553132,   2.0149030205422647,
    2.0794415416798357,   2.1400661634962708,   2.1972245773362196,
    2.2512917986064953,   2.302585092994046,
};

RAYX_CONSTEXPR_ACC std::array<double, 59> m_schwingerLogY = {
    -2.3299562897901778,  -1.5436501718960973,  -1.3197578373196464,  -1.0967340536563375,  -0.968268470966868,   -0.8786096649064665,
    -0.8096809968158968,  -0.6029409126737963,  -0.48857501483117105, -0.4112819934297291,  -0.3545343794396214,  -0.31033676301658714,
    -0.2750949569463094,  -0.2456959557847213,  -0.22133269191024715, -0.2006484734868388,  -0.1337599901795244,  -0.10158985576163317,
    -0.08801187732321335, -0.08588473915716963, -0.09162198954731303, -0.10314075891951337, -0.11895246917729328, -0.13811330212963427,
    -0.1602274393208889,  -0.1844037229982793,  -0.21084449572719277, -0.23800341422123386, -0.2678794451556012,  -0.2978670975101462,
    -0.329164006906722,   -0.36154392105023625, -0.42863138614000396, -0.6071988684235804,  -0.797175250983993,   -0.9948605664247034,
    -1.198653642848336,   -1.4063950328268935,  -1.618730959345693,   -1.8338166012166428,  -2.0512040002685965,  -2.2712075573017905,
    -2.492898499717553,   -2.7158303884211725,  -2.9404867993280126,  -3.1667875340740523,  -3.3939884287164857,  -3.6222119508679222,
    -3.851398383611711,   -4.312649836896061,   -4.776670504150463,   -5.242854277848891,   -5.6871023644050025,  -6.18201990882904,
    -6.654043726101509,   -7.12800925913115,    -7.603165017196341,   -8.07929316216491,    -8.556974061428082,
};

double get_factorCriticalEnergy() {
    double planc = 3 * PLANCK_BAR / (2 * pow(SPEED_OF_LIGHT, 5) * pow(ELECTRON_MASS, 3)) * pow(ELECTRON_VOLT, 2) * 1.0e24;  // nach RAY-UI
    return planc;  // 2.5050652873563215 , 2.2182868570172918
}

/*
 * unused
 *
double get_factorMagneticField() {
    double magElectricField = ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE) * 1.0e9;
    return magElectricField;
}

double get_factorDistribution() { return 3 * FINE_STRUCTURE_CONSTANT / (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE); }

double get_factorTotalPowerDipol() {
    double totalPower = pow(ELEMENTARY_CHARGE, 2) / (3 * ELECTRIC_PERMITTIVITY * pow(SPEED_OF_LIGHT, 8) * pow(ELECTRON_MASS, 4)) *
                        pow(ELECTRON_VOLT * 1.0E9, 3) / (2 * PI) / (ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE));
    return totalPower;
}
*/

double get_factorElectronEnergy() {
    double factorElectronEnergy = ELECTRON_VOLT * 1.0e9 / (ELECTRON_MASS * pow(SPEED_OF_LIGHT, 2));
    return factorElectronEnergy;
}

double get_factorOmega() {
    double factorOmega = 3 * FINE_STRUCTURE_CONSTANT /
                         (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE * pow(SPEED_OF_LIGHT, 4) * pow(ELECTRON_MASS, 2) / pow(ELECTRON_VOLT * 1.0e9, 2));
    return factorOmega;
}

RAYX_FN_ACC
double dipoleBessel(double hnue, double zeta) {
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
glm::dvec4 getStokesSyn(double energy, double psi1, double psi2, double electronEnergy, double criticalEnergy,
                        ElectronEnergyOrientation electronEnergyOrientation) {
    double fak = 3453345200000000.0;  // getFactorDistribution

    double gamma = fabs(electronEnergy) * 1957;  // getFactorElectronEnergy
    double y0 = energy / criticalEnergy / 1000.0;
    double xnue1 = 1.0 / 3.0;
    double xnue2 = 2.0 / 3.0;

    double dpsi = (psi2 - psi1) / 101.0;
    double psi = psi1 + dpsi / 2.0;

    if (dpsi < 0.001) {
        dpsi = 0.001;
    }

    glm::dvec4 stokes = glm::dvec4(0.0, 0.0, 0.0, 0.0);

    while (psi <= psi2) {
        double sign1 = (electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? PI : -PI) / 2;
        double sign2 = psi >= 0.0 ? 1.0 : -1.0;
        double phase = -(sign1 * sign2);
        double x = gamma * psi * 0.001;
        double zeta = pow(1.0 + pow(x, 2), (3.0 / 2.0)) * 0.5 * y0;
        double xkn2 = dipoleBessel(xnue2, zeta);
        double xkn1 = dipoleBessel(xnue1, zeta);
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

RAYX_FN_ACC
PsiAndStokes calcDipoleFold(double psi, double photonEnergy, double sigpsi, double electronEnergy, double criticalEnergy,
                            ElectronEnergyOrientation electronEnergyOrientation, Rand& __restrict rand) {
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
            sy = (rand.randomDouble() - 0.5) * sgyp;
            zw = trsgyp * sy * sy;
            wy = exp(zw);
        } while (wy - rand.randomDouble() < 0);

        newpsi = psi + sy;
        Stokes = getStokesSyn(photonEnergy, newpsi, newpsi, electronEnergy, criticalEnergy, electronEnergyOrientation);

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

double calcMaxIntensity(double photonEnergy, double verDivergence, double electronEnergy, double criticalEnergy,
                        ElectronEnergyOrientation electronEnergyOrientation, Rand& __restrict rand) {
    double smax = 0.0;
    double psi = -verDivergence;

    for (int i = 1; i < 250; i++) {
        psi = psi + 0.05;
        auto S = calcDipoleFold(psi, photonEnergy, 1.0, electronEnergy, criticalEnergy, electronEnergyOrientation, rand);
        if (smax < (S.stokes[2] + S.stokes[3])) {
            smax = S.stokes[2] + S.stokes[3];
        } else {
            break;
        }
    }
    return smax;
}

/*
 * unused
 *
double calcFluxOrg(double photonEnergy, double energySpread, EnergySpreadUnit energySpreadUnit, double horDivergence, Stokes stokes) {
    double bandwidth = 0.001;

    if (energySpread != 0.0) {
        if (energySpreadUnit == EnergySpreadUnit::EU_PERCENT) {
            bandwidth = energySpread / 100.0;
        } else if (energySpreadUnit == EnergySpreadUnit::EU_eV) {
            bandwidth = energySpread / photonEnergy;
        }
        bandwidth = energySpread / 100;
    }

    return (stokes[2] + stokes[3]) * (horDivergence * 1e-3) * bandwidth * 100.0;
}
*/

double calcVerDivergence(double energy, double sigv, double electronEnergy, double criticalEnergy) {
    double gamma = fabs(electronEnergy) * get_factorElectronEnergy();
    if (gamma == 0.0 || criticalEnergy == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(criticalEnergy * 1000.0 / energy, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2));
}

/*
 * unused
 *
struct SourcePath {
    double sourcePathLength;
    double phaseJitter;
};
SourcePath calcSourcePath(double photonEnergy, double photonWaveLength, double sourcePulseLength) {
    const auto sourcePathLength = fabs(sourcePulseLength) * 1000 * 0.3;
    const auto phaseJitter = photonEnergy == 0 ? 0 : fabs(sourcePulseLength * 0.3) / photonWaveLength * 2000000 * PI;
    return {.sourcePathLength = sourcePathLength, .phaseJitter = phaseJitter};
}

struct HorDivDegSec {
    double horDivDegrees;
    double horDivSeconds;
};
HorDivDegSec calcHorDivDegSec(double horDivergence) {
    const auto horDivDegrees = horDivergence * 180.0 / PI;
    const auto horDivSeconds = horDivergence * 180.0 / PI * 3600.0;
    return {.horDivDegrees = horDivDegrees, .horDivSeconds = horDivSeconds};
}

double calcMagneticField(double electronEnergy, double horDivergence, double bendingRadius) {
    const auto magneticFieldStrength = get_factorMagneticField() * fabs(electronEnergy) / bendingRadius;
    const auto criticalEnergy = get_factorCriticalEnergy() * pow(fabs(electronEnergy), 3) / bendingRadius;

    const auto totalPower = get_factorTotalPowerDipol() * 0.1 * pow(fabs(electronEnergy), 3) * magneticFieldStrength * fabs(horDivergence) / 1000.0;
    const auto gamma = electronEnergy / (ELECTRON_MASS * pow(SPEED_OF_LIGHT, 2) / (ELECTRON_VOLT) * 1.e-9);

    if (gamma >= 1) {
        const auto beta = sqrt(pow(gamma, 2) - 1) / gamma;
        return beta;
    } else {
        return 1;
    }
}
*/

RAYX_FN_ACC
double getDipoleInterpolation(double energy) {
    double functionOne = 0.0;
    double functionTwo = 0.0;
    double result = 0.0;
    int x0Position = 0;

    for (int i = 0; i < int(m_schwingerLogX.size()) && x0Position < int(m_schwingerLogX.size()); i++) {
        if (energy < m_schwingerLogX[i]) {
            break;
        }
        x0Position++;  // TODO: out of bounds checken
    }

    double dx0 = energy - m_schwingerLogX[x0Position - 1];
    double dx1 = energy - m_schwingerLogX[x0Position];
    double dx2 = energy - m_schwingerLogX[x0Position + 1];

    functionOne = (dx0 * m_schwingerLogY[x0Position] - dx1 * m_schwingerLogY[x0Position - 1]) / (dx0 - dx1);
    functionTwo = (dx0 * m_schwingerLogY[x0Position + 1] - dx2 * m_schwingerLogY[x0Position - 1]) / (dx0 - dx2);

    result = (dx1 * functionTwo - dx2 * functionOne) / (dx1 - dx2);

    return result;
}

/**
 * calculate probability for chosen energy with edge-cases according to H.Wiedemann Synchrotron Radiation P. 259 (D.21)
 */
RAYX_FN_ACC
double schwinger(double energy, double gamma, double criticalEnergy) {
    double preFactor = FACTOR_SCHWINGER_RAY * 1.e-3;

    double Y0 = energy / criticalEnergy;
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
            yg0 = exp(getDipoleInterpolation(y));
        }
    }
    flux = preFactor * gamma * yg0;

    return flux;
}

double calcMaxFlux(double photonEnergy, double energySpread, double criticalEnergy, double gamma) {
    double EMAXS = 285.81224786 * criticalEnergy;  // welche Zahl ist das?
    double Emax = photonEnergy + energySpread / 2;
    double Emin = photonEnergy - energySpread / 2;

    if (Emax < EMAXS) {
        return schwinger(Emax, gamma, criticalEnergy);
    } else if (Emin > EMAXS) {
        return schwinger(Emin, gamma, criticalEnergy);
    } else {
        return schwinger(EMAXS, gamma, criticalEnergy);
    }
}

double calcGamma(double electronEnergy) { return std::fabs(electronEnergy) * get_factorElectronEnergy(); }

DipoleSource::DipoleSource(const DesignSource& dSource)
    : ModelLightSource(dSource),
      m_bendingRadius(dSource.getBendingRadius()),
      m_electronEnergyOrientation(dSource.getElectronEnergyOrientation()),
      // m_photonFlux(dSource.getPhotonFlux()),
      m_sourceHeight(dSource.getSourceHeight()),
      m_sourceWidth(dSource.getSourceWidth()),
      m_electronEnergy(dSource.getElectronEnergy()),
      m_criticalEnergy(get_factorCriticalEnergy()),
      m_photonEnergy(dSource.getEnergy()),
      m_verEbeamDivergence(dSource.getVerEBeamDivergence()),
      // m_bandwidth(1.0e-3),
      // m_photonWaveLength(hvlam(m_photonEnergy)),
      m_energySpread(dSource.getEnergySpread()),
      m_horDivergence(dSource.getHorDivergence()) {
    auto rand = Rand(randomUint());
    m_gamma = calcGamma(m_electronEnergy);
    m_verDivergence = calcVerDivergence(m_photonEnergy, m_verEbeamDivergence, m_electronEnergy, m_criticalEnergy);
    // m_stokes = DipoleSource::getStokesSyn(m_photonEnergy, -3 * m_verDivergence, 3 * m_verDivergence);
    m_maxIntensity = calcMaxIntensity(m_photonEnergy, m_verDivergence, m_electronEnergy, m_criticalEnergy, m_electronEnergyOrientation, rand);
    m_maxFlux = calcMaxFlux(m_photonEnergy, m_energySpread, m_criticalEnergy, m_gamma);
    // m_flux = calcFluxOrg(m_photonEnergy, m_energySpread, dSource.getEnergySpreadUnit(), m_horDivergence, m_stokes);
}

/**
 * Creates random ray from dipole source
 *
 * with natural X, Z Position on the bending radius
 * with natural energy distribution by Schwinger (see Doku)
 * with natural psi and polarisation distribution (see Doku)
 *
 * @returns Ray
 */
RAYX_FN_ACC
Ray DipoleSource::genRay(const SourceId sourceId, Rand& __restrict rand) const {
    double phi, en;  // phi=horizontal Angle, en=energy

    // create ray with random position and divergence within the given span
    // for width, height, horizontal and vertical divergence
    phi = (rand.randomDouble() - 0.5) * m_horDivergence;  // chooses phi in given Divergence

    glm::dvec3 position = getXYZPosition(phi, rand);

    en = getEnergy(rand);  // Verteilung nach Schwingerfunktion

    auto psiandstokes = getPsiandStokes(en, rand);
    psiandstokes.psi = psiandstokes.psi + m_misalignmentParams.m_rotationYerror.rad;

    phi = phi + m_misalignmentParams.m_rotationXerror.rad;

    // get corresponding angles based on distribution and deviation from
    // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
    glm::dvec3 direction = getDirectionFromAngles(phi, psiandstokes.psi);
    glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
    direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);

    const auto field = stokesToElectricField(psiandstokes.stokes, m_orientation);

    return Ray{
        .m_position = position,
        .m_eventType = EventType::Emitted,
        .m_direction = direction,
        .m_energy = en,
        .m_field = field,
        .m_pathLength = 0.0,
        .m_order = 0,
        .m_lastElement = -1,
        .m_sourceID = sourceId,
    };
}

/**
 * calculates x, z position based on given horizontal angle
 * takes bending radius in the dipole into account
 * chooses y position in given source hight
 * */
RAYX_FN_ACC
glm::dvec3 DipoleSource::getXYZPosition(double phi, Rand& __restrict rand) const {
    double x1 = getNormalFromRange(m_sourceWidth, rand);

    double sign = m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? -1.0 : 1.0;

    double x = sign * (x1 * cos(phi) + (m_bendingRadius * 1000 * (1 - cos(phi))));  // bendingRadius in mm
    x = x + m_position.x + m_misalignmentParams.m_translationXerror;

    double y = getNormalFromRange(m_sourceHeight, rand);
    y = y + m_position.y + m_misalignmentParams.m_translationYerror;

    double z = sign * (m_bendingRadius * 1000 - x1) * sin(phi) + m_misalignmentParams.m_translationZerror;

    return glm::dvec3(x, y, z);
}

/// monte-Carlo-method to get normal-distributed x and y Values for getXYZPosition()
RAYX_FN_ACC
double DipoleSource::getNormalFromRange(double range, Rand& __restrict rand) const {
    double value;
    double Distribution;

    double expanse = -0.5 / range / range;

    do {
        value = (rand.randomDouble() - 0.5) * 9 * range;
        Distribution = exp(expanse * value * value);
    } while (Distribution < rand.randomDouble());

    return value;
}

/**
 * chooses photon energy according to the natural energy distribution spectrum by schwinger
 */
RAYX_FN_ACC
double DipoleSource::getEnergy(Rand& __restrict rand) const {
    double flux = 0.0;
    double energy = 0.0;

    do {
        energy = m_photonEnergy + (rand.randomDouble() - 0.5) * m_energySpread;
        flux = schwinger(energy, m_gamma, m_criticalEnergy);
    } while ((flux / m_maxFlux - rand.randomDouble()) < 0);
    return energy;
}

/**
 * chooses psi and stokes-vector according to the natural distribution spectrum
 */
RAYX_FN_ACC
PsiAndStokes DipoleSource::getPsiandStokes(double en, Rand& __restrict rand) const {
    PsiAndStokes psiandstokes;
    do {
        psiandstokes.psi = (rand.randomDouble() - 0.5) * 6 * m_verDivergence;
        psiandstokes =
            calcDipoleFold(psiandstokes.psi, en, m_verEbeamDivergence, m_electronEnergy, m_criticalEnergy, m_electronEnergyOrientation, rand);
    } while ((psiandstokes.stokes[0]) / m_maxIntensity < rand.randomDouble());

    psiandstokes.psi = psiandstokes.psi * 1e-3;  // psi in rad

    return psiandstokes;
}

}  // namespace RAYX
