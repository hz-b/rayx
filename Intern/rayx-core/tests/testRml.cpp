#include "setupTests.h"

TEST_F(TestSuite, allBeamlineObjects) {
    auto b = loadBeamline("allBeamlineObjects");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(),
             8);  // plane mirror, toroid, slit, sphere grating, plane grating,
                  // sphere mirror, rzp, image plane
}

TEST_F(TestSuite, loadDatFile) {
    auto b = loadBeamline("loadDatFile");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.getAverage(), (12. + 15. + 17.) / 3, 0.1);
}

TEST_F(TestSuite, loadDatFile2) {
    auto b = loadBeamline("loadDatFile2");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.getAverage(), 100, 0.1); //TODO value needs to be confirmed
    CHECK_EQ(b.m_LightSources[0]->m_EnergyDistribution.selectEnergy(), 100, 0.1); //TODO value needs to be confirmed
}


TEST_F(TestSuite, loadGroups) {
    auto b = loadBeamline("loadGroups");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 4);
}

TEST_F(TestSuite, groupTransform) {
    auto b = loadBeamline("groupTransform");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);
    auto m = b.m_OpticalElements[0].m_element.m_inTrans;
    glm::dmat4x4 correct = {
        1,   0,  0,     0,  //
        0,   0,  1,     0,  //
        0,   -1, 0,     0,  //
        -42, 0,  -1000, 1,  //
    };
    CHECK_EQ(correct, m);
}

// TODO(rudi) re-enable group tests

// TEST_F(TestSuite, groupTransform2) {
//     auto b = loadBeamline("groupTransform2");
//     CHECK_EQ(b.m_LightSources.size(), 1);
//     CHECK_EQ(b.m_OpticalElements.size(), 1);
//     glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0, 0.174, 0.985, 0, 0, 0, 0, 1);
//     glm::dvec4 elementPos = glm::dvec4(0, 0, 1000, 1);
//     glm::dvec4 groupPos = glm::dvec4(42, 2, 4, 0);
//     glm::dmat4x4 elementOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.996, -0.087, 0, 0, 0.087, 0.996, 0, 0, 0, 0, 1);

//     glm::dmat4x4 orientationCorrect = groupOr * elementOr;
//     glm::dvec4 positionCorrect = groupPos + (groupOr * elementPos);

//     glm::dmat4x4 orientationResult = b.m_OpticalElements[0].m_element.m_inTrans;
//     glm::dvec4 positionResult = b.m_OpticalElements[0].m_element.m_inTrans * glm::dvec4(0, 0, 0, 1);

//     CHECK_EQ((orientationCorrect), (orientationResult));
//     CHECK_EQ((positionCorrect), (positionResult));
// }

/*
TEST_F(TestSuite, groupTransformMisalignment) {
    auto b = loadBeamline("groupTransformMisalignment");
    CHECK_EQ(b.m_LightSources.size(), 1);
    CHECK_EQ(b.m_OpticalElements.size(), 1);

    glm::dmat4x4 groupOr = glm::dmat4x4(1, 0, 0, 0, 0, 0.985, -0.174, 0, 0, 0.174, 0.985, 0, 0, 0, 0, 1);
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
*/
