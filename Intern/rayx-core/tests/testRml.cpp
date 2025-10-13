#include "setupTests.h"

TEST_F(TestSuite, allBeamlineObjects) {
    RAYX::Beamline bl = loadBeamline("allBeamlineObjects");
    // Point, Circle, Dipole, Matrix, Pixel, simple Undulator
    CHECK_EQ(bl.numSources(), 6);
    // Cone, Cylinder, Ellipsoid, Paraboloid, plane mirror, toroid, slit, sphere grating, plane grating, sphere mirror, rzp, image plane
    CHECK_EQ(bl.numElements(), 12);
}

TEST_F(TestSuite, loadDatFile) {
    const auto rays = traceRml("loadDatFile", RayAttrMask::Energy);
    writeCsvUsingFilename(rays, "loadDatFile.rayx");
    expectEqualAny(rays.energy, {12.0, 15.0, 17.0});
}

TEST_F(TestSuite, loadDatFile2) {
    const auto rays = traceRml("loadDatFile2", RayAttrMask::Energy);
    expectEqualAny(rays.energy, {12.0, 15.0, 17.0});
}

TEST_F(TestSuite, loadGroups) {
    RAYX::Beamline bl = loadBeamline("loadGroups");
    CHECK_EQ(bl.numSources(), 1);
    CHECK_EQ(bl.numElements(), 4);
}

TEST_F(TestSuite, groupTransform) {
    RAYX::Beamline bl = loadBeamline("groupTransform");
    CHECK_EQ(bl.numSources(), 1);
    CHECK_EQ(bl.numElements(), 1);
    auto m = bl.compileElements()[0].transform.m_inTrans;
    glm::dmat4x4 correct = {
        1,   0,     0,  0,  //
        0,   1,     0,  0,  //
        0,   0,     -1, 0,  //
        -42, -1000, 0,  1,  //
    };
    CHECK_EQ(correct, m);
}

TEST_F(TestSuite, testEnergyDistributionSeparateEnergies) {
    const auto rays = traceRml("PointSourceSeparateEnergies", RayAttrMask::Energy);
    expectAtLeastOnce(rays.energy, {
                                       0.0,
                                       50.0,
                                       100.0,
                                       150.0,
                                       200.0,
                                   });
}

// TODO: this test does not really test anything
TEST_F(TestSuite, testEnergyDistributionSoftEdge) {
    const auto rays = traceRml("PointSourceSoftEdgeEnergy", RayAttrMask::Energy);
    expectDifferentValues(rays.energy);
}

// TODO: this test does not really test anything
TEST_F(TestSuite, testEnergyDistributionHardEdge) {
    {
        const auto rays = traceRml("PointSourceHardEdgeEnergy", RayAttrMask::Energy);
        expectInRange(rays.energy, 0.0, 200.0);
        expectDifferentValues(rays.energy);
    }
    {
        const auto rays = traceRml("PointSourceHardEdge", RayAttrMask::Energy);
        expectInRange(rays.energy, 0.0, 200.0);
        expectDifferentValues(rays.energy);
    }
}

TEST_F(TestSuite, testParaboloidQuad) {
    auto beamline = loadBeamline("paraboloid_matrix_IP");

    OpticalElement para = beamline.compileElements()[0].element;
    auto parabo = para.m_surface.get<Surface::Quadric>();

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
    OpticalElement sph = beamline.compileElements()[0].element;
    auto sphere = sph.m_surface.get<Surface::Quadric>();

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
    OpticalElement elli = beamline.compileElements()[0].element;
    auto ellips = elli.m_surface.get<Surface::Quadric>();

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
    OpticalElement cyli = beamline.compileElements()[0].element;
    auto cylinder = cyli.m_surface.get<Surface::Quadric>();

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
    OpticalElement con = beamline.compileElements()[0].element;
    auto cone = con.m_surface.get<Surface::Quadric>();

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

TEST_F(TestSuite, test_Toroid) {
    auto beamline = loadBeamline("toroid");
    OpticalElement trid = beamline.compileElements()[0].element;
    auto toroid = trid.m_surface.get<Surface::Toroid>();

    CHECK_EQ(10470.4917, toroid.m_longRadius, 0.001);
    CHECK_EQ(315.723959, toroid.m_shortRadius, 0.001);
    EXPECT_EQ(toroid.m_toroidType, ToroidType::Concave);
}

TEST_F(TestSuite, testExpertsOptic) {
    auto beamline = loadBeamline("toroid");
    OpticalElement trid = beamline.compileElements()[0].element;
    auto toroid = trid.m_surface.get<Surface::Toroid>();

    CHECK_EQ(10470.4917, toroid.m_longRadius, 0.001);
    CHECK_EQ(315.723959, toroid.m_shortRadius, 0.001);
    EXPECT_EQ(toroid.m_toroidType, ToroidType::Concave);
}

/***
 * Tests if two sources can be traced in one go.
 * Its a static test, so every change can result in a fail even if it's still working correctly
 */
// TODO: write more tests with two or more sources
TEST_F(TestSuite, testTwoSourcesInOneRML) {
    auto [beamline, rays] = loadBeamlineAndTrace("twoSourcesTest");
    CHECK_EQ(beamline.numSources(), 2);

    const auto source0Rays = rays.filterByObjectId(0);
    CHECK_EQ(source0Rays.size(), 200);

    const auto source1Rays = rays.filterByObjectId(1);
    CHECK_EQ(source1Rays.size(), 200);

    const auto imagePlaneRays = rays.filterByObjectId(2);
    CHECK_EQ(imagePlaneRays.size(), 400);
}

TEST_F(TestSuite, groupTransform2) {
    RAYX::Beamline bl = loadBeamline("groupTransform2");
    CHECK_EQ(bl.numSources(), 1);
    CHECK_EQ(bl.numElements(), 1);

    glm::dmat4x4 yz_swap = {
        1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    };

    glm::dmat4x4 groupOr = glm::dmat4x4(  //
        1, 0, 0, 0,                       //
        0, 0.985, -0.174, 0,              //
        0, 0.174, 0.985, 0,               //
        0, 0, 0, 1);                      //
    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);

    glm::dmat4x4 elementOr = glm::dmat4x4(  //
        1, 0, 0, 0,                         //
        0, 1, 0, 0,                         //
        0, 0, 1, 0,                         //
        0, 0, 0, 1);                        //
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);

    glm::dmat4x4 orientationCorrect = groupOr * elementOr;
    glm::dvec4 positionCorrect = groupPos + (groupOr * elementPos);

    glm::dmat4x4 inTrans = bl.compileElements()[0].transform.m_inTrans * yz_swap;
    glm::dmat4x4 outTrans = bl.compileElements()[0].transform.m_outTrans * yz_swap;

    glm::dmat4x4 orientationResult = glm::dmat4x4(glm::dmat3x3(inTrans));
    glm::dvec4 positionResult = outTrans * glm::dvec4(0, 0, 0, 1);

    CHECK_EQ(orientationCorrect, orientationResult);
    CHECK_EQ(positionCorrect, positionResult);
}
