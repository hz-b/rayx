#include "DipoleSource.h"

#include "Data/xml.h"
#include "Debug/Debug.h"
#include "Random.h"
#include "Shared/Constants.h"
//#include "Shared/TranslatedFortran.h"



namespace RAYX {

double get_factorCriticalEnergy(){
    double planc = Planck_bar / (2*pow(c_elementaryCharge, 5) * pow(c_electronMass,3));
    return 3 * planc * pow(c_electronVolt,2) * 1.0e24; //nach RAY-UI 
}

DipoleSource::DipoleSource(const DesignObject& dobj) : LightSource(dobj) {
    m_energySpreadType = dobj.parseEnergyDistribution();
    m_photonFlux = dobj.parsePhotonFlux();
    m_electronEnergyOrientation = dobj.parseElectronEnergyOrientation();
    m_sourcePulseType = dobj.parseSourcePulseType();
    m_bendingRadius = dobj.parseBendingRadiusDouble();
    m_electronEnergy = dobj.parseElectronEnergy();
    m_photonEnergy = dobj.parsePhotonEnergy();
    m_verEbeamDivergence = dobj.parseVerEbeamDivergence();

    d_sourceWidth = -0.5 * m_sourceWidth * m_sourceWidth;
    d_criticalEnergy = RAYX::get_factorCriticalEnergy();
    d_sigpsi = DipoleSource::vDivergence(m_electronEnergy, d_criticalEnergy, m_photonEnergy, m_verEbeamDivergence);
    d_syn = DipoleSource::syn(m_electronEnergy, d_criticalEnergy, m_photonEnergy, -3*d_sigpsi, 3*d_sigpsi);

    d_flux = d_syn[5] * m_horDivergence * 1.0e-3 * 1.0e2;

}

/**
 * get deviation from main ray according to specified distribution (uniform if
 * hard edge, gaussian if soft edge)) and extent (eg specified width/height of
 * source)
 */
double getRandom(const double extent) {
    if (extent>0) {
        return (randomDouble() - 0.5) * extent;
    } else {
        return randomNormal(0, 1) * extent;
    }
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
std::vector<Ray> DipoleSource::getRays() const {
    double x, x1, y, z, psi, phi,
        en;  // x,y,z pos, psi,phi direction cosines, en=energy
    double sourceWidth2 = 9 * m_sourceWidth; //RAYUI Fortran

    int n = m_numberOfRays;
    std::vector<Ray> rayList;
    rayList.reserve(m_numberOfRays);
    // rayList.reserve(1048576);
    RAYX_VERB << "Create " << n << " rays with standard normal deviation...";

    // create n rays with random position and divergence within the given span
    // for width, height, depth, horizontal and vertical divergence
    for (int i = 0; i < n; i++) {
        
        phi = getRandom( m_horDivergence);

        x1 = getRandom(sourceWidth2);



        x = cos(phi) + m_bendingRadius * (1 - cos(phi)) ;
        x += m_position.x;
        y = getRandom( m_sourceHeight);
        y += m_position.y;
        z = (randomDouble() - 0.5) * m_sourceDepth;
        z += m_position.z;
        en = selectEnergy();  // LightSource.cpp
        // double z = (rn[2] - 0.5) * m_sourceDepth;
        glm::dvec3 position = glm::dvec3(x, y, z);

        // get random deviation from main ray based on distribution
        
        // get corresponding angles based on distribution and deviation from
        // main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
        glm::dvec3 direction = getDirectionFromAngles(phi, psi);
        glm::dvec4 tempDir = m_orientation * glm::dvec4(direction, 0.0);
        direction = glm::dvec3(tempDir.x, tempDir.y, tempDir.z);
        glm::dvec4 stokes = glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

        Ray r = {position, W_UNINIT, direction, en, stokes, 0.0, 0.0, 0.0, 0.0};

        rayList.push_back(r);
    }
    return rayList;
}


double get_factorMagneticField(){
    return c_electronVolt/(c_speedOfLight*c_elementaryCharge)*1.0e9;
} 

double get_factorElectronEnergy(){
    return c_electronVolt *1.0e9 / (c_electronMass * pow(c_speedOfLight,2));
}

double get_factorOmega(){
    return 3 * alpha / (4.0 * pow(M_PI,2) * c_elementaryCharge * pow(c_speedOfLight,4) * pow(c_electronMass,2) / pow(c_electronVolt * 1.0e9,2));
}

double get_factorDistribution(){
    return 3 * alpha / (4.0 * pow(M_PI,2) * c_elementaryCharge);
}

double DipoleSource::vDivergence(double eel, double ec, double hv, double sigv){
    double gamma = fabs(eel)* get_factorElectronEnergy();
    if( gamma == 0.0 || ec == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(ec * 1000.0 / hv, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2) );
}

std::array<double, 6> DipoleSource::syn(double eel, double ec, double hv, double psi1, double psi2){
    double fak = get_factorDistribution();


    double gamma = fabs(eel)*get_factorElectronEnergy();
    double y0 = hv / ec / 1000.0;
    double xnue1 = 1.0/3.0;
    double xnue2 = 2.0/3.0;

    double dpsi = (psi2 - psi1) / 101.0;
    double psi = psi1 + dpsi / 2.0;

    if ( dpsi < 0.001 ){
        dpsi = 0.001;
    }

    std::array<double, 6> result;
    for(int i = 0; i < 6; i++){
        result[i] = 0.0;
    }

    while(psi <= psi2){
        double sign1 = DipoleSource::m_electronEnergyOrientation == ElectronEnergyOrientation::Clockwise ? M_PI_2 : -M_PI_2;
        double sign2 = psi >= 0.0 ? 1.0 : -1.0;
        double phase = -(sign1 * sign2);
        double x = gamma * psi * 0.001;
        double zeta = pow( 1.0 +pow(x, 2), (3.0 / 2.0))*0.5*y0;
        double xkn2 = bessel(xnue2, zeta);
        double xkn1 = bessel(xnue1, zeta);
        double xint = fak * pow(gamma, 2.0) * pow(y0, 2.0) * pow(1.0 + pow (x, 2.0), 2.0);
        double xintp = xint * pow(xkn2, 2.0);
        double xints = xint * (pow(x, 2.0) / (1.0 + pow(x, 2.0)) * pow(xkn1, 2.0));
        xintp = xintp * dpsi * 1e-6;
        xints = xints * dpsi * 1e-6;

        result[0] = result[0] + xintp - xints;
        result[2] = result[2] + 2.0 * sqrt( xintp * xints ) * sin( phase );
        result[3] = result[3] + xintp;
        result[4] = result[4] + xints;
        psi = psi + dpsi;
    }

    result[5] = result[3] + result[4];

    if( result[5] == 0.0 ){
        result[5] = 1.0;
    }

    return result;
}

double DipoleSource::bessel(double hnue,double zeta){
    double result;
    double h = 0.1;
    result = h / 2.0 * exp(-zeta);
    double c1 = 1;
    double c2 = 0;
    for( int i = 1; c1 > c2 ;i++) {
        double cosh1 = (exp(h * i) + exp(-h * i)) / 2.0;
        double cosh2 = (exp(h * i * hnue) + exp(-h * i * hnue)) / 2.0;
        if( (zeta * cosh1) > 225 ){
            return result;
        }
        c1 = h * exp(-zeta * cosh1) * cosh2;
        result = result + c1;
        c2 = result / 1e6;
    }
    return result;
}


void DipoleSource::getMaxIntensity(){
    double psi;
    psi = -d_sigpsi;
    d_sigpsi = 6*d_sigpsi;

    for( int i = 1; i<250; i++){
        psi = psi + 0.05;

        
        
    }
}

std::array<double, 6> DipoleSource::dipoleFold(double psi, double eel, double ec, double hv, double sigpsi){
    int ln = (int)sigpsi;
    double trsgyp;
    double sgyp;
    double sy;
    double zw;
    double wy;
    double psi1;
    int whilee = 0;

    std::array<double, 6> ST = {0,0,0,0,0,0};
    std::array<double, 6> S;
    
    if(sigpsi != 0){
        if(ln > 10){ln = 10;}
        if(ln = 0){ln = 10;}
        trsgyp = -0.5/sigpsi/sigpsi;
        sgyp = 4.0e-3 * sigpsi;
    }
    else
    {
        trsgyp = 0;
        ln = 1;
    }

    for( int i = 1; i <= ln; i++){

        while(whilee == 0)
        {
            sy = (randomDouble()-0,5) * sgyp;
            zw = trsgyp * sy * sy;
            wy = exp(zw);
            if(wy - randomDouble() > 0){whilee = 1;}
        }

        psi1 = psi + sy;
        S = syn(m_electronEnergy, d_criticalEnergy, m_photonEnergy, psi1, psi1);
        
        for( int i = 0; i<6; i++){
            ST[i] = ST[i] + S[i];
        }
    }

    for( int i = 0; i<6; i++){
        S[i] = ST[i] / ln;
    }

    psi = psi1;

}

}  // namespace RAYX
