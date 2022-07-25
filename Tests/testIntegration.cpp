#include "setupTests.h"

TEST_F(TestSuite, PlaneMirror) { compareAgainstRayUI("PlaneMirror"); }
TEST_F(TestSuite, PlaneMirrorDef) { compareAgainstRayUI("PlaneMirrorDef"); }
TEST_F(TestSuite, PlaneMirrorMis) { compareAgainstRayUI("PlaneMirrorMis"); }

TEST_F(TestSuite, SphereMirrorDefault) {
    compareAgainstRayUI("SphereMirrorDefault",
                        1e-7);  // TODO(Rudi) this tolerance is not so great.
}
TEST_F(TestSuite, PlaneGratingDeviationDefault) {
    compareAgainstRayUI("PlaneGratingDeviationDefault");
}
TEST_F(TestSuite, PlaneGratingDeviationAz) {
    compareAgainstRayUI("PlaneGratingDeviationAz");
}
TEST_F(TestSuite, PlaneGratingDeviationAzMis) {
    compareAgainstRayUI("PlaneGratingDeviationAzMis");
}

TEST_F(TestSuite, BoringImagePlane) { compareAgainstRayUI("BoringImagePlane"); }

TEST_F(TestSuite, CylinderDefault) {
    compareAgainstRayUI("CylinderDefault", 1e-7);
}

TEST_F(TestSuite, PlaneGratingDevAzMisVLS) {
    compareAgainstRayUI("PlaneGratingDevAzMisVLS", 1e-7);
}
TEST_F(TestSuite, PlaneGratingIncAzMis) {
    compareAgainstRayUI("PlaneGratingIncAzMis");
}
TEST_F(TestSuite, ReflectionZonePlateAzim200) {
    compareAgainstRayUI("ReflectionZonePlateAzim200", 1e-7);
}
TEST_F(TestSuite, ReflectionZonePlateDefault) {
    compareAgainstRayUI("ReflectionZonePlateDefault");
}
TEST_F(TestSuite, ReflectionZonePlateDefault200) {
    compareAgainstRayUI("ReflectionZonePlateDefault200");
}
TEST_F(TestSuite, ReflectionZonePlateDefault200Toroid) {
    compareAgainstRayUI("ReflectionZonePlateDefault200Toroid", 1e-7);
}
TEST_F(TestSuite, ReflectionZonePlateMis) {
    compareAgainstRayUI("ReflectionZonePlateMis", 1e-7);
}

TEST_F(TestSuite, globalCoordinates_20rays) {
    compareAgainstRayUI("globalCoordinates_20rays");
}
TEST_F(TestSuite, pm_ell_ip_200mirrormis) {
    compareAgainstRayUI("pm_ell_ip_200mirrormis");
}

TEST_F(TestSuite, Ellipsoid) {
    auto rayx = traceRML("Ellipsoid").filter([](Ray& r) {
        return r.m_extraParam == 21.0;
    });
    CHECK_EQ(rayx.rayAmount(), 92);
    for (auto r : rayx) {
        CHECK_EQ(r.m_position, glm::dvec3(0, 0, 0), 1e-11);
    }
}
// TODO(Rudi): fix and re-enable tests:
/*
TEST_F(TestSuite, Spec1Pfirst_rzp4mm) {
    auto a = traceRML("Spec1+first_rzp4mm").filter([](Ray& r) {
        return r.m_extraParam == 21.0;
    });
    auto b = loadCSVRayUI("Spec1+first_rzp4mm");

    compareRayLists(a, b, 1e-7);
}
TEST_F(TestSuite, Spec1Mfirst_rzp4mm) {
    auto a = traceRML("Spec1-first_rzp4mm").filter([](Ray& r) {
        return r.m_extraParam == 21.0;
    });
    auto b = loadCSVRayUI("Spec1-first_rzp4mm");

    compareRayLists(a, b, 1e-7);
}
TEST_F(TestSuite, Spec1Mfirst_rzp02mm) {
    auto a = traceRML("Spec1-first_rzp02mm").filter([](Ray& r) {
        return r.m_extraParam == 21.0;
    });
    auto b = loadCSVRayUI("Spec1-first_rzp02mm");

    compareRayLists(a, b, 1e-7);
}
TEST_F(TestSuite, ellipsoid_ip_200ellipsmis) {
    compareAgainstRayUI("ellipsoid_ip_200ellipsmis");
}
TEST_F(TestSuite, ellipsoid_ip_200mirrormis) {
    compareAgainstRayUI("ellipsoid_ip_200mirrormis");
}
TEST_F(TestSuite, slit) { compareAgainstRayUI("slit"); }
TEST_F(TestSuite, toroid) { compareAgainstRayUI("toroid"); }
*/