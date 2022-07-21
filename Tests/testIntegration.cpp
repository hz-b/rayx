#include "setupTests.h"

TEST_F(TestSuite, BoringImagePlane) { compareAgainstRayUI("BoringImagePlane"); }
TEST_F(TestSuite, PlaneMirror) { compareAgainstRayUI("PlaneMirror"); }
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

TEST_F(TestSuite, PlaneMirrorMis) {
    auto a = traceRML("PlaneMirrorMis");
    auto b = loadCSVRayUI("PlaneMirrorMis");
    for (auto r : a) {
        r.m_stokes = glm::dvec4(0, 0, 0, 0);
    }
    for (auto r : b) {
        r.m_stokes = glm::dvec4(0, 0, 0, 0);
    }
    compareRayLists(a, b);
}

// TODO(rudi): this test fails. Possibly because of a wasteBox bug.
// TEST_F(TestSuite, Toroid) { compareAgainstRayUI("Toroid"); }
