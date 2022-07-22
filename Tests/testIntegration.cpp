#include "setupTests.h"

using namespace RAYX;

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
    auto a_unfiltered = traceRML("ReflectionZonePlateAzim200");
    auto b = loadCSVRayUI("ReflectionZonePlateAzim200");

    RayList a;
    for (auto r : a_unfiltered) {
        if (r.m_extraParam == 21.0) {
            a.push(r);
        }
    }

    compareRayLists(a, b, 1e-7);
}
TEST_F(TestSuite, ReflectionZonePlateDefault) {
    compareAgainstRayUI("ReflectionZonePlateDefault");
}
TEST_F(TestSuite, ReflectionZonePlateDefault200) {
    compareAgainstRayUI("ReflectionZonePlateDefault200");
}
TEST_F(TestSuite, ReflectionZonePlateDefault200Toroid) {
    compareAgainstRayUI("ReflectionZonePlateDefault200Toroid");
}
TEST_F(TestSuite, ReflectionZonePlateMis) {
    compareAgainstRayUI("ReflectionZonePlateMis");
}
TEST_F(TestSuite, Spec1Pfirst_rzp4mm) {
    compareAgainstRayUI("Spec1+first_rzp4mm");
}
TEST_F(TestSuite, Spec1Mfirst_rzp02mm) {
    compareAgainstRayUI("Spec1-first_rzp02mm");
}
TEST_F(TestSuite, Spec1Mfirst_rzp4mm) {
    compareAgainstRayUI("Spec1-first_rzp4mm");
}
TEST_F(TestSuite, ellipsoid_ip_200ellipsmis) {
    compareAgainstRayUI("ellipsoid_ip_200ellipsmis");
}
TEST_F(TestSuite, ellipsoid_ip_200mirrormis) {
    compareAgainstRayUI("ellipsoid_ip_200mirrormis");
}
TEST_F(TestSuite, globalCoordinates_20rays) {
    compareAgainstRayUI("globalCoordinates_20rays");
}
TEST_F(TestSuite, pm_ell_ip_200mirrormis) {
    compareAgainstRayUI("pm_ell_ip_200mirrormis");
}
TEST_F(TestSuite, slit) { compareAgainstRayUI("slit"); }
TEST_F(TestSuite, toroid) { compareAgainstRayUI("toroid"); }

TEST_F(TestSuite, Ellipsoid) {
    auto rayx = traceRML("Ellipsoid");

    int count = 0;
    for (auto ray : rayx) {
        auto dist =
            abs(ray.m_extraParam - 21);  // 1 = Ellipsoid, 2 = ImagePlane
        if (dist < 0.5) {
            count += 1;
            CHECK_EQ(ray.m_position, glm::dvec3(0, 0, 0), 1e-11);
        }
    }
    if (count != 92) {
        RAYX_ERR << "unexpected number of rays (" << count
                 << ") hitting the ImagePlane from the "
                    "Ellipsoid!";
    }
}
