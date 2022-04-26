#include "setupTests.h"
#if RUN_TEST_RML

TEST(RmlTest, allBeamlineObjects) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/allBeamlineObjects.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(),
             8);  // plane mirror, toroid, slit, sphere grating, plane grating,
                  // sphere mirror, rzp, image plane
}

TEST(RmlTest, loadDatFile) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/loadDatFile.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.getAverage(),
             (12. + 15. + 17.) / 3, 0.1);
}

TEST(RmlTest, loadGroups) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/loadGroups.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 4);
}

TEST(RmlTest, groupTransform) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/groupTransform.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    auto m = b.m_OpticalElements[0]->getInMatrix();
    std::array<double, 4 * 4> correct = {
        1,   0, 0,     0,  //
        0,   0, 1,     0,  //
        0,   1, 0,     0,  //
        -42, 0, -1000, 1,  //
    };
    CHECK_EQ(correct, m);
}

TEST(RmlTest, groupTransform2) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/groupTransform2.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
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
    CHECK_EQ((orientationCorrect), (orientationResult));
    CHECK_EQ((positionCorrect), (positionResult));
}

TEST(RmlTest, groupTransformMisalignment) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_rml/groupTransformMisalignment.rml"));
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);

    glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0,
                                        0.174, 0.985, 0, 0, 0, 0, 1);
    printDMat4(groupOr);

    glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
    glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);

    glm::dvec4 positionCorrect = groupPos + groupOr * elementPos;
    glm::dmat4x4 orientationResult = b.m_OpticalElements[0]->getOrientation();
    printDMat4(orientationResult);
    glm::dvec4 positionResult = b.m_OpticalElements[0]->getPosition();

    CHECK_EQ(positionCorrect, positionResult);
    CHECK_EQ(groupOr, orientationResult, 1e-15);
}

#endif
