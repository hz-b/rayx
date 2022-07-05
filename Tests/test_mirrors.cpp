#include "setupTests.h"
#if RUN_TEST_MIRRORS

TEST(PlaneMirror, testSimpleParams) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_mirrors/testSimpleParams.rml"));

    auto plM = b.m_OpticalElements[0];

    glm::dmat4x4 correctInMatrix =
        glm::dmat4x4(1, 0, 0, 0, 0, 0.97357890287316029, 0.22835087011065572, 0,
                     0, -0.22835087011065572, 0.97357890287316029, 0, 0,
                     2741.352195678422, -11687.814728992289, 1);
    glm::dmat4x4 correctOutMatrix = glm::dmat4x4(
        1, 0, 0, 0, 0, 0.97357890287316029, -0.22835087011065572, 0, 0,
        0.22835087011065572, 0.97357890287316029, 0, 0, 0, 12005, 1);

    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};

    int icurv = 1;
    std::array<double, 4 * 4> surface = {
        0, 0, 0, 0, double(icurv), 0, 0, -1, 0, 0, 0, 0, 0, 0, 29, 0};

    CHECK_EQ(plM->getWidth(), 68.12);
    CHECK_EQ(plM->getHeight(), 123.6);
    CHECK_EQ(plM->getSlopeError(), sE);
    CHECK_EQ(plM->getSurfaceParams(), surface);
    CHECK_EQ(plM->getInMatrix(), correctInMatrix);
    CHECK_EQ(plM->getOutMatrix(), correctOutMatrix);
}

TEST(PlaneMirror, testAdvancedParams) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_mirrors/testAdvancedParams.rml"));

    auto plM = b.m_OpticalElements[0];

    int icurv = 1;
    std::array<double, 7> sE = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
    std::array<double, 4 * 4> surface = {
        0, 0, 0, 0, double(icurv), 0, 0, -1, 0, 0, 0, 0, 0, 0, 29, 0};

    glm::dmat4x4 correctInMatrix = glm::dmat4x4(
        0.98631018201912979, -0.16127244932632739, -0.034400900187032908, 0,
        0.16212528089630251, 0.91026081860532748, 0.38097327387397439, 0,
        -0.030126701440516851, -0.38133507470497185, 0.92394585483136815, 0,
        360.67105079340479, 4575.9275718331864, -11094.969987250573, 1);
    glm::dmat4x4 correctOutMatrix = glm::dmat4x4(
        0.98631018201912979, 0.16212528089630251, -0.030126701440516851, 0,
        -0.16127244932632739, 0.91026081860532748, -0.38133507470497185, 0,
        -0.034400900187032908, 0.38097327387397439, 0.92394585483136815, 0,
        0.56056258280537641, 3.1255667397288804, 12006.979040713644, 1);

    CHECK_EQ(plM->getWidth(), 124.12);
    CHECK_EQ(plM->getHeight(), 26);
    CHECK_EQ(plM->getSlopeError(), sE);
    CHECK_EQ(plM->getSurfaceParams(), surface);
    CHECK_EQ(plM->getInMatrix(), correctInMatrix);
    CHECK_EQ(plM->getOutMatrix(), correctOutMatrix);
}

TEST(SphereMirror, testParams) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_mirrors/testParams.rml"));

    std::shared_ptr<RAYX::SphereMirror> sM =
        std::dynamic_pointer_cast<RAYX::SphereMirror>(b.m_OpticalElements[0]);

    int icurv = 1;
    double radius = 104.32651829593351;  // from old RAY
    std::array<double, 7> sE = {0.7, 0.5, 0.3, 0.7, 0.3, 3, 2};
    std::array<double, 4 * 4> surface = {
        1, 0, 0, 0, double(icurv), 1, 0, -radius, 0, 0, 1, 0, 0, 0, 29, 0};

    glm::dmat4x4 correctInMatrix = glm::dmat4x4(
        0.024368111991334068, -0.85883516451860731, -0.51167211698926318, 0,
        0.39036506969235873, 0.47936537520215317, -0.78601777932905481, 0,
        0.92033760516565755, -0.18058515233428035, 0.34694047799924849, 0,
        -21.15449177460777, -48.811307953708521, -6.2049185933508921, 1);

    glm::dmat4x4 correctOutMatrix = glm::dmat4x4(
        0.024368111991334068, 0.39036506969235873, 0.92033760516565755, 0,
        -0.85883516451860731, 0.47936537520215317, -0.18058515233428035, 0,
        -0.51167211698926318, -0.78601777932905481, 0.34694047799924849, 0,
        -44.580256504514161, 26.779249273575296, 12.807414238606773, 1);

    CHECK_EQ(sM->getWidth(), 21.62);
    CHECK_EQ(sM->getHeight(), 813.12);
    CHECK_EQ(sM->getRadius(), radius, 0.0000000001);
    CHECK_EQ(sM->getExitArmLength(), 123.1);
    CHECK_EQ(sM->getEntranceArmLength(), 12.7);
    CHECK_EQ(sM->getSlopeError(), sE);
    CHECK_EQ(sM->getSurfaceParams(), surface);
    CHECK_EQ(sM->getInMatrix(), correctInMatrix);
    CHECK_EQ(sM->getOutMatrix(), correctOutMatrix);
}

TEST(Ellipse, defaultParams) {
    auto b = RAYX::importBeamline(
        resolvePath("Tests/rml_files/test_mirrors/defaultParams.rml"));

    auto eb =
        std::dynamic_pointer_cast<RAYX::Ellipsoid>(b.m_OpticalElements[0]);

    double correctTangentAngle = -0.14327895993853446;  // alpha1 in old ray code

    double shortHalfAxisC = 549.1237529650836;
    double y0 = -312.4887730997391;
    double z0 = 4522.597446463712;
    double shortHalfAxisB = 549.1237529650836;
    double longHalfAxisA = 5500;
    double m_a11 = 1;
    double m_a22 = 0.97981447224651996;
    double m_a23 = 0.13991733136051135;
    double m_a24 = -315.72395939432272;  // not equal to radius
    double a33 = 0.03015368960704581;
    double a34 = 0.0; // always equal to zero?
    double a44 = -2.9103830456733704e-11;
    std::array<double, 4 * 4> surface = {m_a11, 0, 0, 0, 
                                         1, m_a22, m_a23, m_a24,
                                         0, 0, a33, a34, 
                                         7, 0, 79, a44};
    std::array<double, 4 * 4> elementParams = {sin(correctTangentAngle),
                                               cos(correctTangentAngle),
                                               y0,
                                               z0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0,
                                               0};

    CHECK_EQ(eb->getShortHalfAxisB(), shortHalfAxisB, 0.0000001);
    CHECK_EQ(eb->getLongHalfAxisA(), longHalfAxisA, 0.0000001);
    CHECK_EQ(eb->getHalfAxisC(), shortHalfAxisC, 0.0000001);
    CHECK_EQ(eb->getY0(), y0, 0.0000001);
    CHECK_EQ(eb->getZ0(), z0, 0.0000001);
    CHECK_EQ(eb->getTangentAngle(), correctTangentAngle, 0.0000001);
    CHECK_EQ(eb->getSurfaceParams(), surface);
    CHECK_EQ(eb->getElementParameters(), elementParams);
}

#endif
