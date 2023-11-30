#include "setupTests.h"

TEST_F(TestSuite, allBeamlineObjects) {
    auto b = loadBeamline("allBeamlineObjects");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(),
             8);  // plane mirror, toroid, slit, sphere grating, plane grating,
                  // sphere mirror, rzp, image plane
}

TEST_F(TestSuite, loadDatFile) {
    RAYX::fixSeed(RAYX::FIXED_SEED);

    auto b = loadBeamline("loadDatFile");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.getAverage(), (12. + 15. + 17.) / 3, 0.1);
    // CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.selectEnergy(), 17, 0.1); // TODO Fanny check why it failes depending on the compiler. gcc
    // calculates 15, clang 17
}

TEST_F(TestSuite, loadDatFile2) {
    RAYX::fixSeed(RAYX::FIXED_SEED);

    auto b = loadBeamline("loadDatFile2");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    // CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.getAverage(), 14.6, 0.1); //TODO value needs to be confirmed, check why it failes depending
    // on the compiler. CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.selectEnergy(), 17.1, 0.1); //TODO value needs to be confirmed, check why
    // it failes depending on the compiler.
}

TEST_F(TestSuite, loadGroups) {
    auto b = loadBeamline("loadGroups");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 4);
}

TEST_F(TestSuite, groupTransform) {
    auto b = loadBeamline("groupTransform");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    auto m = b.m_OpticalElements[0].m_element.m_inTrans;
    glm::dmat4x4 correct = {
        1,   0,  0,     0,  //
        0,   1,  0,     0,  //
        0,   0, -1,     0,  //
        -42, -1000,  0, 1,  //
    };
    CHECK_EQ(correct, m);
}

TEST_F(TestSuite, testEnergyDistribution) {
    RAYX::fixSeed(RAYX::FIXED_SEED);

    struct testInput {
        std::string rmlFile;
        double energy;
        double average;
    };

    std::vector<testInput> testinput = {
        {
            .rmlFile = "PointSourceSeperateEnergies",
            .energy = 100,
            .average = 100,
        },
        {
            .rmlFile = "PointSourceSoftEdgeEnergy",
            .energy = 106.42,
            .average = 100,
        },
        {
            .rmlFile = "PointSourceThreeSoftEdgeEnergies",
            .energy = 47.92,
            .average = 50,
        },
        {
            .rmlFile = "PointSourceHardEdgeEnergy",
            .energy = 127.96,
            .average = 100,
        },
    };

    for (auto values : testinput) {
        auto beamline = loadBeamline(values.rmlFile);
        auto energy = beamline.m_LightSources[0]->m_EnergyDistribution.selectEnergy();
        auto average = beamline.m_LightSources[0]->m_EnergyDistribution.getAverage();

        CHECK_EQ(energy, values.energy, 0.1);
        CHECK_EQ(average, values.average, 0.1);
    }
}

TEST_F(TestSuite, testParaboloidQuad) {
    auto beamline = loadBeamline("paraboloid_matrix_IP");

    Element para = beamline.m_OpticalElements[0].m_element;
    auto parabo = deserializeQuadric(para.m_surface);

    CHECK_EQ(1, parabo.m_a11);
    CHECK_EQ(0, parabo.m_a12);
    CHECK_EQ(0, parabo.m_a13);
    CHECK_EQ(0, parabo.m_a14);
    CHECK_EQ(1, parabo.m_a22);
    CHECK_EQ(0, parabo.m_a23);
    CHECK_EQ(-3765.641, parabo.m_a24, 0.001);
    CHECK_EQ(0, parabo.m_a33);
    CHECK_EQ(663.984, parabo.m_a34, 0.001);
    CHECK_EQ(-2.3283e-10, parabo.m_a44, 0.001);
    CHECK_EQ(1, parabo.m_icurv);
}

TEST_F(TestSuite, testSphereQuad) {
    auto beamline = loadBeamline("SphereMirrorDefault");
    Element sph = beamline.m_OpticalElements[0].m_element;
    auto sphere = deserializeQuadric(sph.m_surface);

    CHECK_EQ(1, sphere.m_a11);
    CHECK_EQ(0, sphere.m_a12);
    CHECK_EQ(0, sphere.m_a13);
    CHECK_EQ(0, sphere.m_a14);
    CHECK_EQ(1, sphere.m_a22);
    CHECK_EQ(0, sphere.m_a23);
    CHECK_EQ(-10470.4917875, sphere.m_a24, 1e-6);
    CHECK_EQ(1, sphere.m_a33);
    CHECK_EQ(0, sphere.m_a34);
    CHECK_EQ(0, sphere.m_a44);
    CHECK_EQ(1, sphere.m_icurv);
}

TEST_F(TestSuite, testEllipsoidQuad) {
    auto beamline = loadBeamline("Ellipsoid");
    Element elli = beamline.m_OpticalElements[0].m_element;
    auto ellips = deserializeQuadric(elli.m_surface);

    CHECK_EQ(1, ellips.m_a11);
    CHECK_EQ(0, ellips.m_a12);
    CHECK_EQ(0, ellips.m_a13);
    CHECK_EQ(0, ellips.m_a14);
    CHECK_EQ(0.9798144, ellips.m_a22, 0.001);
    CHECK_EQ(0.1399173, ellips.m_a23, 0.001);
    CHECK_EQ(-315.7239, ellips.m_a24, 0.001);
    CHECK_EQ(0.0301536, ellips.m_a33, 0.001);
    CHECK_EQ(0, ellips.m_a34);
    CHECK_EQ(0, ellips.m_a44);
    CHECK_EQ(1, ellips.m_icurv);
}

TEST_F(TestSuite, testCylinderQuad) {
    auto beamline = loadBeamline("CylinderDefault");
    Element cyli = beamline.m_OpticalElements[0].m_element;
    auto cylinder = deserializeQuadric(cyli.m_surface);

    CHECK_EQ(0, cylinder.m_a11);
    CHECK_EQ(0, cylinder.m_a12);
    CHECK_EQ(0, cylinder.m_a13);
    CHECK_EQ(0, cylinder.m_a14);
    CHECK_EQ(1, cylinder.m_a22);
    CHECK_EQ(0, cylinder.m_a23);
    CHECK_EQ(-10470.4917, cylinder.m_a24, 0.001);
    CHECK_EQ(1, cylinder.m_a33);
    CHECK_EQ(0, cylinder.m_a34);
    CHECK_EQ(0, cylinder.m_a44);
    CHECK_EQ(1, cylinder.m_icurv);
}

TEST_F(TestSuite, testConeQuad) {
    auto beamline = loadBeamline("Cone");
    Element con = beamline.m_OpticalElements[0].m_element;
    auto cone = deserializeQuadric(con.m_surface);

    CHECK_EQ(0.903353, cone.m_a11, 0.001);
    CHECK_EQ(0, cone.m_a12);
    CHECK_EQ(0, cone.m_a13);
    CHECK_EQ(0, cone.m_a14);
    CHECK_EQ(0.806707, cone.m_a22, 0.001);
    CHECK_EQ(0.295475, cone.m_a23, 0.001);
    CHECK_EQ(-300.120, cone.m_a24, 0.001);
    CHECK_EQ(0, cone.m_a33);
    CHECK_EQ(0, cone.m_a34);
    CHECK_EQ(0, cone.m_a44);
    CHECK_EQ(1, cone.m_icurv);
}

TEST_F(TestSuite, testToroidSurface) {
    auto beamline = loadBeamline("toroid");
    Element trid = beamline.m_OpticalElements[0].m_element;
    auto toroid = deserializeToroid(trid.m_surface);

    CHECK_EQ(10470.4917, toroid.m_longRadius, 0.001);
    CHECK_EQ(315.723959, toroid.m_shortRadius, 0.001);
    CHECK_EQ(1, toroid.m_toroidType);
}


TEST_F(TestSuite, testExpertsOptic) {
    auto beamline = loadBeamline("toroid");
    Element trid = beamline.m_OpticalElements[0].m_element;
    auto toroid = deserializeToroid(trid.m_surface);

    CHECK_EQ(10470.4917, toroid.m_longRadius, 0.001);
    CHECK_EQ(315.723959, toroid.m_shortRadius, 0.001);
    CHECK_EQ(1, toroid.m_toroidType);
}


/***
 * Tests if two sources can be traced in one go.
 * Its a static test, so every change can result in a fail even if it's still working correctly
 */
TEST_F(TestSuite, testTwoSourcesInOneRML) {
    RAYX::fixSeed(RAYX::FIXED_SEED);

    auto beamline = loadBeamline("twoSourcesTest");

    std::shared_ptr<LightSource> dsrc = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*dsrc);

    std::shared_ptr<LightSource> psrc = beamline.m_LightSources[1];
    PointSource* pointsource = dynamic_cast<PointSource*>(&*psrc);

    CHECK_EQ(100, dipolesource->getEnergy());
    CHECK_EQ(148.8, pointsource->selectEnergy(), 0.1);

    RAYX::fixSeed(RAYX::FIXED_SEED);
    CHECK_EQ(-21.74, dipolesource->getXYZPosition(0.1).x, 0.1);
    CHECK_EQ(0, pointsource->getSourceWidth(), 0.1);
}

// TODO(rudi) re-enable group tests

// TEST_F(TestSuite, groupTransform2) {
//     auto b = loadBeamline("groupTransform2");
//     CHECK_EQ(b.m_LightSources.size(), 1);
//     CHECK_EQ(b.m_OpticalElements.size(), 1);
//     glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0, 0.174, 0.985, 0, 0, 0, 0, 1);
//     glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);
//     glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
//     glm::dmat4x4 elementOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.996, -0.087, 0, 0, 0.087, 0.996, 0, 0, 0, 0, 1);

//     glm::dmat4x4 orientationCorrect = groupOr * elementOr;
//     glm::dvec4 positionCorrect = groupPos + (groupOr * elementPos);

//     glm::dmat4x4 orientationResult = b.m_OpticalElements[0].m_element.m_inTrans;
//     glm::dvec4 positionResult = b.m_OpticalElements[0].m_element.m_inTrans * glm::dvec4(0, 0, 0, 1);

//     CHECK_EQ((orientationCorrect), (orientationResult));
//     CHECK_EQ((positionCorrect), (positionResult));
// }

/*
TEST_F(TestSuite, groupTransformMisalignment) {
    auto b = loadBeamline("groupTransformMisalignment");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);

    glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0, 0.174, 0.985, 0, 0, 0, 0, 1);
    printDMat4(groupOr);

    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);

    glm::dvec4 positionCorrect = groupPos + groupOr * elementPos;
    glm::dmat4x4 orientationResult = b.m_OpticalElements[0]->getOrientation();
    printDMat4(orientationResult);
    glm::dvec4 positionResult = b.m_OpticalElements[0]->getPosition();

    CHECK_EQ(positionCorrect, positionResult);
    CHECK_EQ(groupOr, orientationResult, 1e-15);
}
*/
