#include <fstream>
#include <sstream>

#include "Core.h"
#include "Model/Beamline/Beamline.h"
#include "Model/Beamline/Objects/MatrixSource.h"
#include "Model/Beamline/Objects/PointSource.h"
#include "Tracer/Ray.h"
#include "Tracer/VulkanTracer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "setupTests.h"

// TODO(Rudi) test other sources

TEST_F(TestSuite, MatrixSource) {
    auto beamline = loadBeamline("MatrixSource");
    auto a = beamline.getInputRays();
    auto b = loadCSVRayUI("MatrixSource");
    compareRayLists(a, b);
}