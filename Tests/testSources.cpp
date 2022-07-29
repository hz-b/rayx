#include "setupTests.h"

void checkDistribution(const RayList& rays, double sourceEnergy,
                       double energySpread) {
    CHECK_EQ(rays.rayAmount(), 200);
    for (auto r : rays) {
        double min_e = sourceEnergy - energySpread;
        double max_e = sourceEnergy + energySpread;
        if (r.m_energy > max_e || r.m_energy < min_e) {
            RAYX_WARN << "energy out of range: " << r.m_energy;
            RAYX_ERR << "range is " << min_e << " to " << max_e;
        }
    }
}

TEST_F(TestSuite, MatrixSource) {
    auto beamline = loadBeamline("MatrixSource");
    auto a = beamline.getInputRays();
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