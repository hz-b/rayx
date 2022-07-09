#include "setupTests.h"
#if RUN_TEST_RZP

TEST(RZP, testdefaultParams) {
    RAYX::GratingMount mount = RAYX::GratingMount::Incidence;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    RAYX::CurvatureType curvatureType = RAYX::CurvatureType::Plane;
    auto imageType = RAYX::ImageType::Point2Point;
    int additionalOrder = 1;
    double beta_in = 0;
    double width = 50.0;
    double height = 200.0;
    double deviation = 170;
    double grazingIncidence = 1;  // auto == true -> taken from designAlpha
    double azimuthal = 0;
    double dist = 10000.0;
    double designEnergy = 100;
    double sourceEnergy = 100;  // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1;  // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double fresnelOffset = 0;

    std::array<double, 6> mis = {1, 2, 3, 0.001, 0.002, 0.003};
    std::array<double, 7> sE = {1, 2, 3, 4, 5, 6, 7};

    // alpha and beta calculated from user params
    double alpha = 0.017453292519943295;
    double beta = 0.017453292519941554;
    RAYX::GeometricUserParams rzp_guparams = RAYX::GeometricUserParams(
        mount, imageType, deviation, grazingIncidence, beta_in, sourceEnergy,
        designEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha,
        dBeta, mEntrance, mExit, sEntrance, sExit);
    CHECK_EQ(rzp_guparams.getAlpha(), alpha);
    CHECK_EQ(rzp_guparams.getBeta(), beta);

    RAYX::WorldUserParams rzp_param =
        RAYX::WorldUserParams(alpha, beta, azimuthal, dist, mis);
    glm::dvec4 position = rzp_param.calcPosition();
    glm::dmat4x4 orientation = rzp_param.calcOrientation();

    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate(
        "RZP", geometricalShape, curvatureType, width, height,
        rzp_param.getAzimuthalAngle(), position, orientation, designEnergy,
        orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance,
        sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder,
        fresnelOffset, sE, Material::Cu);

    double d_alpha = 0.017453292519943295;
    double d_beta = 0.017453292519943295;
    double wl = 12.39852;
    double icurv = 1;
    std::array<double, 4 * 4> quad = {0, 0, 0, 0, icurv, 0, 0,  -1,
                                      0, 0, 0, 0, 4,     0, 29, 0};  // plane
    std::array<double, 4 * 4> correctElementParams = {0,
                                                      0,
                                                      0,
                                                      hvlam(sourceEnergy),
                                                      double(curvatureType),
                                                      designOrderOfDiffraction,
                                                      orderOfDiffraction,
                                                      fresnelOffset,
                                                      sEntrance,
                                                      sExit,
                                                      mEntrance,
                                                      mExit,
                                                      degToRad(dAlpha),
                                                      degToRad(dBeta),
                                                      0,
                                                      double(additionalOrder)};
    std::array<double, 4 * 4> correctObjectParams = {
        width,
        height,
        sE[0],
        sE[1],
        sE[2],
        sE[3],
        sE[4],
        sE[5],
        sE[6],
        0,
        rzp_param.getAzimuthalAngle(),
        0,
        0,
        0,
        0,
        0};
    CHECK_EQ(rzp.getElementParameters(), correctElementParams);
    CHECK_EQ(rzp.getObjectParameters(), correctObjectParams);
    CHECK_EQ(rzp.getSurfaceParams(), quad);
    CHECK_EQ(rzp.getSlopeError(), sE);
    // std::vector<double> zeros = {0, 0, 0, 0, 0, 0};
    CHECK_EQ(rzp.getWidth(), width);
    CHECK_EQ(rzp.getHeight(), height);
    CHECK_EQ(rzp.getDesignEnergy(), designEnergy);
    CHECK_EQ(rzp.getOrderOfDiffraction(), orderOfDiffraction);
    CHECK_EQ(rzp.getDesignOrderOfDiffraction(), designOrderOfDiffraction);
    CHECK_EQ(rzp.getWaveLength(), wl);
    CHECK_EQ(rzp.getDesignAlphaAngle(), d_alpha, 0.000000001);
    CHECK_EQ(rzp.getDesignBetaAngle(), d_beta, 0.000000001);
}

TEST(RZP, testdefaultParamsElliptical) {
    RAYX::GratingMount mount = RAYX::GratingMount::Incidence;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::ELLIPTICAL;
    auto curvatureType = RAYX::CurvatureType::Plane;
    auto imageType = RAYX::ImageType::Point2Point;
    int additionalOrder = 1;
    double beta_in = 0;
    double width = 50.0;
    double height = 200.0;
    double deviation = 170;
    double grazingIncidence = 0;  // auto == true -> taken from designAlpha
    double azimuthal = 0;
    double dist = 10000.0;
    double designEnergy = 100;
    double sourceEnergy = 100;  // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1;  // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double fresnelOffset = 0;
    int icurv = 1;
    std::array<double, 6> mis = {1, 2, 3, 0.001, 0.002, 0.003};
    std::array<double, 7> sE = {1, 2, 3, 4, 5, 6, 7};

    double alpha = 0.017453292519943295;
    double beta = 0.017453292519941554;

    RAYX::GeometricUserParams rzp_guparams = RAYX::GeometricUserParams(
        mount, imageType, deviation, grazingIncidence, beta_in, sourceEnergy,
        designEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha,
        dBeta, mEntrance, mExit, sEntrance, sExit);
    CHECK_EQ(rzp_guparams.getAlpha(), alpha);
    CHECK_EQ(rzp_guparams.getBeta(), beta);

    RAYX::WorldUserParams rzp_param = RAYX::WorldUserParams(
        rzp_guparams.getAlpha(), rzp_guparams.getBeta(), azimuthal, dist, mis);
    glm::dvec4 position = rzp_param.calcPosition();
    glm::dmat4x4 orientation = rzp_param.calcOrientation();
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate(
        "RZP", geometricalShape, curvatureType, width, height,
        rzp_param.getAzimuthalAngle(), position, orientation, designEnergy,
        orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance,
        sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder,
        fresnelOffset, sE, Material::Cu);

    double d_alpha = 0.017453292519943295;
    double d_beta = 0.017453292519943295;
    double wl = 12.39852;
    std::array<double, 4 * 4> quad = {
        0, 0, 0, 0, double(icurv), 0, 0, -1, 0, 0, 0, 0, 4, 0, 29, 0};  // plane
    std::array<double, 4 * 4> correctElementParams = {0,
                                                      0,
                                                      0,
                                                      inm2eV / sourceEnergy,
                                                      double(curvatureType),
                                                      designOrderOfDiffraction,
                                                      orderOfDiffraction,
                                                      fresnelOffset,
                                                      sEntrance,
                                                      sExit,
                                                      mEntrance,
                                                      mExit,
                                                      degToRad(dAlpha),
                                                      degToRad(dBeta),
                                                      0,
                                                      double(additionalOrder)};
    std::array<double, 4 * 4> correctObjectParams = {
        -width,
        -height,
        sE[0],
        sE[1],
        sE[2],
        sE[3],
        sE[4],
        sE[5],
        sE[6],
        0,
        rzp_param.getAzimuthalAngle(),
        0,
        0,
        0,
        0,
        0};
    CHECK_EQ(rzp.getElementParameters(), correctElementParams);
    CHECK_EQ(rzp.getObjectParameters(), correctObjectParams);
    CHECK_EQ(rzp.getSurfaceParams(), quad);
    CHECK_EQ(rzp.getSlopeError(), sE);
    // std::vector<double> zeros = {0, 0, 0, 0, 0, 0};

    CHECK_EQ(rzp.getWidth(), -width);
    CHECK_EQ(rzp.getHeight(), -height);
    CHECK_EQ(rzp.getDesignEnergy(), designEnergy);
    CHECK_EQ(rzp.getOrderOfDiffraction(), orderOfDiffraction);
    CHECK_EQ(rzp.getDesignOrderOfDiffraction(), designOrderOfDiffraction);
    CHECK_EQ(rzp.getWaveLength(), wl);
    CHECK_EQ(rzp.getDesignAlphaAngle(), d_alpha, 0.000000001);
    CHECK_EQ(rzp.getDesignBetaAngle(), d_beta, 0.000000001);
}

TEST(RZP, testParams) {
    RAYX::GratingMount mount = RAYX::GratingMount::Deviation;
    RAYX::OpticalElement::GeometricalShape geometricalShape =
        RAYX::OpticalElement::GeometricalShape::RECTANGLE;
    auto imageType = RAYX::ImageType::Point2Point;
    auto curvatureType = RAYX::CurvatureType::Plane;
    int additionalOrder = 0;
    double beta_in = 0;
    double width = 151.74;
    double height = 354.3;
    double deviation = 13.7;
    double grazingIncidence = 12.5;
    double azimuthal = 32.412;
    double dist = 5123.753;
    double designEnergy = 331.5;
    double sourceEnergy = 100;  // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1;  // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double fresnelOffset = 12;

    std::array<double, 6> mis = {0, 0, 0, 0, 0, 0};
    std::array<double, 7> sE = {1, 3, 4, 5, 6, 7, 9};

    double alpha = 0.21816615649929119;
    double beta = 0.21816615649929122;
    RAYX::GeometricUserParams rzp_guparams = RAYX::GeometricUserParams(
        mount, imageType, deviation, grazingIncidence, beta_in, sourceEnergy,
        designEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha,
        dBeta, mEntrance, mExit, sEntrance, sExit);
    CHECK_EQ(rzp_guparams.getAlpha(), alpha);
    CHECK_EQ(rzp_guparams.getBeta(), beta);

    RAYX::WorldUserParams rzp_param =
        RAYX::WorldUserParams(rzp_guparams.getAlpha(), 0.21816615649929122,
                              degToRad(azimuthal), dist, mis);
    glm::dvec4 position = rzp_param.calcPosition();
    glm::dmat4x4 orientation = rzp_param.calcOrientation();
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate(
        "RZP", geometricalShape, curvatureType, width, height,
        rzp_param.getAzimuthalAngle(), position, orientation, designEnergy,
        orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance,
        sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder,
        fresnelOffset, sE, Material::Cu);

    std::array<double, 4 * 4> correctElementParams = {0,
                                                      0,
                                                      0,
                                                      inm2eV / designEnergy,
                                                      double(curvatureType),
                                                      designOrderOfDiffraction,
                                                      orderOfDiffraction,
                                                      fresnelOffset,
                                                      sEntrance,
                                                      sExit,
                                                      mEntrance,
                                                      mExit,
                                                      degToRad(dAlpha),
                                                      degToRad(dBeta),
                                                      0,
                                                      double(additionalOrder)};
    std::array<double, 4 * 4> correctObjectParams = {
        width,
        height,
        sE[0],
        sE[1],
        sE[2],
        sE[3],
        sE[4],
        sE[5],
        sE[6],
        0,
        rzp_param.getAzimuthalAngle(),
        0,
        0,
        0,
        0,
        0};
    glm::dmat4x4 correctInMatrix = glm::dmat4x4(
        0.84421568368313848, -0.52329819288792645, -0.11601241633394895, 0,
        0.53600361885290571, 0.82420440112787274, 0.18272171665687006, 0, 0,
        -0.21643961393810288, 0.97629600711993336, 0, 0, 1108.9831212341965,
        -5002.2995953687796, 1);
    glm::dmat4x4 correctOutMatrix = glm::dmat4x4(
        0.84421568368313848, 0.53600361885290571, 0, 0, -0.52329819288792645,
        0.82420440112787274, -0.21643961393810288, 0, -0.11601241633394895,
        0.18272171665687006, 0.97629600711993336, 0, 0, 0, 5123.7529999999997,
        1);
    CHECK_EQ(rzp.getObjectParameters(), correctObjectParams);
    CHECK_EQ(rzp.getElementParameters(), correctElementParams);
    CHECK_EQ(rzp.getInMatrix(), correctInMatrix);
    CHECK_EQ(rzp.getOutMatrix(), correctOutMatrix);
    CHECK_EQ(rzp.getSlopeError(), sE);
}

#endif