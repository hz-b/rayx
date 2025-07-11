#include "DesignSource.h"

#include <filesystem>
#include <memory>

#include "Beamline/Objects/Objects.h"
#include "Debug/Debug.h"
namespace RAYX {

DesignSource::DesignSource(DesignSource&& other) noexcept { m_elementParameters = std::move(other.m_elementParameters); }

DesignSource& DesignSource::operator=(DesignSource&& other) noexcept {
    m_elementParameters = std::move(other.m_elementParameters);
    return *this;
}

std::unique_ptr<BeamlineNode> DesignSource::clone() const {
    DesignSource clone;
    clone.m_elementParameters = m_elementParameters.clone();
    return std::make_unique<DesignSource>(std::move(clone));
}

std::vector<Ray> DesignSource::compile(int numThreads, const glm::dvec4& groupPosition, const glm::dmat4& groupOrientation) const {
    // Apply group transformations
    glm::dvec4 position = groupOrientation * getPosition() + groupPosition;
    glm::dmat4 orientation = groupOrientation * getOrientation();

    // Create a temporary copy with world instead of local pos/ori
    std::unique_ptr<BeamlineNode> ds = this->clone();
    DesignSource* dsPtr = static_cast<DesignSource*>(ds.get());
    dsPtr->setPosition(position);
    dsPtr->setOrientation(orientation);

    switch (dsPtr->getType()) {
        case ElementType::PointSource:
            return PointSource(*dsPtr).getRays(numThreads);
        case ElementType::MatrixSource:
            return MatrixSource(*dsPtr).getRays(numThreads);
        case ElementType::DipoleSource:
            return DipoleSource(*dsPtr).getRays(numThreads);
        case ElementType::DipoleSrc:
            return DipoleSource(*dsPtr).getRays(numThreads);
        case ElementType::PixelSource:
            return PixelSource(*dsPtr).getRays(numThreads);
        case ElementType::CircleSource:
            return CircleSource(*dsPtr).getRays(numThreads);
        case ElementType::SimpleUndulatorSource:
            return SimpleUndulatorSource(*dsPtr).getRays(numThreads);
        default:
            throw std::runtime_error("Unknown source type");
    }
}

void DesignSource::setName(std::string s) { m_elementParameters["name"] = s; }
void DesignSource::setType(ElementType s) { m_elementParameters["type"] = s; }

std::string DesignSource::getName() const { return m_elementParameters["name"].as_string(); }
ElementType DesignSource::getType() const { return m_elementParameters["type"].as_elementType(); }

void DesignSource::setPosition(glm::dvec4 p) {
    m_elementParameters["position"] = Map();
    m_elementParameters["position"]["x"] = p.x;
    m_elementParameters["position"]["y"] = p.y;
    m_elementParameters["position"]["z"] = p.z;
    m_elementParameters["position"]["w"] = p.w;
}

glm::dvec4 DesignSource::getPosition() const {
    glm::dvec4 d;
    d[0] = m_elementParameters["position"]["x"].as_double();
    d[1] = m_elementParameters["position"]["y"].as_double();
    d[2] = m_elementParameters["position"]["z"].as_double();
    d[3] = 1;
    return d;
}

void DesignSource::setOrientation(glm::dmat4x4 orientation) {
    m_elementParameters["xDirection"] = Map();
    m_elementParameters["xDirection"]["x"] = orientation[0][0];
    m_elementParameters["xDirection"]["y"] = orientation[0][1];
    m_elementParameters["xDirection"]["z"] = orientation[0][2];
    m_elementParameters["xDirection"]["w"] = orientation[0][3];

    m_elementParameters["yDirection"] = Map();
    m_elementParameters["yDirection"]["x"] = orientation[1][0];
    m_elementParameters["yDirection"]["y"] = orientation[1][1];
    m_elementParameters["yDirection"]["z"] = orientation[1][2];
    m_elementParameters["yDirection"]["w"] = orientation[1][3];

    m_elementParameters["zDirection"] = Map();
    m_elementParameters["zDirection"]["x"] = orientation[2][0];
    m_elementParameters["zDirection"]["y"] = orientation[2][1];
    m_elementParameters["zDirection"]["z"] = orientation[2][2];
    m_elementParameters["zDirection"]["w"] = orientation[2][3];
}

glm::dmat4x4 DesignSource::getOrientation() const {
    glm::dmat4x4 orientation;

    orientation[0][0] = m_elementParameters["xDirection"]["x"].as_double();
    orientation[0][1] = m_elementParameters["xDirection"]["y"].as_double();
    orientation[0][2] = m_elementParameters["xDirection"]["z"].as_double();
    orientation[0][3] = 0;

    orientation[1][0] = m_elementParameters["yDirection"]["x"].as_double();
    orientation[1][1] = m_elementParameters["yDirection"]["y"].as_double();
    orientation[1][2] = m_elementParameters["yDirection"]["z"].as_double();
    orientation[1][3] = 0;

    orientation[2][0] = m_elementParameters["zDirection"]["x"].as_double();
    orientation[2][1] = m_elementParameters["zDirection"]["y"].as_double();
    orientation[2][2] = m_elementParameters["zDirection"]["z"].as_double();
    orientation[2][3] = 0;

    orientation[3][0] = 0;
    orientation[3][1] = 0;
    orientation[3][2] = 0;
    orientation[3][3] = 1;

    return orientation;
}

void DesignSource::setMisalignment(Misalignment m) {
    m_elementParameters["rotationXerror"] = m.m_rotationXerror.rad;
    m_elementParameters["rotationYerror"] = m.m_rotationYerror.rad;
    m_elementParameters["rotationZerror"] = m.m_rotationZerror.rad;

    m_elementParameters["translationXerror"] = m.m_translationXerror;
    m_elementParameters["translationYerror"] = m.m_translationYerror;
    m_elementParameters["translationZerror"] = m.m_translationZerror;
}

Misalignment DesignSource::getMisalignment() const {
    Misalignment m;

    m.m_rotationXerror.rad = m_elementParameters["rotationXerror"].as_double();
    m.m_rotationYerror.rad = m_elementParameters["rotationYerror"].as_double();
    m.m_rotationZerror.rad = m_elementParameters["rotationZerror"].as_double();

    m.m_translationXerror = m_elementParameters["translationXerror"].as_double();
    m.m_translationYerror = m_elementParameters["translationYerror"].as_double();
    m.m_translationZerror = m_elementParameters["translationZerror"].as_double();

    return m;
}

void DesignSource::setStokeslin0(double value) {
    m_elementParameters["stokes"] = Map();
    m_elementParameters["stokes"]["linPol0"] = value;
}

void DesignSource::setStokeslin45(double value) { m_elementParameters["stokes"]["linPol45"] = value; }

void DesignSource::setStokescirc(double value) { m_elementParameters["stokes"]["circPol"] = value; }

glm::dvec4 DesignSource::getStokes() const {
    glm::dvec4 pol;
    pol[0] = 1;
    pol[1] = m_elementParameters["stokes"]["linPol0"].as_double();
    pol[2] = m_elementParameters["stokes"]["linPol45"].as_double();
    pol[3] = m_elementParameters["stokes"]["circPol"].as_double();
    return pol;
}

void DesignSource::setWidthDist(SourceDist value) { m_elementParameters["widthDist"] = value; }
SourceDist DesignSource::getWidthDist() const { return m_elementParameters["widthDist"].as_sourceDist(); }

void DesignSource::setHeightDist(SourceDist value) { m_elementParameters["heightDist"] = value; }
SourceDist DesignSource::getHeightDist() const { return m_elementParameters["heightDist"].as_sourceDist(); }

void DesignSource::setHorDist(SourceDist value) { m_elementParameters["horDist"] = value; }
SourceDist DesignSource::getHorDist() const { return m_elementParameters["horDist"].as_sourceDist(); }

void DesignSource::setVerDist(SourceDist value) { m_elementParameters["verDist"] = value; }
SourceDist DesignSource::getVerDist() const { return m_elementParameters["verDist"].as_sourceDist(); }

void DesignSource::setHorDivergence(double value) { m_elementParameters["horDivergence"] = value; }
double DesignSource::getHorDivergence() const { return m_elementParameters["horDivergence"].as_double(); }

void DesignSource::setVerDivergence(double value) { m_elementParameters["verDivergence"] = value; }
double DesignSource::getVerDivergence() const { return m_elementParameters["verDivergence"].as_double(); }

void DesignSource::setVerEBeamDivergence(double value) { m_elementParameters["verEBeamDivergence"] = value; }
double DesignSource::getVerEBeamDivergence() const { return m_elementParameters["verEBeamDivergence"].as_double(); }

void DesignSource::setSourceDepth(double value) { m_elementParameters["sourceDepth"] = value; }
double DesignSource::getSourceDepth() const { return m_elementParameters["sourceDepth"].as_double(); }

void DesignSource::setSourceHeight(double value) { m_elementParameters["sourceHeight"] = value; }
double DesignSource::getSourceHeight() const { return m_elementParameters["sourceHeight"].as_double(); }

void DesignSource::setSourceWidth(double value) { m_elementParameters["sourceWidth"] = value; }
double DesignSource::getSourceWidth() const { return m_elementParameters["sourceWidth"].as_double(); }

void DesignSource::setBendingRadius(double value) { m_elementParameters["bendingRadius"] = value; }
double DesignSource::getBendingRadius() const { return m_elementParameters["bendingRadius"].as_double(); }

void DesignSource::setEnergy(double value) { m_elementParameters["energy"] = value; }
double DesignSource::getEnergy() const { return m_elementParameters["energy"].as_double(); }

void DesignSource::setElectronEnergy(double value) { m_elementParameters["electronEnergy"] = value; }
double DesignSource::getElectronEnergy() const { return m_elementParameters["electronEnergy"].as_double(); }

void DesignSource::setElectronEnergyOrientation(ElectronEnergyOrientation value) { m_elementParameters["electronEnergyOrientation"] = value; }
ElectronEnergyOrientation DesignSource::getElectronEnergyOrientation() const {
    return m_elementParameters["electronEnergyOrientation"].as_electronEnergyOrientation();
}

void DesignSource::setEnergySpread(double value) { m_elementParameters["energySpread"] = value; }
double DesignSource::getEnergySpread() const { return m_elementParameters["energySpread"].as_double(); }

void DesignSource::setEnergySpreadUnit(EnergySpreadUnit value) { m_elementParameters["energySpreadUnit"] = value; }
EnergySpreadUnit DesignSource::getEnergySpreadUnit() const { return m_elementParameters["energySpreadUnit"].as_energySpreadUnit(); }

void DesignSource::setEnergyDistributionType(EnergyDistributionType value) { m_elementParameters["energyDistributionType"] = value; }
EnergyDistributionType DesignSource::getEnergyDistributionType() const {
    return m_elementParameters["energyDistributionType"].as_energyDistributionType();
}

void DesignSource::setEnergyDistributionFile(std::string value) { m_elementParameters["photonEnergyDistributionFile"] = value; }

void DesignSource::setEnergySpreadType(SpreadType value) { m_elementParameters["energyDistribution"] = value; }
SpreadType DesignSource::getEnergySpreadType() const { return m_elementParameters["energyDistribution"].as_energySpreadType(); }

void DesignSource::setSeparateEnergies(int value) { m_elementParameters["SeparateEnergies"] = value; }

void DesignSource::setPhotonFlux(double value) { m_elementParameters["photonFlux"] = value; }
double DesignSource::getPhotonFlux() const { return m_elementParameters["photonFlux"].as_double(); }

EnergyDistribution DesignSource::getEnergyDistribution() const {
    EnergyDistribution en;
    SpreadType spreadType = m_elementParameters["energyDistribution"].as_energySpreadType();
    EnergyDistributionType energyDistributionType = m_elementParameters["energyDistributionType"].as_energyDistributionType();

    if (energyDistributionType == EnergyDistributionType::File) {
        std::string filename = m_elementParameters["photonEnergyDistributionFile"].as_string();

        DatFile df;
        DatFile::load(filename, &df);

        df.m_continuous = (spreadType == SpreadType::SoftEdge ? true : false);
        en = EnergyDistribution(df);

    } else if (energyDistributionType == EnergyDistributionType::Values) {
        double photonEnergy = m_elementParameters["energy"].as_double();
        double energySpread = m_elementParameters["energySpread"].as_double();

        if (spreadType == SpreadType::SoftEdge) {
            if (energySpread == 0) {
                energySpread = 1;
            }
            en = EnergyDistribution(SoftEdge(photonEnergy, energySpread));

        } else if (spreadType == SpreadType::SeparateEnergies) {
            int numOfEnergies;
            if (!m_elementParameters["SeparateEnergies"].as_int()) {
                numOfEnergies = 3;
            } else {
                numOfEnergies = m_elementParameters["SeparateEnergies"].as_int();
            }
            numOfEnergies = abs(numOfEnergies);
            en = EnergyDistribution(SeparateEnergies(photonEnergy, energySpread, numOfEnergies));
        } else {
            en = EnergyDistribution(HardEdge(photonEnergy, energySpread));
        }

    } else {
        RAYX_EXIT << "paramEnergyDistribution is not implemented for "
                     "energyDistributionType"
                  << static_cast<int>(energyDistributionType) << "!";
    }
    return en;
}

void DesignSource::setNumberOfRays(double value) { m_elementParameters["numberOfRays"] = value; }
double DesignSource::getNumberOfRays() const { return m_elementParameters["numberOfRays"].as_double(); }

void DesignSource::setNumOfCircles(int value) { m_elementParameters["numOfCircles"] = value; }

int DesignSource::getNumOfCircles() const { return m_elementParameters["numOfCircles"].as_int(); }

void DesignSource::setMaxOpeningAngle(Rad value) { m_elementParameters["maxOpeningAngle"] = value; }

Rad DesignSource::getMaxOpeningAngle() const { return m_elementParameters["maxOpeningAngle"].as_rad(); }

void DesignSource::setMinOpeningAngle(Rad value) { m_elementParameters["minOpeningAngle"] = value; }

Rad DesignSource::getMinOpeningAngle() const { return m_elementParameters["minOpeningAngle"].as_rad(); }

void DesignSource::setDeltaOpeningAngle(Rad value) { m_elementParameters["deltaOpeningAngle"] = value; }

Rad DesignSource::getDeltaOpeningAngle() const { return m_elementParameters["deltaOpeningAngle"].as_rad(); }

void DesignSource::setSigmaType(SigmaType value) { m_elementParameters["sigmaType"] = value; }

SigmaType DesignSource::getSigmaType() const { return m_elementParameters["sigmaType"].as_sigmaType(); }

void DesignSource::setUndulatorLength(double value) { m_elementParameters["undulatorLength"] = value; }

double DesignSource::getUndulatorLength() const { return m_elementParameters["undulatorLength"].as_double(); }

void DesignSource::setElectronSigmaX(double value) { m_elementParameters["electronSigmaX"] = value; }

double DesignSource::getElectronSigmaX() const { return m_elementParameters["electronSigmaX"].as_double(); }

void DesignSource::setElectronSigmaXs(double value) { m_elementParameters["electronSigmaXs"] = value; }

double DesignSource::getElectronSigmaXs() const { return m_elementParameters["electronSigmaXs"].as_double(); }

void DesignSource::setElectronSigmaY(double value) { m_elementParameters["electronSigmaY"] = value; }

double DesignSource::getElectronSigmaY() const { return m_elementParameters["electronSigmaY"].as_double(); }

void DesignSource::setElectronSigmaYs(double value) { m_elementParameters["electronSigmaYs"] = value; }

double DesignSource::getElectronSigmaYs() const { return m_elementParameters["electronSigmaYs"].as_double(); }

}  // namespace RAYX
