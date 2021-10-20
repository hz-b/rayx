#include "PointSource.h"
#include <Data/xml.h>
#include <cassert>
#include <cmath>

namespace RAYX
{


    PointSource::PointSource(const std::string name, const int spreadType,
        const double sourceWidth, const double sourceHeight, const double sourceDepth, const double horDivergence,
        const double verDivergence, const int widthDist, const int heightDist, const int horDist, const int verDist,
        const double photonEnergy, const double energySpread, const double linPol0, const double linPol45, const double circPol, const std::vector<double> misalignment)
        : LightSource(name.c_str(), spreadType, photonEnergy, energySpread, linPol0, linPol45, circPol, misalignment, sourceDepth, sourceHeight, sourceWidth, horDivergence, verDivergence)
    {
        m_widthDist = widthDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_heightDist = heightDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_horDist = horDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        m_verDist = verDist == 0 ? SD_HARDEDGE : SD_GAUSSIAN;
        std::normal_distribution<double> m_stdnorm(0, 1);
        std::uniform_real_distribution<double> m_uniform(0, 1);
        std::default_random_engine m_re;

    }

    PointSource::~PointSource() {}

    // returns nullptr on error
    std::shared_ptr<PointSource> PointSource::createFromXML(rapidxml::xml_node<>* node) {
        const std::string name = node->first_attribute("name")->value();

        int numberOfRays;
        if (!xml::paramInt(node, "numberRays", &numberOfRays)) { return nullptr; }

        int spreadType;
        if (!xml::paramInt(node, "energySpreadType", &spreadType)) { return nullptr; }

        double sourceWidth;
        if (!xml::paramDouble(node, "sourceWidth", &sourceWidth)) { return nullptr; }

        double sourceHeight;
        if (!xml::paramDouble(node, "sourceHeight", &sourceHeight)) { return nullptr; }

        double sourceDepth;
        if (!xml::paramDouble(node, "sourceDepth", &sourceDepth)) { return nullptr; }

        double horDivergence;
        if (!xml::paramDouble(node, "horDiv", &horDivergence)) { return nullptr; }

        double verDivergence;
        if (!xml::paramDouble(node, "verDiv", &verDivergence)) { return nullptr; }

        int widthDist;
        if (!xml::paramInt(node, "sourceWidthDistribution", &widthDist)) { return nullptr; }

        int heightDist;
        if (!xml::paramInt(node, "sourceHeightDistribution", &heightDist)) { return nullptr; }

        int horDist;
        if (!xml::paramInt(node, "horDivDistribution", &horDist)) { return nullptr; }

        int verDist;
        if (!xml::paramInt(node, "verDivDistribution", &verDist)) { return nullptr; }

        double photonEnergy;
        if (!xml::paramDouble(node, "photonEnergy", &photonEnergy)) { return nullptr; }

        double energySpread;
        if (!xml::paramDouble(node, "energySpread", &energySpread)) { return nullptr; }

        double linPol0;
        if (!xml::paramDouble(node, "linearPol_0", &linPol0)) { return nullptr; }

        double linPol45;
        if (!xml::paramDouble(node, "linearPol_45", &linPol45)) { return nullptr; }

        double circPol;
        if (!xml::paramDouble(node, "circularPol", &circPol)) { return nullptr; }

        std::vector<double> misalignment(6, 0.f);

        if (xml::paramYes(node, "alignmentError")) { // TODO(rudi): is this the right condition?
            // all misalignment-values will be left at 0 if they are missing.
            xml::paramDouble(node, "translationXerror", &misalignment[0]);
            xml::paramDouble(node, "translationYerror", &misalignment[1]);
            xml::paramDouble(node, "translationZerror", &misalignment[2]);
            xml::paramDouble(node, "rotationXerror", &misalignment[3]);
            xml::paramDouble(node, "rotationYerror", &misalignment[4]);
            xml::paramDouble(node, "rotationZerror", &misalignment[5]);
        }


        return std::make_shared<PointSource>(
            name, spreadType, sourceWidth, sourceHeight, sourceDepth,
            horDivergence, verDivergence, widthDist, heightDist, horDist, verDist, photonEnergy, energySpread,
            linPol0, linPol45, circPol, misalignment
        );
    }

    /**
     * creates random rays from point source with specified width and height
     * distributed according to either uniform or gaussian distribution across width & height of source
     * the deviation of the direction of each ray from the main ray (0,0,1, phi=psi=0) can also be specified to be
     * uniform or gaussian within a given range (m_verDivergence, m_horDivergence)
     * z-position of ray is always from uniform distribution
     *
     * returns list of rays
     */
    std::vector<Ray> PointSource::getRays() {
        double x, y, z, psi, phi, en; //x,y,z pos, psi,phi direction cosines, en=energy

        int n = SimulationEnv::get().m_numOfRays;
        std::vector<Ray> rayVector;
        rayVector.reserve(1048576);
        std::cout << "create " << n << " rays with standard normal deviation..." << std::endl;

        // create n rays with random position and divergence within the given span for width, height, depth, horizontal and vertical divergence
        for (int i = 0; i < n; i++) {
            x = getCoord(m_widthDist, m_sourceWidth) + getMisalignmentParams()[0];
            y = getCoord(m_heightDist, m_sourceHeight) + getMisalignmentParams()[1];
            z = (m_uniformDist(m_randEngine) - 0.5) * m_sourceDepth;
            en = selectEnergy(); // LightSource.cpp
            //double z = (rn[2] - 0.5) * m_sourceDepth;
            glm::dvec3 position = glm::dvec3(x, y, z);

            // get random deviation from main ray based on distribution
            psi = getCoord(m_verDist, m_verDivergence) + getMisalignmentParams()[2];
            phi = getCoord(m_horDist, m_horDivergence) + getMisalignmentParams()[3];
            // get corresponding angles based on distribution and deviation from main ray (main ray: xDir=0,yDir=0,zDir=1 for phi=psi=0)
            glm::dvec3 direction = getDirectionFromAngles(phi, psi);
            glm::dvec4 stokes = glm::dvec4(1, getLinear0(), getLinear45(), getCircular());

            Ray r = Ray(position, direction, stokes, en, 1.0);
            rayVector.emplace_back(r);
        }
        std::cout << &(rayVector[0]) << std::endl;
        //rayVector.resize(1048576);
        return rayVector;
    }

    /**
     * get deviation from main ray according to specified distribution (uniform if hard edge, gaussian if soft edge)) and extent (eg specified width/height of source)
     */
    double PointSource::getCoord(const PointSource::SOURCE_DIST l, const double extent) {
        if (l == SD_HARDEDGE) {
            return (m_uniformDist(m_randEngine) - 0.5) * extent;
        }
        else {
            return (m_normDist(m_randEngine) * extent);
        }
    }

    double PointSource::getSourceDepth() const { return m_sourceDepth; }
    double PointSource::getSourceHeight() const { return m_sourceHeight; }
    double PointSource::getSourceWidth() const { return m_sourceWidth; }
    double PointSource::getVerDivergence() const { return m_verDivergence; }
    double PointSource::getHorDivergence() const { return m_horDivergence; }
} // namespace RAYX