//
// Copied from RAY-UI/LightSource/dipolesource.cpp
//

float DipoleSource::vDivergence(float eel, float ec, float hv, float sigv) {
    float gamma = fabs(eel) * get_factorElectronEnergy();
    if (gamma == 0.0 || ec == 0.0) {
        return 0;
    }
    float psi = get_factorOmega() * 1.e-18 * 0.1 / gamma * pow(ec * 1000.0 / hv, 0.43);
    return sqrt(pow(psi, 2) + pow(sigv * 0.001, 2));
}

std::list<float> DipoleSource::syn(float eel, float ec, float hv, float psi1, float psi2) {
    float fak = get_factorDistribution();

    float gamma = fabs(eel) * get_factorElectronEnergy();
    float y0 = hv / ec / 1000.0;
    float xnue1 = 1.0 / 3.0;
    float xnue2 = 2.0 / 3.0;

    float dpsi = (psi2 - psi1) / 101.0;
    float psi = psi1 + dpsi / 2.0;

    if (dpsi < 0.001) {
        dpsi = 0.001;
    }

    std::list<float> result;
    for (int i = 0; i < 6; i++) {
        result << 0.0;
    }

    while (psi <= psi2) {
        float sign1 = m_energyOrienation == EE_CLOCKWISE ? M_PI_2 : -M_PI_2;
        float sign2 = psi >= 0.0 ? 1.0 : -1.0;
        float phase = -(sign1 * sign2);
        float x = gamma * psi * 0.001;
        float zeta = pow(1.0 + pow(x, 2), (3.0 / 2.0)) * 0.5 * y0;
        float xkn2 = bessel(xnue2, zeta);
        float xkn1 = bessel(xnue1, zeta);
        float xint = fak * pow(gamma, 2.0) * pow(y0, 2.0) * pow(1.0 + pow(x, 2.0), 2.0);
        float xintp = xint * pow(xkn2, 2.0);
        float xints = xint * (pow(x, 2.0) / (1.0 + pow(x, 2.0)) * pow(xkn1, 2.0));
        xintp = xintp * dpsi * 1e-6;
        xints = xints * dpsi * 1e-6;

        result[0] = result[0] + xintp - xints;
        result[2] = result[2] + 2.0 * sqrt(xintp * xints) * sin(phase);
        result[3] = result[3] + xintp;
        result[4] = result[4] + xints;
        psi = psi + dpsi;
    }

    result[5] = result[3] + result[4];

    if (result[5] == 0.0) {
        result[5] = 1.0;
    }

    return result;
}

float DipoleSource::bessel(float hnue, float zeta) {
    float result;
    float h = 0.1;
    result = h / 2.0 * exp(-zeta);
    float c1 = 1;
    float c2 = 0;
    for (int i = 1; c1 > c2; i++) {
        float cosh1 = (exp(h * i) + exp(-h * i)) / 2.0;
        float cosh2 = (exp(h * i * hnue) + exp(-h * i * hnue)) / 2.0;
        if ((zeta * cosh1) > 225) {
            return result;
        }
        c1 = h * exp(-zeta * cosh1) * cosh2;
        result = result + c1;
        c2 = result / 1e6;
    }
    return result;
}
