#include "setupTests.h"

TEST_F(TestSuite, PlaneMirror) { compareAgainstRayUI("PlaneMirror"); }
TEST_F(TestSuite, Ellipsoid) {
    auto rayxGlobal = traceRML("Ellipsoid", false);

    int count = 0;
    for (auto l : rayxGlobal) {
        for (auto ray : l) {
            auto dist =
                abs(ray.m_extraParam - 21);  // 1 = Ellipsoid, 2 = ImagePlane
            if (dist < 0.5) {
                count += 1;
                CHECK_EQ(ray.m_position.x, 0, 1e-11);
                CHECK_EQ(ray.m_position.y, 0, 1e-11);
                CHECK_EQ(ray.m_position.z, 0, 1e-11);
            }
        }
    }
    if (count != 92) {
        RAYX_ERR << "unexpected number of rays (" << count
                 << ") hitting the ImagePlane from the "
                    "Ellipsoid!";
    }
}

// TODO(rudi): this test fails. Possibly because of a wasteBox bug.
// TEST_F(TestSuite, Toroid) { compareAgainstRayUI("Toroid"); }
