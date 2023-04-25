//
// Copied from RAY-UI/LightSource/dipolesource.cpp
//

double DipoleSource::vDivergence(double eel, double ec, double hv, double sigv){
    double gamma = fabs(eel)*get_factorElectronEnergy();
    if( gamma == 0.0 || ec == 0.0) {
        return 0;
    }
    double psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(ec * 1000.0 / hv, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2) );
}

List<double> DipoleSource::syn(double eel, double ec, double hv, double psi1, double psi2){
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

    QList<double> result;
    for(int i = 0; i < 6; i++){
        result << 0.0;
    }

    while(psi <= psi2){
        double sign1 = m_energyOrienation == EE_CLOCKWISE ? M_PI_2 : -M_PI_2;
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

