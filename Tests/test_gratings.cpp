#include "setupTests.h"

TEST(planeGrating, testParams) {
    // user parameters
    double azimuthal = 61.142;
    double dist = 1245.71;

    int mount =
        0;  // { GM_DEVIATION, GM_INCIDENCE, GM_CCF, GM_CCF_NO_PREMIRROR}
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::vector<double> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    // other user parameters
    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int orderOfDiffraction = 1;
    int add_order = 0;

    // derived from above parameters
    double beta = 1.4676505971882481;
    double alpha = 1.457521229154248;
    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    ASSERT_DOUBLE_EQ(g_guparam.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(g_guparam.getAlpha(), g_guparam.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    std::vector<double> sE = {0, 0, 0, 0, 0, 0, 0};
    std::vector<double> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};
    RAYX::PlaneGrating p1 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height, position, orientation,
        designEnergy, lineDensity, orderOfDiffraction, add_order, vls, sE);

    std::vector<double> surface = {0, 0, 0, 0, 1, 0, 0, -1,
                                   0, 0, 0, 0, 1, 0, 0, 0};
    std::vector<double> objparams = {width, height, sE[0], sE[1], sE[2], sE[3],
                                     sE[4], sE[5],  sE[6], 0,     0,     0,
                                     0,     0,      0,     0};
    std::vector<double> elparams = {0,
                                    0,
                                    lineDensity,
                                    double(orderOfDiffraction),
                                    abs(hvlam(designEnergy)),
                                    0,
                                    vls[0],
                                    vls[1],
                                    vls[2],
                                    vls[3],
                                    vls[4],
                                    vls[5],
                                    0,
                                    0,
                                    0,
                                    double(add_order)};
    ASSERT_DOUBLE_EQ(p1.getWidth(), width);
    ASSERT_DOUBLE_EQ(p1.getHeight(), height);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p1.getSurfaceParams(),
                              surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p1.getElementParameters(),
                              elparams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p1.getObjectParameters(),
                              objparams);

    glm::dmat4x4 correctInMat = {
        0.4921238928720304,     -0.29383657091479237, -0.8194352589787155, 0,
        0.87052322798627602,    0.16412181692266264,  0.46395402654396495, 0,
        -0.0018394566677098242, -0.941660288428391,   0.3365598276623748,  0,
        -2.9205704344671948,    1165.7356378981308,   -419.60594291729689, 1};
    glm::dmat4x4 correctOutMat = {
        0.4921238928720304,   0.87052322798627602, -0.0018394566677098242, 0,
        -0.29383657091479237, 0.16412181692266264, -0.941660288428391,     0,
        -0.8194352589787155,  0.46395402654396495, 0.3365598276623748,     0,
        0.13314042132848769,  5.8976402370902958,  1238.9440885860024,     1};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p1.getInMatrix(),
                              glmToVector16(correctInMat));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p1.getOutMatrix(),
                              glmToVector16(correctOutMat));
}

TEST(planeGraing, mount1) {
    double azimuthal = 61.142;
    double dist = 1245.71;

    int mount = 1;
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::vector<double> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int orderOfDiffraction = 1;
    int add_order = 0;

    std::vector<double> sE = {0, 0, 0, 0, 0, 0, 0};
    std::vector<double> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    double alpha = 1.3069025438933539;
    double beta = 1.317319261832787;
    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    ASSERT_DOUBLE_EQ(g_guparam.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(g_guparam.getAlpha(), g_guparam.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    add_order = 1;
    RAYX::PlaneGrating p2 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height, position, orientation,
        designEnergy, lineDensity, orderOfDiffraction, add_order, vls, sE);

    std::vector<double> surface = {0, 0, 0, 0, 1, 0, 0, -1,
                                   0, 0, 0, 0, 1, 0, 0, 0};
    std::vector<double> objparams = {width, height, sE[0], sE[1], sE[2], sE[3],
                                     sE[4], sE[5],  sE[6], 0,     0,     0,
                                     0,     0,      0,     0};
    std::vector<double> elparams = {0,
                                    0,
                                    lineDensity,
                                    double(orderOfDiffraction),
                                    abs(hvlam(designEnergy)),
                                    0,
                                    vls[0],
                                    vls[1],
                                    vls[2],
                                    vls[3],
                                    vls[4],
                                    vls[5],
                                    0,
                                    0,
                                    0,
                                    double(add_order)};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p2.getSurfaceParams(),
                              surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p2.getElementParameters(),
                              elparams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p2.getObjectParameters(),
                              objparams);

    glm::dmat4x4 correctInMat = {
        0.49177445965332872,    -0.41424900911211315, -0.76586920507506306, 0,
        0.87071579141593136,    0.23047793543536038,  0.43443518717569601,  0,
        -0.0034483926117463183, -0.88049854044574449, 0.47403631597305812,  0,
        -0.91630283962149228,   1089.5458368186687,   -590.86177917079829,  1};
    glm::dmat4x4 correctOutMat = {
        0.49177445965332872,  0.87071579141593136, -0.0034483926117463183, 0,
        -0.41424900911211315, 0.23047793543536038, -0.88049854044574449,   0,
        -0.76586920507506306, 0.43443518717569601, 0.47403631597305812,    0,
        -0.72894350458154844, 6.3727119490494584,  1239.4303003430443,     1};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p2.getInMatrix(),
                              glmToVector16(correctInMat));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p2.getOutMatrix(),
                              glmToVector16(correctOutMat));
}

TEST(planeGrating, higherOrderOfDiffraction) {
    // NEXT
    // higher order of diffraction, mount = 1
    double azimuthal = 61.142;
    double dist = 1245.71;

    int mount = 1;
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::vector<double> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int add_order = 0;

    std::vector<double> sE = {0, 0, 0, 0, 0, 0, 0};
    std::vector<double> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    double alpha = 1.3069025438933539;
    double beta = 1.3380699314613769;
    int orderOfDiffraction = 3;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    ASSERT_DOUBLE_EQ(g_guparam.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    RAYX::PlaneGrating p3 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height, position, orientation,
        designEnergy, lineDensity, orderOfDiffraction, add_order, vls, sE);

    std::vector<double> surface = {0, 0, 0, 0, 1, 0, 0, -1,
                                   0, 0, 0, 0, 1, 0, 0, 0};
    std::vector<double> objparams = {width, height, sE[0], sE[1], sE[2], sE[3],
                                     sE[4], sE[5],  sE[6], 0,     0,     0,
                                     0,     0,      0,     0};
    std::vector<double> elparams = {0,
                                    0,
                                    lineDensity,
                                    double(orderOfDiffraction),
                                    abs(hvlam(designEnergy)),
                                    0,
                                    vls[0],
                                    vls[1],
                                    vls[2],
                                    vls[3],
                                    vls[4],
                                    vls[5],
                                    0,
                                    0,
                                    0,
                                    double(add_order)};

    ASSERT_DOUBLE_EQ(p3.getWidth(), width);
    ASSERT_DOUBLE_EQ(p3.getHeight(), height);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p3.getSurfaceParams(),
                              surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p3.getElementParameters(),
                              elparams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p3.getObjectParameters(),
                              objparams);

    glm::dmat4x4 correctInMat = {
        0.49177445965332872,    -0.41424900911211315, -0.76586920507506306, 0,
        0.87071579141593136,    0.23047793543536038,  0.43443518717569601,  0,
        -0.0034483926117463183, -0.88049854044574449, 0.47403631597305812,  0,
        -0.91630283962149228,   1089.5458368186687,   -590.86177917079829,  1};
    glm::dmat4x4 correctOutMat = {
        0.49177445965332872,  0.87071579141593136, -0.0034483926117463183, 0,
        -0.41424900911211315, 0.23047793543536038, -0.88049854044574449,   0,
        -0.76586920507506306, 0.43443518717569601, 0.47403631597305812,    0,
        -0.72894350458154844, 6.3727119490494584,  1239.4303003430443,     1};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p3.getInMatrix(),
                              glmToVector16(correctInMat));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p3.getOutMatrix(),
                              glmToVector16(correctOutMat));
}

TEST(planeGrating, deviation) {
    // NEXT
    // mount = 0, use deviation angle, with higher order of diffraction
    double azimuthal = 61.142;
    double dist = 1245.71;

    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::vector<double> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int add_order = 0;
    int orderOfDiffraction = 3;

    std::vector<double> sE = {0, 0, 0, 0, 0, 0, 0};
    std::vector<double> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    int mount = 0;
    double alpha = 1.4473913414095938;
    double beta = 1.4777804849329026;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    ASSERT_DOUBLE_EQ(g_guparam.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    std::vector<double> surface = {0, 0, 0, 0, 1, 0, 0, -1,
                                   0, 0, 0, 0, 1, 0, 0, 0};
    std::vector<double> elparams = {0,
                                    0,
                                    lineDensity,
                                    double(orderOfDiffraction),
                                    abs(hvlam(designEnergy)),
                                    0,
                                    vls[0],
                                    vls[1],
                                    vls[2],
                                    vls[3],
                                    vls[4],
                                    vls[5],
                                    0,
                                    0,
                                    0,
                                    double(add_order)};
    std::vector<double> objparams = {width, height, sE[0], sE[1], sE[2], sE[3],
                                     sE[4], sE[5],  sE[6], 0,     0,     0,
                                     0,     0,      0,     0};

    RAYX::PlaneGrating p4 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height, position, orientation,
        designEnergy, lineDensity, orderOfDiffraction, add_order, vls, sE);

    ASSERT_DOUBLE_EQ(p4.getWidth(), width);
    ASSERT_DOUBLE_EQ(p4.getHeight(), height);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p4.getSurfaceParams(),
                              surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p4.getElementParameters(),
                              elparams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p4.getObjectParameters(),
                              objparams);

    glm::dmat4x4 correctInMat = {
        0.49210708552960752,    -0.30217566159372633, -0.81640705895524279, 0,
        0.87053249006934974,    0.16871726983154484,  0.46228526538734399,  0,
        -0.0019493858553223987, -0.93820272457554921, 0.3460806950608537,   0,
        -2.7836305461663344,    1161.4285160310073,   -431.46618264425604,  1};
    glm::dmat4x4 correctOutMat = {
        0.49210708552960752,  0.87053249006934974, -0.0019493858553223987, 0,
        -0.30217566159372633, 0.16871726983154484, -0.93820272457554921,   0,
        -0.81640705895524279, 0.46228526538734399, 0.3460806950608537,     0,
        0.073237329511777249, 5.9306512508972986,  1238.9720881547919,     1};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p4.getInMatrix(),
                              glmToVector16(correctInMat));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, p4.getOutMatrix(),
                              glmToVector16(correctOutMat));
}

TEST(PlaneGrating, testHvlam) {
    double hv = 100;
    double linedensity = 1000;
    double orderOfDiff = 1;
    double a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    ASSERT_DOUBLE_EQ(a, 0.01239852);
}

TEST(SphereGrating, testParams) {
    int mount = 0;
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    double width = 241.623;
    double height = 836.213;
    double deviation = 9.215;
    double incidence = 12.512;
    double azimuthal = 123.2;
    double distance = 2151.62;
    double entranceArm = 6134.1;
    double exitArm = 623.12;
    double designEnergy = 232.2;
    double linedensity = 432.2;
    int order = 1;
    std::vector<double> mis = {1.41, 5.3, 1.5, 0.2, 1.0, 1.4};
    std::vector<double> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};
    std::vector<double> sE = {0.1, 0.5, 0.1, 0.2, 0.5, 1, 3};

    double alpha = 1.4892226555787231;
    double beta = 1.4915379074397925;
    double radius = 1134.9852832410934;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, incidence, linedensity, designEnergy, order);
    g_guparam.calcGratingRadius(mount, degToRad(deviation), entranceArm,
                                exitArm);
    ASSERT_DOUBLE_EQ(g_guparam.getAlpha(), alpha);
    ASSERT_DOUBLE_EQ(g_guparam.getBeta(), beta);
    ASSERT_DOUBLE_EQ(g_guparam.getRadius(), radius);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), distance, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    RAYX::SphereGrating s1 = RAYX::SphereGrating(
        "spheregrating", mount, geometricalShape, width, height,
        g_guparam.getRadius(), position, orientation, designEnergy, linedensity,
        double(order), vls, sE);

    std::vector<double> quad = {1, 0, 0, 0, 1, 1, 0, -radius,
                                0, 0, 1, 0, 2, 0, 0, 0};
    std::vector<double> elparams = {0,
                                    0,
                                    linedensity,
                                    double(order),
                                    abs(hvlam(designEnergy)),
                                    0,
                                    vls[0],
                                    vls[1],
                                    vls[2],
                                    vls[3],
                                    vls[4],
                                    vls[5],
                                    0,
                                    0,
                                    0,
                                    0};
    std::vector<double> objparams = {width, height, sE[0], sE[1], sE[2], sE[3],
                                     sE[4], sE[5],  sE[6], 0,     0,     0,
                                     0,     0,      0,     0};

    ASSERT_DOUBLE_EQ(s1.getWidth(), width);
    ASSERT_DOUBLE_EQ(s1.getHeight(), height);
    ASSERT_DOUBLE_EQ(s1.getGratingMount(), mount);
    ASSERT_DOUBLE_EQ(s1.getLineDensity(), linedensity);
    ASSERT_DOUBLE_EQ(s1.getDesignEnergyMounting(), designEnergy);
    ASSERT_DOUBLE_EQ(s1.getOrderOfDiffraction(), double(order));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s1.getSurfaceParams(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s1.getElementParameters(),
                              elparams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s1.getObjectParameters(),
                              objparams);

    glm::dmat4x4 correctInMat = {
        0.61518377465346108,  0.40117039712668984,  -0.67868345778597972,  0,
        0.51231349500113244,  -0.85773975878762132, -0.042630846046937843, 0,
        -0.59923601881299182, -0.321472889469963,   -0.73319259072428489,  0,
        1287.9182027984093,   686.3874984413618,    1576.0518420541857,    1};
    glm::dmat4x4 correctOutMat = {
        0.61518377465346108,  0.51231349500113244,   -0.59923601881299182, 0,
        0.40117039712668984,  -0.85773975878762132,  -0.321472889469963,   0,
        -0.67868345778597972, -0.042630846046937843, -0.73319259072428489, 0,
        1.9755870403538665,   -3.8876049626932021,   2147.9714820131962,   1};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s1.getInMatrix(),
                              glmToVector16(correctInMat));
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s1.getOutMatrix(),
                              glmToVector16(correctOutMat));
}
