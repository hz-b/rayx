#include <Tracer/Vulkan/Material.h>

#include "setupTests.h"
#if RUN_TEST_MIRRORS

TEST(PlaneMirror, testSimpleParams) {
    auto b = RAYX::importBeamline(
        "../../Tests/rml_files/test_mirrors/testSimpleParams.rml");

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
        0, 0, 0, 0, double(icurv), 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0};

    CHECK_EQ(plM->getWidth(), 68.12);
    CHECK_EQ(plM->getHeight(), 123.6);
    CHECK_EQ(sE, plM->getSlopeError());
    CHECK_EQ(surface, plM->getSurfaceParams());
    CHECK_EQ(plM->getInMatrix(), correctInMatrix);
    CHECK_EQ(plM->getOutMatrix(), correctOutMatrix);
}

TEST(PlaneMirror, testAdvancedParams) {
    auto b = RAYX::importBeamline(
        "../../Tests/rml_files/test_mirrors/testAdvancedParams.rml");

    auto plM = b.m_OpticalElements[0];

    int icurv = 1;
    std::array<double, 7> sE = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
    std::array<double, 4 * 4> surface = {
        0, 0, 0, 0, double(icurv), 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0};

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
    CHECK_EQ(sE, plM->getSlopeError());
    CHECK_EQ(surface, plM->getSurfaceParams());
    CHECK_EQ(plM->getInMatrix(), correctInMatrix);
    CHECK_EQ(plM->getOutMatrix(), correctOutMatrix);
}

TEST(SphereMirror, testParams) {
    double width = 21.62;
    double height = 813.12;
    double incidence = 12.75;
    double azimuthal = 41.2;
    double dist = 12.12;
    double entranceArmLength = 12.7;
    double exitArmLength = 123.1;
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    int icurv = 1;
    double radius = 104.32651829593351;  // from old RAY
    std::array<double, 6> mis = {10, 51, 2, 0.1, 5, 0.241};
    std::array<double, 7> sE = {0.7, 0.5, 0.3, 0.7, 0.3, 3, 2};
    std::array<double, 4 * 4> surface = {
        1, 0, 0, 0, double(icurv), 1, 0, -radius, 0, 0, 1, 0, 0, 0, 0, 0};

    RAYX::GeometricUserParams g_params = RAYX::GeometricUserParams(incidence);
    RAYX::WorldUserParams w_params =
        RAYX::WorldUserParams(g_params.getAlpha(), g_params.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = w_params.calcPosition();
    glm::dmat4x4 orientation = w_params.calcOrientation();
    RAYX::SphereMirror sM = RAYX::SphereMirror(
        "spheremirror", geometricalShape, width, height,
        w_params.getAzimuthalAngle(), incidence, position, orientation,
        entranceArmLength, exitArmLength, sE, Material::CU);

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

    ASSERT_DOUBLE_EQ(sM.getWidth(), width);
    ASSERT_DOUBLE_EQ(sM.getHeight(), height);
    EXPECT_NEAR(sM.getRadius(), radius, 0.0000000001);
    ASSERT_DOUBLE_EQ(sM.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(sM.getEntranceArmLength(), entranceArmLength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(7, sE, sM.getSlopeError());
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, surface, sM.getSurfaceParams());
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, sM.getInMatrix(),
                                  glmToArray16(correctInMatrix));
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, sM.getOutMatrix(),
                                  glmToArray16(correctOutMatrix));
}

TEST(SphereMirror, testPrecalculateRadius) {
    double width = 21.62;
    double height = 813.12;
    double incidence = 12.75;
    double azimuthal = 41.2;
    double dist = 12.12;
    double entranceArmLength = 12.7;
    double exitArmLength = 123.1;
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    int icurv = 1;
    double radius = 104.32651829593351;  // from old RAY
    std::array<double, 6> mis = {10, 51, 2, 0.1, 5, 0.241};
    std::array<double, 7> sE = {0.7, 0.5, 0.3, 0.7, 0.3, 3, 2};
    std::array<double, 4 * 4> surface = {
        1, 0, 0, 0, double(icurv), 1, 0, -radius, 0, 0, 1, 0, 0, 0, 0, 0};

    RAYX::GeometricUserParams g_params = RAYX::GeometricUserParams(incidence);
    g_params.calcMirrorRadius(entranceArmLength, exitArmLength);
    RAYX::WorldUserParams w_params =
        RAYX::WorldUserParams(g_params.getAlpha(), g_params.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = w_params.calcPosition();
    glm::dmat4x4 orientation = w_params.calcOrientation();
    RAYX::SphereMirror sM = RAYX::SphereMirror(
        "spheremirror", geometricalShape, width, height,
        w_params.getAzimuthalAngle(), incidence, position, orientation,
        entranceArmLength, exitArmLength, sE, Material::CU);

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

    ASSERT_DOUBLE_EQ(sM.getWidth(), width);
    ASSERT_DOUBLE_EQ(sM.getHeight(), height);
    EXPECT_NEAR(g_params.getRadius(), radius, 0.0000000001);
    ASSERT_DOUBLE_EQ(sM.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(sM.getEntranceArmLength(), entranceArmLength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(7, sE, sM.getSlopeError());
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, surface, sM.getSurfaceParams());
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, sM.getInMatrix(),
                                  glmToArray16(correctInMatrix));
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, sM.getOutMatrix(),
                                  glmToArray16(correctOutMatrix));
}

TEST(Ellips, defaultParams) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double width = 50;
    double height = 200;
    double incidence = 10;
    double entranceArmLength = 10000;
    double exitArmLength = 1000;

    double a11 = 1;
    RAYX::FigureRotation figRot = RAYX::FigureRotation::Yes;

    double dist = 10000;
    double chi = 0;
    std::array<double, 6> mis = {1, 2, 3, 0.004, 0.005, 0.006};
    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};

    double alpha = 0.031253965260898464;
    double beta = 0.31781188513796743;
    double correctTangentAngle = 0.14327895993853446;  // alpha1 in old ray code
    RAYX::GeometricUserParams e_params =
        RAYX::GeometricUserParams(incidence, entranceArmLength, exitArmLength);
    ASSERT_DOUBLE_EQ(e_params.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(e_params.getBeta(), beta);
    double tangentAngle = e_params.calcTangentAngle(
        incidence, entranceArmLength, exitArmLength, 1);
    ASSERT_DOUBLE_EQ(correctTangentAngle, tangentAngle);
    tangentAngle = e_params.calcTangentAngle(incidence, entranceArmLength,
                                             exitArmLength, 0);
    ASSERT_DOUBLE_EQ(0, tangentAngle);

    double shortHalfAxisC = 549.1237529650836;
    double y0 = -312.4887730997391;
    double z0 = 4522.597446463379;
    double shortHalfAxisB = 549.1237529650836;
    double longHalfAxisA = 5500;

    double a33 = 0.0099681618535688628;
    double a34 = 45.0819833448842;
    double a44 = -5.8207660913467407e-11;
    std::array<double, 4 * 4> surface = {1, 0, 0,   0,   1, 1, 0, y0,
                                         0, 0, a33, a34, 7, 0, 0, a44};
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

    RAYX::WorldUserParams w_params =
        RAYX::WorldUserParams(e_params.getAlpha(), e_params.getBeta(),
                              degToRad(chi), dist, mis, tangentAngle);
    RAYX::Ellipsoid eb = RAYX::Ellipsoid(
        "ellipsoid", geometricalShape, width, height,
        w_params.getAzimuthalAngle(), w_params.calcPosition(),
        w_params.calcOrientation(), incidence, entranceArmLength, exitArmLength,
        figRot, a11, sE, Material::CU);

    EXPECT_NEAR(eb.getShortHalfAxisB(), shortHalfAxisB, 0.0000001);
    EXPECT_NEAR(eb.getLongHalfAxisA(), longHalfAxisA, 0.0000001);
    EXPECT_NEAR(eb.getHalfAxisC(), shortHalfAxisC, 0.0000001);
    EXPECT_NEAR(eb.getY0(), y0, 0.0000001);
    EXPECT_NEAR(eb.getZ0(), z0, 0.0000001);
    EXPECT_NEAR(eb.getTangentAngle(), correctTangentAngle, 0.0000001);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, surface, eb.getSurfaceParams());
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, elementParams,
                                  eb.getElementParameters());
}

#endif
