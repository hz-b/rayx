#include "setupTests.h"

void checkDistribution(const std::vector<Ray>& rays, double sourceEnergy,
                       double energySpread) {
    CHECK_EQ(rays.size(), 200);
    for (auto r : rays) {
        CHECK_IN(r.m_energy, sourceEnergy - energySpread,
                 sourceEnergy + energySpread);
    }
}

TEST_F(TestSuite, MatrixSource) {
    auto beamline = loadBeamline("MatrixSource");
    auto a = beamline.getInputRays();
    auto b = loadCSVRayUI("MatrixSource");
    compareRayLists(a, b);
}

// this tests tracing an only-lightsource beamline. An error-prone edge case.
TEST_F(TestSuite, MatrixSourceTraced) {
    auto a = traceRML("MatrixSource");
    auto b = loadCSVRayUI("MatrixSource");
    compareRayLists(a, b);
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