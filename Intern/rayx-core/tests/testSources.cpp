#include "setupTests.h"

void checkDistribution(const std::vector<Ray>& rays, double sourceEnergy, double energySpread) {
    CHECK_EQ(rays.size(), 200);
    for (auto r : rays) {
        CHECK_IN(r.m_energy, sourceEnergy - energySpread, sourceEnergy + energySpread);
    }
}

void roughCompare(std::vector<RAYX::Ray> l, std::vector<RAYX::Ray> r) {
    CHECK_EQ(l.size(), r.size());
    // TODO maybe compare more?
    for (int i = 0; i < l.size(); i++) {
        CHECK_EQ(l[i].m_position, r[i].m_position);
        CHECK_EQ(l[i].m_direction, r[i].m_direction);
        CHECK_EQ(l[i].m_energy, r[i].m_energy);
    }
}

TEST_F(TestSuite, MatrixSource) {
    auto beamline = loadBeamline("MatrixSource");
    auto a = beamline.getInputRays();
    auto b = loadCSVRayUI("MatrixSource");
    roughCompare(a, b);
}

TEST_F(TestSuite, MatrixSourceMoved) {
    auto beamline = loadBeamline("MatrixSourceMoved");
    auto a = beamline.getInputRays();
    auto b = loadCSVRayUI("MatrixSource");
    for (auto& r : b) {
        r.m_position += glm::dvec3(5, -5, 3);
    }
    roughCompare(a, b);
}

/// this tests tracing an only-lightsource beamline. An error-prone edge case.
TEST_F(TestSuite, MatrixSourceTraced) {
    auto a = extractLastEvents(traceRML("MatrixSource"));
    auto b = loadCSVRayUI("MatrixSource");
    roughCompare(a, b);
}

TEST_F(TestSuite, PointSourceHardEdge) {
    auto rays = loadBeamline("PointSourceHardEdge").getInputRays();
    checkDistribution(rays, 120.97, 12.1);
}

TEST_F(TestSuite, PointSourceSoftEdge) {
    auto rays = loadBeamline("PointSourceSoftEdge").getInputRays();
    checkDistribution(rays, 151, 6);
}

TEST_F(TestSuite, MatrixSourceEnergyDistribution) {
    auto rays = loadBeamline("MatrixSourceSpreaded").getInputRays();
    checkDistribution(rays, 42, 10);
}

TEST_F(TestSuite, LightSourceGetters) { //Check for correctness of values
    auto beamline = loadBeamline("PointSourceHardEdge");
    auto horDiv = beamline.m_LightSources[0]->getHorDivergence();
    auto verDiv = beamline.m_LightSources[0]->getVerDivergence();
    auto sourceDepth = beamline.m_LightSources[0]->getSourceDepth();
    auto sourceHeight = beamline.m_LightSources[0]->getSourceHeight();
    auto sourceWidth = beamline.m_LightSources[0]->getSourceWidth();
    CHECK_EQ(horDiv, 0.001);
    CHECK_EQ(verDiv, 0.001);
    CHECK_EQ(sourceDepth, 1);
    CHECK_EQ(sourceHeight, 0.04);
    CHECK_EQ(sourceWidth, 0.065);
    }