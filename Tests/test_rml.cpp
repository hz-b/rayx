#include "setupTests.h"
#if RUN_TEST_RML

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

TEST(RmlTest, groupTransform2) {
    auto b = RAYX::Importer::importBeamline(
        "../../Tests/input-files/groupTransform2.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(), 1);
    glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0,
                                        0.174, 0.985, 0, 0, 0, 0, 1);
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);
    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
    glm::dmat4x4 elementOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.996, -0.087, 0, 0,
                                          0.087, 0.996, 0, 0, 0, 0, 1);
    glm::dmat4x4 orientationCorrect = groupOr * elementOr;

    glm::dvec4 positionCorrect = groupPos + (groupOr * elementPos);
    glm::dmat4x4 orientationResult = b.m_OpticalElements[0]->getOrientation();
    glm::dvec4 positionResult = b.m_OpticalElements[0]->getPosition();
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>,
                              glmToVector16(orientationCorrect),
                              glmToVector16(orientationResult));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>,
                              glmToVector4(positionCorrect),
                              glmToVector4(positionResult));
}

TEST(RmlTest, groupTransformMisalignment) {
    auto b = RAYX::Importer::importBeamline(
        "../../Tests/input-files/groupTransformMisalignment.rml");
    ASSERT_EQ(b.m_LightSources.size(), 1);
    ASSERT_EQ(b.m_OpticalElements.size(), 1);

    glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0,
                                        0.174, 0.985, 0, 0, 0, 0, 1);
    glm::dmat4x4 elementOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.996, -0.087, 0, 0,
                                          0.087, 0.996, 0, 0, 0, 0, 1);
    glm::dmat4x4 misalignmentOr = getRotationMatrix(-0.004, 0.005, 0.006);
    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 0);
    glm::dvec4 offset = glm::dvec4(1, 2, 3, 1);

    glm::dmat4x4 orientationCorrect = groupOr * elementOr * misalignmentOr;
    glm::dvec4 positionCorrect =
        groupPos + groupOr * (elementPos + elementOr * misalignmentOr * offset);
    glm::dmat4x4 orientationResult = b.m_OpticalElements[0]->getOrientation();
    glm::dvec4 positionResult = b.m_OpticalElements[0]->getPosition();

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>,
                              glmToVector16(orientationCorrect),
                              glmToVector16(orientationResult));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>,
                              glmToVector4(positionCorrect),
                              glmToVector4(positionResult));
}

#endif