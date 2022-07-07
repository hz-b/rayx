#include "setupTests.h"

// TODO(Rudi) test other sources

TEST_F(TestSuite, MatrixSource) {
    auto beamline = loadBeamline("MatrixSource");
    auto a = beamline.getInputRays();
    auto b = loadCSVRayUI("MatrixSource");
    compareRayLists(a, b);
}
