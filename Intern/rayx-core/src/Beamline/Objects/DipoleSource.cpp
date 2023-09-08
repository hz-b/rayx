#include "DipoleSource.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Debug/Instrumentor.h"
#include "Random.h"
#include "Shared/Constants.h"
#include "Shared/EventType.h"

#include <omp.h>
#include <fstream>
#include <chrono>



namespace RAYX {

double get_factorCriticalEnergy() {
    double planc = 3 * PLANCK_BAR / (2 * pow(SPEED_OF_LIGHT, 5) * pow(ELECTRON_MASS,3)) * pow(ELECTRON_VOLT,2) * 1.0e24; //nach RAY-UI
    return planc; //2.5050652873563215 , 2.2182868570172918
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
    double factorOmega = 3 * FINE_STRUCTURE_CONSTANT / (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE * pow(SPEED_OF_LIGHT, 4) * pow(ELECTRON_MASS, 2) / pow(ELECTRON_VOLT * 1.0e9, 2));
    return factorOmega;
}

double get_factorDistribution() { return 3 * FINE_STRUCTURE_CONSTANT / (4.0 * pow(PI, 2) * ELEMENTARY_CHARGE); }

double get_factorTotalPowerDipol() {
    double totalPower =  pow(ELEMENTARY_CHARGE, 2) / (3 * ELECTRIC_PERMITTIVITY * pow(SPEED_OF_LIGHT, 8) * pow(ELECTRON_MASS, 4)) *
           pow(ELECTRON_VOLT * 1.0E9, 3) / (2 * PI) / (ELECTRON_VOLT / (SPEED_OF_LIGHT * ELEMENTARY_CHARGE));
    return totalPower;
}

DipoleSource::DipoleSource(const DesignObject& dobj) : LightSource(dobj) {
    //auto start = std::chrono::high_resolution_clock::now();

    m_energySpreadType = dobj.parseEnergyDistribution();
    m_photonFlux = dobj.parsePhotonFlux();
    m_electronEnergyOrientation = dobj.parseElectronEnergyOrientation();
    m_sourcePulseType = dobj.parseSourcePulseType();
    m_bendingRadius = dobj.parseBendingRadiusDouble();
    m_electronEnergy = dobj.parseElectronEnergy();
    m_photonEnergy = dobj.parsePhotonEnergy();
    m_verEbeamDivergence = dobj.parseVerEbeamDivergence();
    m_energySpread = dobj.parseEnergySpread();
    m_energySpreadUnit = dobj.parseEnergySpreadUnit();

    m_criticalEnergy = RAYX::get_factorCriticalEnergy();
    m_bandwidth = 1.0e-3;
    m_verDivergence = DipoleSource::vDivergence(m_photonEnergy, m_verEbeamDivergence);
    m_stokes = DipoleSource::getStokesSyn(m_photonEnergy, -3 * m_verDivergence, 3 * m_verDivergence);

    m_gamma = std::fabs(m_electronEnergy) * get_factorElectronEnergy();
    
    
    setLogInterpolation();
    setMaxIntensity();
    setMaxFlux();

    calcFluxOrg();
    calcHorDivDegSec();
    // calcMagneticField();
    calcPhotonWavelength();
    calcSourcePath();
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;
    //double dur = duration.count();
    //m_duration.push_back(dur);
}

/**
 * Creates random rays from dipole source with specified width and height
 * distributed according to either uniform or gaussian distribution across width
 * & height of source the deviation of the direction of each ray from the main
 * ray (0,0,1, phi=psi=0) can also be specified to be uniform or gaussian within
 * a given range (m_verDivergence, m_horDivergence) z-position of ray is always
 * from uniform distribution
 *
 * @returns list of rays
 */

std::vector<Ray> DipoleSource::getRays(int THREAD_COUNT) const {
    RAYX_PROFILE_FUNCTION();
    if (THREAD_COUNT == 0){
        THREAD_COUNT = 1;
        #define DIPOLE_OMP
    }else if(THREAD_COUNT > 1){
        #define DIPOLE_OMP
    }

    double phi, en;  // psi,phi direction cosines, en=energy

    PsiAndStokes psiandstokes;

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    #if defined(DIPOLE_OMP)
    #pragma omp parallel for num_threads(THREAD_COUNT)
    #endif
    for (int i = 0; i < n; i++) {

        phi = (randomDouble() - 0.5) * m_horDivergence; //horDivergence in rad
            
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

        Ray r = {position, ETYPE_UNINIT, direction, en, psiandstokes.stokes, 0.0, 0.0, -1.0, -1.0};
        #if defined(DIPOLE_OMP)
        #pragma omp critical
        {
            rayList.push_back(r);
        }
        #else
            rayList.push_back(r);
        #endif
    }
    
    return rayList;
}


// monte-Carlo-method to get normal-distributed x and y Values for getXYZPosition()
double DipoleSource::getNormalFromRange(double range) const {
    //RAYX_PROFILE_SCOPE("getNormalFromRange");

    double value;
    double Distribution;

    double expanse = -0.5 / range / range;
     
    
    do{   
        value = (randomDouble() - 0.5) * 9 * range;
        Distribution = exp(expanse * value * value);
    } while (Distribution < randomDouble());
    
    return value;
}

glm::dvec3 DipoleSource::getXYZPosition(double phi)const{
    //RAYX_PROFILE_SCOPE("getxyz");
    
    double x1 = getNormalFromRange(m_sourceWidth);
    
    double sign = DipoleSource::m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? -1.0 : 1.0;
    
    double x = sign * (x1 * cos(phi) + (m_bendingRadius * 1000 * (1 - cos(phi))));
    x = x + m_position.x + getMisalignmentParams().m_translationXerror;

    double y = getNormalFromRange(m_sourceHeight);
    y = y + m_position.y + getMisalignmentParams().m_translationYerror;
    
    double z = sign * (m_bendingRadius * 1000 - x1) * sin(phi) + getMisalignmentParams().m_translationZerror;
    
    return glm::dvec3(x, y, z);
}

PsiAndStokes DipoleSource::getPsiandStokes(double en) const {
    //RAYX_PROFILE_SCOPE("getPsiStokes");

    PsiAndStokes psiandstokes;
   
    do {
        psiandstokes.psi = (randomDouble() -0.5) * 6 * m_verDivergence;
        psiandstokes.stokes = dipoleFold(psiandstokes.psi, en, m_verEbeamDivergence);
    } while ((psiandstokes.stokes[0]) / m_maxIntensity < randomDouble());
    
    psiandstokes.psi  = psiandstokes.psi * 1e-3; //psi in rad

    
    return psiandstokes;
}

double DipoleSource::getEnergy() const {
    //RAYX_PROFILE_SCOPE("getEnergy");

    double flux = 0.0;
    double energy = 0.0;

    do {
        energy = m_photonEnergy + (randomDouble() - 0.5) * m_energySpread;
        flux = schwinger(energy);
    } while ((flux / m_maxFlux - randomDouble()) < 0);
    return energy;
}

double DipoleSource::vDivergence(double energy, double sigv) const {
    double gamma = fabs(m_electronEnergy) * get_factorElectronEnergy();  // factorElectronEnergy
    if (gamma == 0.0 || m_criticalEnergy == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(m_criticalEnergy * 1000.0 / energy, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2));
}

glm::dvec4 DipoleSource::getStokesSyn(double energy, double psi1, double psi2) const {
    //RAYX_PROFILE_SCOPE("getStokesSyn");

    double fak = 3453345200000000.0;  // getFactorDistribution

    double gamma = fabs(m_electronEnergy) * 1957;  // getFactorElectronEnergy
    //double ec = m_criticalEnergy * 1000 * pow(m_electronEnergy, 3) / m_bendingRadius;
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
    //RAYX_PROFILE_SCOPE("bessel");

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

double DipoleSource::schwinger(double energy) const {
    //RAYX_PROFILE_SCOPE("schwinger");

    double preFactor = FACTOR_SCHWINGER_RAY * 1.e-3;

    double Y0 = energy / m_criticalEnergy;
    Y0 = Y0 / 1000;
    double yg0 = 0.0;
    double flux;

    if (Y0 > 0) {
        if (Y0 > 10) {
            yg0 = sqrt(PI / 2) * sqrt(energy) * pow(-energy, 2);  // sqrt(PI(T)/2.)*sqrt(z)*exp(-z)
        }
        if (Y0 < 1.e-4) {
            yg0 = 2.1495282415 * pow(energy, (1.0 / 3.0));
        } else {
            double y = log(Y0);
            yg0 = exp(getInterpolation(y));
        }
        // yg0 = linear oder parabolic?
    }

    flux = preFactor * m_gamma * yg0;

    return flux;
}

void DipoleSource::setMaxIntensity() {
    double smax = 0.0;
    double psi = -m_verDivergence;

 
    for (int i = 1; i < 250; i++) {
        psi = psi + 0.05;
        auto  S = dipoleFold(psi, m_photonEnergy, 1.0);
        if (smax < (S[2] + S[3])) {
            smax = S[2] + S[3];
        } else {
            break;
        }
    }
    m_maxIntensity = smax;
}

glm::dvec4 DipoleSource::dipoleFold(double psi, double photonEnergy, double sigpsi) const {
    //RAYX_PROFILE_SCOPE("dipolefold");
        
    int ln = (int)sigpsi;
    double trsgyp = 0.0;
    double sgyp = 0.0;
    double sy = 0.0;
    double zw = 0.0;
    double wy = 0.0;
    double psi1 = 0.0;

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
   //for
    for (int i = 1; i <= ln; i++) {
        do {
            sy = (randomDouble() - 0.5) * sgyp;
            zw = trsgyp * sy * sy;
            wy = exp(zw);
        } while (wy - randomDouble() < 0);

        psi1 = psi + sy;
        Stokes = getStokesSyn(photonEnergy, psi1, psi1);


        for (int i = 0; i < 4; i++) {
            ST[i] = ST[i] + Stokes[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        Stokes[i] = ST[i] / ln;
    }

    psi = psi1;

    ST[0] = Stokes[2] + Stokes[3];
    ST[1] = Stokes[0];
    ST[2] = 0;
    ST[3] = Stokes[1];

    return ST;
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

void DipoleSource::calcPhotonWavelength() {
    // Energy Distribution Type : Values only
    m_photonWaveLength = m_photonEnergy == 0.0 ? 0 : inm2eV / m_photonEnergy;
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
            bandwidth = m_energySpread/100.0;
        } else if (m_energySpreadUnit == RAYX::EnergySpreadUnit::EU_eV) {
            bandwidth = m_energySpread / m_photonEnergy;
        }
        bandwidth = m_energySpread / 100;
    }

    m_flux = (m_stokes[2] + m_stokes[3]) * (m_horDivergence * 1e-3) * bandwidth * 100.0;
}

void DipoleSource::setMaxFlux() {
    //RAYX_PROFILE_SCOPE("setMaxFlux");
    double EMAXS = 285.81224786 * m_criticalEnergy; //welche Zahl ist das?
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

double DipoleSource::getInterpolation(double energy) const {
    //RAYX_PROFILE_SCOPE("getInterpolation");

    //TODO: Interpolation benchmarken 
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

}  // namespace RAYX

