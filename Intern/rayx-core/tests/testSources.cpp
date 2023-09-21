#include "setupTests.h"
#include <fstream>

void checkEnergyDistribution(const std::vector<Ray>& rays, float photonEnergy, float energySpread) {
    for (auto r : rays) {
        CHECK_IN(r.m_energy, photonEnergy - energySpread, photonEnergy + energySpread);
    }
}

void checkZDistribution(const std::vector<Ray>& rays, float center, float spread) {
    for (auto r : rays) {
        CHECK_IN(r.m_position.z, center - spread, center + spread);
    }

}

void checkPositionDistribution(const std::vector<Ray>& rays, float sourceWidth, float sourceHight) {
    for (auto r : rays) {
        CHECK_IN(r.m_position[0], -4.5 * sourceWidth, 4.5 * sourceWidth);
        CHECK_IN(r.m_position[1], -4.5 * sourceHight, 4.5 * sourceHight);
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
        r.m_position += glm::vec3(5, -5, 3);
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
    checkEnergyDistribution(rays, 120.97, 12.1);
}

TEST_F(TestSuite, PointSourceSoftEdge) {
    auto rays = loadBeamline("PointSourceSoftEdge").getInputRays();
    checkEnergyDistribution(rays, 151, 6);
}

TEST_F(TestSuite, MatrixSourceEnergyDistribution) {
    auto rays = loadBeamline("MatrixSourceSpreaded").getInputRays();
    checkEnergyDistribution(rays, 42, 10);
}

TEST_F(TestSuite, DipoleSourcePosition) {
    auto rays = loadBeamline("dipole_plain").getInputRays();
    checkPositionDistribution(rays, 0.065, 0.04);
}

TEST_F(TestSuite, DipoleEnergyDistribution) {
    auto rays = loadBeamline("dipole_energySpread").getInputRays();
    checkEnergyDistribution(rays, 1000, 23000);
}

TEST_F(TestSuite, DipoleZDistribution) {
    auto beamline = loadBeamline("dipole_plain");
    std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*src);

    auto rays = beamline.getInputRays();
    checkZDistribution(rays, 0 , 2.2);
}



TEST_F(TestSuite, testInterpolationFunctionDipole) {
    struct InOutPair{
        float in;
        float out;
    };
    std::vector<InOutPair> inouts = {{
                                         .in = 1.5298292375594387,
                                         .out = -3.5010758381905855,
                                     }
    };

    auto beamline = loadBeamline("dipole_plain");
    std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*src);

    for(auto values : inouts){
        auto result = dipolesource->getInterpolation(values.in);
        CHECK_EQ(result, values.out, 0.01);
    }
}

TEST_F(TestSuite, testVerDivergenceDipole) {
    struct InOutPair{
        float energy;
        float sigv;
        float out;
    };
    std::vector<InOutPair> inouts = {{
                                         .energy = 100,
                                         .sigv = 1,
                                         .out = 1.591581814000419,
                                     }
    };

    auto beamline = loadBeamline("dipole_plain");
    std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*src);

    for(auto values : inouts){
        auto result = dipolesource->vDivergence(values.energy, values.sigv);
        CHECK_EQ(result, values.out, 0.1);
    }
}

TEST_F(TestSuite, testLightsourceGetters){
    struct RmlInput{
        std::string rmlFile;
        float horDivergence;
        float sourceHight;
        float sourceWidth;
        float sourceDepth;
        float averagePhotonEnergy;
    };
    
    std::vector<RmlInput> rmlinputs = {{
                                        .rmlFile = "PointSourceHardEdge",
                                        .horDivergence = 0.001,             //conversion /1000 in the parser
                                        .sourceHight = 0.04,
                                        .sourceWidth = 0.065,
                                        .sourceDepth = 1,
                                        .averagePhotonEnergy = 120.97,
                                       }
    };
    
    for(auto values : rmlinputs){
        auto beamline = loadBeamline(values.rmlFile);
        std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
        LightSource* lightsource = dynamic_cast<LightSource*>(&*src);

        auto horResult = lightsource->getHorDivergence();
        auto hightResult = lightsource->getSourceHeight();
        auto widthResult = lightsource->getSourceWidth();
        auto average = lightsource->getPhotonEnergy();

        CHECK_EQ(horResult, values.horDivergence);
        CHECK_EQ(hightResult, values.sourceHight);
        CHECK_EQ(widthResult, values.sourceWidth);
        CHECK_EQ(average, values.averagePhotonEnergy);
    }
}
