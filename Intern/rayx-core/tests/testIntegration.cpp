#include "setupTests.h"

// in this module tests mostly compare rayx's output with RAY-UI.
// The output of Ray-UI is mostly generated using RawRaysOutgoing.

TEST_F(TestSuite, ImagePlane) { compareLastAgainstRayUI("ImagePlane"); }
TEST_F(TestSuite, PlaneMirror) { compareLastAgainstRayUI("PlaneMirror"); }
TEST_F(TestSuite, PlaneMirrorDef) {
    compareLastAgainstRayUI("PlaneMirrorDef");
    RAYX_LOG << "planemirrdef ";
    // additional path length test
    auto rays = extractLastEvents(traceRML("PlaneMirrorDef"));
    for (auto r : rays) {
        CHECK_IN(r.m_pathLength, 11000, 11001)
    }
}
TEST_F(TestSuite, PlaneMirrorMis) { compareLastAgainstRayUI("PlaneMirrorMis"); }
TEST_F(TestSuite, SphereMirrorDefault) { compareLastAgainstRayUI("SphereMirrorDefault"); }

// In these tests the ray would hit the ImagePlane before the PlaneGrating, if we trace dynamically.
// This prevents comparing the results to Ray-UI, and hence we do sequential tracing here to allow for such a comparison.
TEST_F(TestSuite, PlaneGratingDeviationDefault) { compareLastAgainstRayUI("PlaneGratingDeviationDefault", 1e-11, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDeviationAz) { compareLastAgainstRayUI("PlaneGratingDeviationAz", 1e-11, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDeviationAzMis) { compareLastAgainstRayUI("PlaneGratingDeviationAzMis", 1e-11, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDevAzMisVLS) { compareLastAgainstRayUI("PlaneGratingDevAzMisVLS", 1e-7, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingIncAzMis) { compareLastAgainstRayUI("PlaneGratingIncAzMis", 1e-11, Sequential::Yes); }

TEST_F(TestSuite, BoringImagePlane) { compareLastAgainstRayUI("BoringImagePlane"); }

TEST_F(TestSuite, CylinderDefault) { compareLastAgainstRayUI("CylinderDefault", 1e-7); }

TEST_F(TestSuite, ReflectionZonePlateAzim200) { compareLastAgainstRayUI("ReflectionZonePlateAzim200", 1e-7); }
TEST_F(TestSuite, ReflectionZonePlateDefault) { compareLastAgainstRayUI("ReflectionZonePlateDefault"); }
TEST_F(TestSuite, ReflectionZonePlateDefault200) { compareLastAgainstRayUI("ReflectionZonePlateDefault200", 1e-7); }

// TODO re-enable this test:
// It seems to be caused by imprecision in the current toroid collision being larger than the COLLISION_EPSILON.
// TEST_F(TestSuite, ReflectionZonePlateDefault200Toroid) { compareLastAgainstRayUI("ReflectionZonePlateDefault200Toroid", 1e-7); }

TEST_F(TestSuite, ReflectionZonePlateMis) { compareLastAgainstRayUI("ReflectionZonePlateMis", 1e-7); }

TEST_F(TestSuite, globalCoordinates_20rays) { compareLastAgainstRayUI("globalCoordinates_20rays"); }
TEST_F(TestSuite, pm_ell_ip_200mirrormis) { compareLastAgainstRayUI("pm_ell_ip_200mirrormis"); }

TEST_F(TestSuite, Ellipsoid) {
    auto rayx = traceRML("Ellipsoid");

    writeToOutputCSV(rayx, "Ellipsoid.rayx");

    bool found_atleast_one = false;
    for (const auto& ray_hist : rayx) {
        auto opt = lastSequentialHit(ray_hist, 2);
        if (opt) {
            CHECK_EQ(opt->m_position, glm::dvec3(0, 0, 0), 1e-11);
            found_atleast_one = true;
        }
    }
    CHECK(found_atleast_one)
}

TEST_F(TestSuite, toroid) { compareLastAgainstRayUI("toroid"); }

// this is the same test as above, but xLength and zLength are exchanged. This
// tests the wasteBox, as not all rays hit the toroid.
TEST_F(TestSuite, toroid_swapped) { compareLastAgainstRayUI("toroid_swapped"); }

TEST_F(TestSuite, Ellipsoid_DGIA) { compareLastAgainstRayUI("Ellipsoid_DGIA"); }
TEST_F(TestSuite, Ellipsoid_MB) { compareLastAgainstRayUI("Ellipsoid_MB"); }

// TODO: why is this called Ellipsoid_PM if it contains a PlaneMirror and no
// Ellipsoid?
TEST_F(TestSuite, Ellipsoid_PM) { compareLastAgainstRayUI("Ellipsoid_PM"); }

TEST_F(TestSuite, ellipsoid_ip_200mirrormis) { compareLastAgainstRayUI("ellipsoid_ip_200mirrormis"); }

TEST_F(TestSuite, Cone) { compareLastAgainstRayUI("Cone"); }

TEST_F(TestSuite, RZP_misalignment) { compareLastAgainstRayUI("RZP_misalignment", 1e-10); }

TEST_F(TestSuite, RZP_plane) { compareLastAgainstRayUI("test-rzp-1-plane", 1e-10); }
TEST_F(TestSuite, RZP_spherical) { compareLastAgainstRayUI("test-rzp-1-spherical", 1e-10); }
