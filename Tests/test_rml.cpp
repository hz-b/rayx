#include "Data/Importer.h"
#include "Model/Beamline/LightSource.h"
#include "Model/Beamline/OpticalElement.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(RmlTest, allBeamlineObjects) {
    auto b =
        RAYX::Importer::importBeamline("../../Tests/input-files/test1.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(),
              8);  // plane mirror, toroid, slit, sphere grating, plane grating,
                   // sphere mirror, rzp, image plane
}

TEST(RmlTest, loadDatFile) {
    auto b =
        RAYX::Importer::importBeamline("../../Tests/input-files/test2.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(), 1);
    ASSERT_NEAR(b.m_LightSources[0]->m_EnergyDistribution.getAverage(),
                (12. + 15. + 17.) / 3, 0.1);
}

TEST(RmlTest, loadGroups) {
    auto b = RAYX::Importer::importBeamline(
        "../../Tests/input-files/loadGroups.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(), 4);
}

TEST(RmlTest, groupTransform) {
    auto b = RAYX::Importer::importBeamline(
        "../../Tests/input-files/groupTransform.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(), 1);
    auto m = b.m_OpticalElements[0]->getInMatrix();
    std::vector<double> correct = {
        1,   0, 0,     0,  //
        0,   0, 1,     0,  //
        0,   1, 0,     0,  //
        -42, 0, -1000, 1,  //
    };
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(correct[i], m[i]);
    }
}