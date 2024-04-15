#include "setupTests.h"

TEST_F(TestSuite, allBeamlineObjects) {
    auto b = loadBeamline("allBeamlineObjects");
    CHECK_EQ(b.m_DesignSources.size(), 6); // Point, Circle, Dipole, Matrix, Pixel, simple Undulator
    CHECK_EQ(b.m_DesignElements.size(),
             12);  // Cone, Cylinder, Ellipsoid, Paraboloid, plane mirror, toroid, slit, sphere grating, plane grating,
                  // sphere mirror, rzp, image plane
}

TEST_F(TestSuite, loadDatFile) {
    auto b = loadBeamline("loadDatFile");
    CHECK_EQ(b.m_DesignSources.size(), 1);
    CHECK_EQ(b.m_DesignElements.size(), 1);

    // This only works due to fixed seeding!
    // The loaded DAT file only has the 3 energies 12, 15, 17 with equal probability.
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 15, 0.1);
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 17, 0.1);
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 17, 0.1);
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 12, 0.1);
}

TEST_F(TestSuite, loadDatFile2) {
    auto b = loadBeamline("loadDatFile2");
    CHECK_EQ(b.m_DesignSources.size(), 1);
    CHECK_EQ(b.m_DesignElements.size(), 1);

    // This only works due to fixed seeding!
    // The loaded DAT file only has the 3 energies 12, 15, 17 with - but it uses soft band.
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 14.7, 0.1);
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 17.1, 0.1);
    CHECK_EQ(b.m_DesignSources[0].getEnergyDistribution().selectEnergy(), 16.7, 0.1);
}

TEST_F(TestSuite, loadGroups) {
    auto b = loadBeamline("loadGroups");
    CHECK_EQ(b.m_DesignSources.size(), 1);
    CHECK_EQ(b.m_DesignElements.size(), 4);
}

TEST_F(TestSuite, groupTransform) {
    auto b = loadBeamline("groupTransform");
    CHECK_EQ(b.m_DesignSources.size(), 1);
    CHECK_EQ(b.m_DesignElements.size(), 1);
    auto m = b.m_DesignElements[0].compile().m_inTrans;
    glm::dmat4x4 correct = {
        1,   0,  0,     0,  //
        0,   1,  0,     0,  //
        0,   0, -1,     0,  //
        -42, -1000,  0, 1,  //
    };
    CHECK_EQ(correct, m);
}

TEST_F(TestSuite, testEnergyDistribution) {
    struct testInput {
        std::string rmlFile;
        double energy;
    };
    
    std::vector<testInput> testinput = {
        {
            .rmlFile = "PointSourceSeperateEnergies",
            .energy = 100,
        },
        {
            .rmlFile = "PointSourceSoftEdgeEnergy",
            .energy = 104.042,
        },
        {
            .rmlFile = "PointSourceThreeSoftEdgeEnergies",
            .energy = 51.29,
        },
        {
            .rmlFile = "PointSourceHardEdgeEnergy",
            .energy = 123.19,
        },
    };

    for (auto values : testinput) {
        auto beamline = loadBeamline(values.rmlFile);
        auto energy = beamline.m_DesignSources[0].getEnergyDistribution().selectEnergy();

        CHECK_EQ(energy, values.energy, 0.1);
    }
}

TEST_F(TestSuite, testParaboloidQuad) {
    auto beamline = loadBeamline("paraboloid_matrix_IP");

    Element para = beamline.m_DesignElements[0].compile();
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
    Element sph = beamline.m_DesignElements[0].compile();
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
    Element elli = beamline.m_DesignElements[0].compile();
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
    Element cyli = beamline.m_DesignElements[0].compile();
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
    Element con = beamline.m_DesignElements[0].compile();
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
    Element trid = beamline.m_DesignElements[0].compile();
    auto toroid = deserializeToroid(trid.m_surface);

    CHECK_EQ(10470.4917, toroid.m_longRadius, 0.001);
    CHECK_EQ(315.723959, toroid.m_shortRadius, 0.001);
    CHECK_EQ(1, toroid.m_toroidType);
}


TEST_F(TestSuite, testExpertsOptic) {
    auto beamline = loadBeamline("toroid");
    Element trid = beamline.m_DesignElements[0].compile();
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
    auto beamline = loadBeamline("twoSourcesTest");

    DesignSource dipolesource = beamline.m_DesignSources[0];

    DesignSource pointsource = beamline.m_DesignSources[1];

    CHECK_EQ(100, dipolesource.getEnergy());
    CHECK_EQ(150.24724068638105, pointsource.getEnergyDistribution().selectEnergy());

    RAYX::fixSeed(RAYX::FIXED_SEED);
    //CHECK_EQ(-21.74, dipolesource->getXYZPosition(0.1).x, 0.1);
    CHECK_EQ(0, pointsource.getSourceWidth(), 0.1);
}

TEST_F(TestSuite, groupTransform2) {
    auto b = loadBeamline("groupTransform2");
    CHECK_EQ(b.m_DesignSources.size(), 1);
    CHECK_EQ(b.m_DesignElements.size(), 1);

    glm::dmat4x4 yz_swap = {
        1, 0, 0, 0,
        0, 0, 1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1,
    };

    glm::dmat4x4 groupOr = glm::dmat4x4( //
        1, 0, 0, 0,          //
        0, 0.985, -0.174, 0, //
        0, 0.174, 0.985, 0,  //
        0, 0, 0, 1);         //
    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);

    glm::dmat4x4 elementOr = glm::dmat4x4( //
        1, 0, 0, 0,  //
        0, 1, 0, 0,  //
        0, 0, 1, 0,  //
        0, 0, 0, 1); //
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);

    glm::dmat4x4 orientationCorrect = groupOr * elementOr;
    glm::dvec4 positionCorrect = groupPos + (groupOr * elementPos);

    glm::dmat4x4 inTrans = b.m_DesignElements[0].compile().m_inTrans * yz_swap;
    glm::dmat4x4 outTrans = b.m_DesignElements[0].compile().m_outTrans * yz_swap;

    glm::dmat4x4 orientationResult = glm::dmat4x4(glm::dmat3x3(inTrans));
    glm::dvec4 positionResult = outTrans * glm::dvec4(0, 0, 0, 1);

    CHECK_EQ(orientationCorrect, orientationResult);
    CHECK_EQ(positionCorrect, positionResult);
}
