#include "setupTests.h"

// in this module tests mostly compare rayx's output with RAY-UI.
// The output of Ray-UI is mostly generated using RawRaysOutgoing.

TEST_F(TestSuite, PlaneMirror) { compareLastAgainstRayUI("PlaneMirror"); }
TEST_F(TestSuite, PlaneMirrorDef) {
    compareLastAgainstRayUI("PlaneMirrorDef");

    // additional path length test
    auto rays = extractLastEvents(traceRML("PlaneMirrorDef"));
    for (auto r : rays) {
        CHECK_IN(r.m_pathLength, 11000, 11001);
    }
}
TEST_F(TestSuite, PlaneMirrorMis) { compareLastAgainstRayUI("PlaneMirrorMis"); }

TEST_F(TestSuite, SphereMirrorDefault) {
    compareLastAgainstRayUI("SphereMirrorDefault",
                            1e-7);  // TODO(Rudi) this tolerance is not so great.
}
TEST_F(TestSuite, PlaneGratingDeviationDefault) { compareLastAgainstRayUI("PlaneGratingDeviationDefault"); }
TEST_F(TestSuite, PlaneGratingDeviationAz) { compareLastAgainstRayUI("PlaneGratingDeviationAz"); }
TEST_F(TestSuite, PlaneGratingDeviationAzMis) { compareLastAgainstRayUI("PlaneGratingDeviationAzMis"); }

TEST_F(TestSuite, BoringImagePlane) { compareLastAgainstRayUI("BoringImagePlane"); }

TEST_F(TestSuite, CylinderDefault) { compareLastAgainstRayUI("CylinderDefault", 1e-7); }

TEST_F(TestSuite, PlaneGratingDevAzMisVLS) { compareLastAgainstRayUI("PlaneGratingDevAzMisVLS", 1e-7); }
TEST_F(TestSuite, PlaneGratingIncAzMis) { compareLastAgainstRayUI("PlaneGratingIncAzMis"); }
TEST_F(TestSuite, ReflectionZonePlateAzim200) { compareLastAgainstRayUI("ReflectionZonePlateAzim200", 1e-7); }
TEST_F(TestSuite, ReflectionZonePlateDefault) { compareLastAgainstRayUI("ReflectionZonePlateDefault"); }
TEST_F(TestSuite, ReflectionZonePlateDefault200) { compareLastAgainstRayUI("ReflectionZonePlateDefault200", 1e-7); }
// TEST_F(TestSuite, ReflectionZonePlateDefault200Toroid) { compareLastAgainstRayUI("ReflectionZonePlateDefault200Toroid", 1e-7); }
TEST_F(TestSuite, ReflectionZonePlateMis) { compareLastAgainstRayUI("ReflectionZonePlateMis", 1e-7); }

TEST_F(TestSuite, globalCoordinates_20rays) { compareLastAgainstRayUI("globalCoordinates_20rays"); }
TEST_F(TestSuite, pm_ell_ip_200mirrormis) { compareLastAgainstRayUI("pm_ell_ip_200mirrormis"); }

TEST_F(TestSuite, Ellipsoid) {
    auto rayx = traceRML("Ellipsoid");

    writeToOutputCSV(rayx, "Ellipsoid.rayx");

    bool found_atleast_one = false;
    for (auto ray_hist : rayx) {
        auto opt = lastSequentialHit(ray_hist, 2);
        if (opt) {
            CHECK_EQ(opt->m_position, glm::dvec3(0, 0, 0), 1e-11);
            found_atleast_one = true;
        }
    }
    CHECK(found_atleast_one);
}

TEST_F(TestSuite, Slit) {
    auto hist = traceRML("slit");

    int absorbed = 0;      // number of rays absorbed by the slit.
    int pass_through = 0;  // number of rays passing through the slit.

    const auto SLIT_ID = 0;
    const auto IMAGE_PLANE_ID = 1;

    for (auto ray_hist : hist) {
        if (ray_hist.size() == 1) {  // matrix source -> slit absorbed
            CHECK(ray_hist[0].m_lastElement == SLIT_ID);
            CHECK(ray_hist[0].m_eventType == ETYPE_ABSORBED);
            absorbed++;
        } else if (ray_hist.size() == 3) {  // matrix source -> slit -> image plane -> fly off
            CHECK(ray_hist[0].m_lastElement == SLIT_ID);
            CHECK(ray_hist[0].m_eventType == ETYPE_JUST_HIT_ELEM);

            CHECK(ray_hist[1].m_lastElement == IMAGE_PLANE_ID);
            CHECK(ray_hist[1].m_eventType == ETYPE_JUST_HIT_ELEM);

            CHECK(ray_hist[2].m_eventType == ETYPE_FLY_OFF);
            pass_through++;
        } else {
            CHECK(false);
        }
    }

    CHECK_EQ(absorbed, 108);
    CHECK_EQ(pass_through, 92);
}

// TODO re-enable toroid tests
// TEST_F(TestSuite, toroid) { compareLastAgainstRayUI("toroid"); }

// this is the same test as above, but xLength and zLength are exchanged. This
// tests the wasteBox, as not all rays hit the toroid.
// TEST_F(TestSuite, toroid_swapped) { compareLastAgainstRayUI("toroid_swapped"); }

TEST_F(TestSuite, Ellipsoid_DGIA) { compareLastAgainstRayUI("Ellipsoid_DGIA"); }
TEST_F(TestSuite, Ellipsoid_MB) { compareLastAgainstRayUI("Ellipsoid_MB"); }

// TODO: why is this called Ellipsoid_PM if it contains a PlaneMirror and no
// Ellipsoid?
TEST_F(TestSuite, Ellipsoid_PM) { compareLastAgainstRayUI("Ellipsoid_PM"); }

TEST_F(TestSuite, ellipsoid_ip_200mirrormis) { compareLastAgainstRayUI("ellipsoid_ip_200mirrormis"); }

TEST_F(TestSuite, Cone) { compareLastAgainstRayUI("Cone"); }

TEST_F(TestSuite, RZP_misalignment) { compareLastAgainstRayUI("RZP_misalignment"); }
