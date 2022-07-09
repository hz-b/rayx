#include <array>

#include "setupTests.h"
#if RUN_TEST_GRATINGS

TEST(planeGrating, testParams) {
    // user parameters
    double azimuthal = 61.142;
    double dist = 1245.71;

    RAYX::GratingMount mount = RAYX::GratingMount::Deviation;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::array<double, 6> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

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
    CHECK_EQ(g_guparam.getAlpha(), alpha);
    CHECK_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(g_guparam.getAlpha(), g_guparam.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};
    std::array<double, 6> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};
    RAYX::PlaneGrating p1 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height,
        g_params.getAzimuthalAngle(), position, orientation, designEnergy,
        lineDensity, orderOfDiffraction, add_order, vls, sE, Material::Cu);

    std::array<double, 4 * 4> surface = {0, 0, 0, 0, 1, 0, 0,  -1,
                                         0, 0, 0, 0, 1, 0, 29, 0};
    std::array<double, 4 * 4> objparams = {width,
                                           height,
                                           sE[0],
                                           sE[1],
                                           sE[2],
                                           sE[3],
                                           sE[4],
                                           sE[5],
                                           sE[6],
                                           0,
                                           g_params.getAzimuthalAngle(),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0};
    std::array<double, 4 * 4> elparams = {0,
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
    CHECK_EQ(p1.getWidth(), width);
    CHECK_EQ(p1.getHeight(), height);
    CHECK_EQ(p1.getSurfaceParams(), surface);
    CHECK_EQ(p1.getElementParameters(), elparams);
    CHECK_EQ(p1.getObjectParameters(), objparams);

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

    CHECK_EQ(p1.getInMatrix(), (correctInMat));
    CHECK_EQ(p1.getOutMatrix(), (correctOutMat));
}

TEST(planeGraing, mount1) {
    double azimuthal = 61.142;
    double dist = 1245.71;

    RAYX::GratingMount mount = RAYX::GratingMount::Incidence;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::array<double, 6> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int orderOfDiffraction = 1;
    int add_order = 0;

    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};
    std::array<double, 6> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    double alpha = 1.3069025438933539;
    double beta = 1.317319261832787;
    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    CHECK_EQ(g_guparam.getAlpha(), alpha);
    CHECK_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(g_guparam.getAlpha(), g_guparam.getBeta(),
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    add_order = 1;
    RAYX::PlaneGrating p2 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height,
        g_params.getAzimuthalAngle(), position, orientation, designEnergy,
        lineDensity, orderOfDiffraction, add_order, vls, sE, Material::Cu);

    std::array<double, 4 * 4> surface = {0, 0, 0, 0, 1, 0, 0,  -1,
                                         0, 0, 0, 0, 1, 0, 29, 0};
    std::array<double, 4 * 4> objparams = {width,
                                           height,
                                           sE[0],
                                           sE[1],
                                           sE[2],
                                           sE[3],
                                           sE[4],
                                           sE[5],
                                           sE[6],
                                           0,
                                           g_params.getAzimuthalAngle(),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0};
    std::array<double, 4 * 4> elparams = {0,
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
    CHECK_EQ(p2.getSurfaceParams(), surface);
    CHECK_EQ(p2.getElementParameters(), elparams);
    CHECK_EQ(p2.getObjectParameters(), objparams);

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

    CHECK_EQ(p2.getInMatrix(), correctInMat);
    CHECK_EQ(p2.getOutMatrix(), correctOutMat);
}

TEST(planeGrating, higherOrderOfDiffraction) {
    // NEXT
    // higher order of diffraction, mount = 1
    double azimuthal = 61.142;
    double dist = 1245.71;

    RAYX::GratingMount mount = RAYX::GratingMount::Incidence;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::array<double, 6> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int add_order = 0;

    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};
    std::array<double, 6> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    double alpha = 1.3069025438933539;
    double beta = 1.3380699314613769;
    int orderOfDiffraction = 3;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    CHECK_EQ(g_guparam.getAlpha(), alpha);
    CHECK_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    RAYX::PlaneGrating p3 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height,
        g_params.getAzimuthalAngle(), position, orientation, designEnergy,
        lineDensity, orderOfDiffraction, add_order, vls, sE, Material::Cu);

    std::array<double, 4 * 4> surface = {0, 0, 0, 0, 1, 0, 0,  -1,
                                         0, 0, 0, 0, 1, 0, 29, 0};
    std::array<double, 4 * 4> objparams = {width,
                                           height,
                                           sE[0],
                                           sE[1],
                                           sE[2],
                                           sE[3],
                                           sE[4],
                                           sE[5],
                                           sE[6],
                                           0,
                                           g_params.getAzimuthalAngle(),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0};
    std::array<double, 4 * 4> elparams = {0,
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

    CHECK_EQ(p3.getWidth(), width);
    CHECK_EQ(p3.getHeight(), height);
    CHECK_EQ(p3.getSurfaceParams(), surface);
    CHECK_EQ(p3.getElementParameters(), elparams);
    CHECK_EQ(p3.getObjectParameters(), objparams);

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

    CHECK_EQ(p3.getInMatrix(), (correctInMat));
    CHECK_EQ(p3.getOutMatrix(), (correctOutMat));
}

TEST(planeGrating, deviation) {
    // NEXT
    // mount = 0, use deviation angle, with higher order of diffraction
    double azimuthal = 61.142;
    double dist = 1245.71;

    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    double deviation = 12.4;
    double normalIncidence = 15.12;
    std::array<double, 6> mis = {5.212, 7.3, 0.35, 0.23, 0.011, 0.0006};

    double width = 125.23;
    double height = 51.56;
    double designEnergy = 100;
    double lineDensity = 812.2;
    int add_order = 0;
    int orderOfDiffraction = 3;

    std::array<double, 7> sE = {0, 0, 0, 0, 0, 0, 0};
    std::array<double, 6> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};

    RAYX::GratingMount mount = RAYX::GratingMount::Deviation;
    double alpha = 1.4473913414095938;
    double beta = 1.4777804849329026;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, normalIncidence, lineDensity, designEnergy,
        orderOfDiffraction);
    CHECK_EQ(g_guparam.getAlpha(), alpha);
    CHECK_EQ(g_guparam.getBeta(), beta);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), dist, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    std::array<double, 4 * 4> surface = {0, 0, 0, 0, 1, 0, 0,  -1,
                                         0, 0, 0, 0, 1, 0, 29, 0};
    std::array<double, 4 * 4> elparams = {0,
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
    std::array<double, 4 * 4> objparams = {width,
                                           height,
                                           sE[0],
                                           sE[1],
                                           sE[2],
                                           sE[3],
                                           sE[4],
                                           sE[5],
                                           sE[6],
                                           0,
                                           g_params.getAzimuthalAngle(),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0};

    RAYX::PlaneGrating p4 = RAYX::PlaneGrating(
        "planegrating", geometricalShape, width, height,
        g_params.getAzimuthalAngle(), position, orientation, designEnergy,
        lineDensity, orderOfDiffraction, add_order, vls, sE, Material::Cu);

    CHECK_EQ(p4.getWidth(), width);
    CHECK_EQ(p4.getHeight(), height);
    CHECK_EQ(p4.getSurfaceParams(), surface);
    CHECK_EQ(p4.getElementParameters(), elparams);
    CHECK_EQ(p4.getObjectParameters(), objparams);

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

    CHECK_EQ(p4.getInMatrix(), (correctInMat));
    CHECK_EQ(p4.getOutMatrix(), (correctOutMat));
}

TEST(PlaneGrating, testHvlam) {
    double hv = 100;
    double linedensity = 1000;
    double orderOfDiff = 1;
    double a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    CHECK_EQ(a, 0.01239852);
}

TEST(SphereGrating, testParams) {
    RAYX::GratingMount mount = RAYX::GratingMount::Deviation;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
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
    std::array<double, 6> mis = {1.41, 5.3, 1.5, 0.2, 1.0, 1.4};
    std::array<double, 6> vls = {2.1, 0.12, 12.2, 8.3, 5.1, 7.23};
    std::array<double, 7> sE = {0.1, 0.5, 0.1, 0.2, 0.5, 1, 3};

    double alpha = 1.4892226555787231;
    double beta = 1.4915379074397925;
    double radius = 1134.9852832410934;

    RAYX::GeometricUserParams g_guparam = RAYX::GeometricUserParams(
        mount, deviation, incidence, linedensity, designEnergy, order);
    g_guparam.calcGratingRadius(mount, degToRad(deviation), entranceArm,
                                exitArm);
    CHECK_EQ(g_guparam.getAlpha(), alpha);
    CHECK_EQ(g_guparam.getBeta(), beta);
    CHECK_EQ(g_guparam.getRadius(), radius);

    RAYX::WorldUserParams g_params =
        RAYX::WorldUserParams(alpha, beta, degToRad(azimuthal), distance, mis);
    glm::dvec4 position = g_params.calcPosition();
    glm::dmat4x4 orientation = g_params.calcOrientation();

    RAYX::SphereGrating s1 = RAYX::SphereGrating(
        "spheregrating", mount, geometricalShape, width, height,
        g_params.getAzimuthalAngle(), g_guparam.getRadius(), position,
        orientation, designEnergy, linedensity, double(order), vls, sE,
        Material::Cu);

    std::array<double, 4 * 4> quad = {1, 0, 0, 0, 1, 1, 0,  -radius,
                                      0, 0, 1, 0, 2, 0, 29, 0};
    std::array<double, 4 * 4> elparams = {0,
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
    std::array<double, 4 * 4> objparams = {width,
                                           height,
                                           sE[0],
                                           sE[1],
                                           sE[2],
                                           sE[3],
                                           sE[4],
                                           sE[5],
                                           sE[6],
                                           0,
                                           g_params.getAzimuthalAngle(),
                                           0,
                                           0,
                                           0,
                                           0,
                                           0};

    CHECK_EQ(s1.getWidth(), width);
    CHECK_EQ(s1.getHeight(), height);
    CHECK_EQ(static_cast<int>(s1.getGratingMount()), static_cast<int>(mount));
    CHECK_EQ(s1.getLineDensity(), linedensity);
    CHECK_EQ(s1.getDesignEnergyMounting(), designEnergy);
    CHECK_EQ(s1.getOrderOfDiffraction(), double(order));
    CHECK_EQ(s1.getSurfaceParams(), quad);
    CHECK_EQ(s1.getElementParameters(), elparams);
    CHECK_EQ(s1.getObjectParameters(), objparams);

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

    CHECK_EQ(s1.getInMatrix(), (correctInMat));
    CHECK_EQ(s1.getOutMatrix(), (correctOutMat));
}

#endif
