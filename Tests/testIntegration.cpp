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
