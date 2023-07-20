#include "setupTests.h"
#include <fstream>

void checkDistribution(const std::vector<Ray>& rays, double sourceEnergy, double energySpread) {
    for (auto r : rays) {
        CHECK_IN(r.m_energy, sourceEnergy - energySpread, sourceEnergy + energySpread);
    }
}

void checkPositionDistribution(const std::vector<Ray>& rays, double sourceWidth, double sourceHight) {
    for (auto r : rays) {
        CHECK_IN(r.m_position[0], -4.5 * sourceWidth, 4.5 * sourceWidth);
        CHECK_IN(r.m_position[1], -4.5 * sourceHight, 4.5 * sourceHight);
    }
}

void checkZDistribution(const std::vector<Ray>& rays) {
    for (auto r : rays) {
        CHECK_EQ(r.m_position.z, float(0));
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

TEST_F(TestSuite, DipoleSourcePosition) {
    auto rays = loadBeamline("dipole_plain").getInputRays();
    checkPositionDistribution(rays, 0.065, 0.04);
}

TEST_F(TestSuite, DipoleEnergyDistribution) {
    auto rays = loadBeamline("dipole_energySpread").getInputRays();
    checkDistribution(rays, 1000, 23000);
}

/*TEST_F(TestSuite, DipoleZDistribution) {
    auto rays = loadBeamline("dipole_imageplane").getInputRays();
    checkZDistribution(rays);
}*/