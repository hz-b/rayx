#include "setupTests.h"

// in this module tests mostly compare rayx's output with RAY-UI.
// The output of Ray-UI is mostly generated using RawRaysOutgoing.

TEST_F(TestSuite, ImagePlane) { traceRmlAndCompareAgainstRayUi("ImagePlane"); }
TEST_F(TestSuite, ImagePlaneRectangle) { loadBeamline("ImagePlaneRectangle"); }  // no RAYUI equivalent
TEST_F(TestSuite, PlaneMirror) { traceRmlAndCompareAgainstRayUi("PlaneMirror"); }
TEST_F(TestSuite, PlaneMirrorDef) {
    traceRmlAndCompareAgainstRayUi("PlaneMirrorDef");
    // additional path length test
    auto rays = traceRml("PlaneMirrorDef", RayAttrMask::PathId | RayAttrMask::PathEventId | RayAttrMask::OpticalPathLength).filterByLastEventInPath();
    for (const auto l : rays.optical_path_length) CHECK_IN(l, 11000, 11001);
}
TEST_F(TestSuite, PlaneMirrorMis) { traceRmlAndCompareAgainstRayUi("PlaneMirrorMis"); }
TEST_F(TestSuite, SphereMirrorDefault) { traceRmlAndCompareAgainstRayUi("SphereMirrorDefault", 1e-10); }
TEST_F(TestSuite, SphereGrating) { traceRmlAndCompareAgainstRayUi("SphereGrating", 1e-12, Sequential::Yes); }
TEST_F(TestSuite, ToroidGrating) { traceRmlAndCompareAgainstRayUi("ToroidGrating", 1e-12, Sequential::Yes); }

// In these tests the ray would hit the ImagePlane before the PlaneGrating, if we trace dynamically.
// This prevents comparing the results to Ray-UI, and hence we do sequential tracing here to allow for such a comparison.
TEST_F(TestSuite, PlaneGratingDeviationDefault) { traceRmlAndCompareAgainstRayUi("PlaneGratingDeviationDefault", 1e-12, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDeviationAz) { traceRmlAndCompareAgainstRayUi("PlaneGratingDeviationAz", 1e-9, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDeviationAzMis) { traceRmlAndCompareAgainstRayUi("PlaneGratingDeviationAzMis", 1e-12, Sequential::Yes); }
TEST_F(TestSuite, PlaneGratingDevAzMisVLS) {
    traceRmlAndCompareAgainstRayUi("PlaneGratingDevAzMisVLS", 1e-8, Sequential::Yes);
}  // TODO: rays dont get absorbed here (rayx_list.size = 200, should be 67)
TEST_F(TestSuite, PlaneGratingIncAzMis) { traceRmlAndCompareAgainstRayUi("PlaneGratingIncAzMis", 1e-11, Sequential::Yes); }

TEST_F(TestSuite, BoringImagePlane) { traceRmlAndCompareAgainstRayUi("BoringImagePlane"); }

TEST_F(TestSuite, CylinderDefault) { traceRmlAndCompareAgainstRayUi("CylinderDefault", 1e-7); }

TEST_F(TestSuite, ReflectionZonePlateAzim200) { traceRmlAndCompareAgainstRayUi("ReflectionZonePlateAzim200", 1e-7); }
TEST_F(TestSuite, ReflectionZonePlateDefault) { traceRmlAndCompareAgainstRayUi("ReflectionZonePlateDefault"); }
TEST_F(TestSuite, ReflectionZonePlateDefault200) { traceRmlAndCompareAgainstRayUi("ReflectionZonePlateDefault200", 1e-7); }

// TODO re-enable this test:
// It seems to be caused by imprecision in the current toroid collision being larger than the COLLISION_EPSILON.
// TEST_F(TestSuite, ReflectionZonePlateDefault200Toroid) { traceRmlAndCompareAgainstRayUi("ReflectionZonePlateDefault200Toroid", 1e-7); }

TEST_F(TestSuite, ReflectionZonePlateMis) { traceRmlAndCompareAgainstRayUi("ReflectionZonePlateMis", 1e-7); }

TEST_F(TestSuite, globalCoordinates_20rays) { traceRmlAndCompareAgainstRayUi("globalCoordinates_20rays"); }
TEST_F(TestSuite, pm_ell_ip_200mirrormis) { traceRmlAndCompareAgainstRayUi("pm_ell_ip_200mirrormis"); }

TEST_F(TestSuite, Ellipsoid) {
    const auto rays = traceRml("Ellipsoid", RayAttrMask::Position | RayAttrMask::ObjectId, Sequential::Yes).filterByObjectId(2);
    writeCsvUsingFilename(rays, "Ellipsoid.rayx");
    expectEqual(rays.position_x, 0.0);
    expectEqual(rays.position_y, 0.0);
    expectEqual(rays.position_x, 0.0);
}

TEST_F(TestSuite, toroid) { traceRmlAndCompareAgainstRayUi("toroid"); }

// this is the same test as above, but xLength and zLength are exchanged. This
// tests the wasteBox, as not all rays hit the toroid.
TEST_F(TestSuite, toroid_swapped) { traceRmlAndCompareAgainstRayUi("toroid_swapped"); }

TEST_F(TestSuite, Ellipsoid_DGIA) { traceRmlAndCompareAgainstRayUi("Ellipsoid_DGIA"); }
TEST_F(TestSuite, Ellipsoid_MB) { traceRmlAndCompareAgainstRayUi("Ellipsoid_MB"); }

TEST_F(TestSuite, ellipsoid_ip_200mirrormis) { traceRmlAndCompareAgainstRayUi("ellipsoid_ip_200mirrormis"); }

TEST_F(TestSuite, Cone) { traceRmlAndCompareAgainstRayUi("Cone"); }

TEST_F(TestSuite, RZP_misalignment) { traceRmlAndCompareAgainstRayUi("RZP_misalignment", 1e-10); }

TEST_F(TestSuite, RZP_plane) { traceRmlAndCompareAgainstRayUi("test-rzp-1-plane", 1e-10); }
TEST_F(TestSuite, RZP_spherical) { traceRmlAndCompareAgainstRayUi("test-rzp-1-spherical", 1e-10); }

TEST_F(TestSuite, Crystal) { traceRmlAndCompareAgainstRayUi_allowMontecarloRayUi("crystal"); }

TEST_F(TestSuite, MultilayerCone) { traceRmlAndCompareAgainstRayUi("MultilayerCone", 1e-7); }

TEST_F(TestSuite, Foil) { traceRmlAndCompareAgainstRayUi("Foil", 1e-9); }
