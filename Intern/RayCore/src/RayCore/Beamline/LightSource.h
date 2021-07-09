#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <random>

#include "glm.hpp"

#include "Core.h"
#include "Tracer/Ray.h"
#include "BeamlineObject.h"

namespace RAY
{

    class RAY_API LightSource : public BeamlineObject
    {
    public:

        LightSource(const int id, const int numberOfRays, const char* name, const int spreadType, const double photonEnergy, const double energySpread, const std::vector<double> misalignment);
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
        int m_numberOfRays;
        // std::string m_name;
        std::vector<double> m_misalignmentParams; // x, y, psi, phi
        // std::vector<Ray *> m_rayList; ?

        // point source & matrix source
        enum SPREAD_TYPE { ES_WHITE_BAND, ES_THREE_ENERGIES }; // default ES_WHITE_BAND
        SPREAD_TYPE m_energySpreadType;
        enum ENERGY_DISTRIBUTION_TYPE { ET_FILE, ET_VALUES, ET_TOTAL, ET_PARAM }; // default ET_VALUES
        ENERGY_DISTRIBUTION_TYPE m_energyDistributionType;
        enum SOURCE_DISTRIBUTION_TYPE { ST_SIMULTANEOUS, ST_HARD_EDGE, ST_GAUSS }; // default simultaneously
        SOURCE_DISTRIBUTION_TYPE m_sourceDistributionType;
        double m_energySpread;
        double m_photonEnergy;
        std::uniform_real_distribution<double> m_unif;
        std::default_random_engine re;

    };

} // namespace RAY