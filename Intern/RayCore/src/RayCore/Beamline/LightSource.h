#pragma once
#include "Core.h"
#include "glm.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include "Tracer/Ray.h"
#include <string>
#include <random>

namespace RAY
{

    class RAY_API LightSource
    {
    public:

        LightSource(int id, int numberOfRays, const char* name, int spreadType, double photonEnergy, double energySpread, std::vector<double> misalignment);
        const char* getName();
        int getNumberOfRays();
        int getId();
        std::vector<double> getMisalignmentParams();
        double getPhotonEnergy();
        double getEnergySpread();
        int getSpreadType();

        double selectEnergy();
        void setNumberOfRays(int numberOfRays);
        glm::dvec3 getDirectionFromAngles(double phi, double psi);
        // get the rays according to specific light source, has to be implemented in each class that inherits from LightSource
        virtual std::vector<Ray> getRays() = 0;

        LightSource();
        virtual ~LightSource();

    private:
        int m_id;
        const char* m_name;
        int m_numberOfRays;
        std::vector<double> m_misalignmentParams; // x, y, psi, phi
        // std::vector<Ray *> m_rayList; ?

        // point source & matrix source
        enum SPREAD_TYPE {ES_WHITE_BAND, ES_THREE_ENERGIES}; // default ES_WHITE_BAND
        SPREAD_TYPE m_energySpreadType;
        enum ENERGY_DISTRIBUTION_TYPE {ET_FILE, ET_VALUES, ET_TOTAL, ET_PARAM}; // default ET_VALUES
        ENERGY_DISTRIBUTION_TYPE m_energyDistributionType;
        enum SOURCE_DISTRIBUTION_TYPE {ST_SIMULTANEOUS, ST_HARD_EDGE, ST_GAUSS}; // default simultaneously
        SOURCE_DISTRIBUTION_TYPE m_sourceDistributionType;
        double m_energySpread;
        double m_photonEnergy;
        std::uniform_real_distribution<double> m_unif;
        std::default_random_engine re;
        
    };

} // namespace RAY