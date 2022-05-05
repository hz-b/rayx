#include "setupTests.h"
#if RUN_TEST_SLIT

TEST(Slit, defaultParams) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::None;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);

    std::array<double, 4 * 4> correctElementParams = {0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> correctObjectParams = {
        width, height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), 0);
    CHECK_EQ(s.getBeamstopWidth(), 0);
    CHECK_EQ(s.getHeight(), height);
    CHECK_EQ(s.getWidth(), width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

TEST(Slit, rectangleBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::Rectangle;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);

    std::array<double, 4 * 4> correctElementParams = {beamstopWidth / 2,
                                                      beamstopHeight / 2,
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
                                                      0,
                                                      0,
                                                      0};
    std::array<double, 4 * 4> correctObjectParams = {
        width, height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), beamstopHeight);
    CHECK_EQ(s.getBeamstopWidth(), beamstopWidth);
    CHECK_EQ(s.getHeight(), height);
    CHECK_EQ(s.getWidth(), width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::Elliptical;
    double width = 20;
    double height = 2;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);

    std::array<double, 4 * 4> correctElementParams = {-beamstopWidth / 2,
                                                      beamstopHeight / 2,
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
                                                      0,
                                                      0,
                                                      0};
    std::array<double, 4 * 4> correctObjectParams = {
        width, height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), beamstopHeight);
    CHECK_EQ(s.getBeamstopWidth(), -beamstopWidth);
    CHECK_EQ(s.getHeight(), height);
    CHECK_EQ(s.getWidth(), width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitellipticalBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::Elliptical;
    double width = 24;
    double height = 3;
    double chi = 10;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);

    std::array<double, 4 * 4> correctElementParams = {-beamstopWidth / 2,
                                                      beamstopHeight / 2,
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
                                                      0,
                                                      0,
                                                      0};
    std::array<double, 4 * 4> correctObjectParams = {
        -width, -height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), beamstopHeight);
    CHECK_EQ(s.getBeamstopWidth(), -beamstopWidth);
    CHECK_EQ(s.getHeight(), -height);
    CHECK_EQ(s.getWidth(), -width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {0.98480775301220802,
                                              -0.17364817766693033,
                                              0,
                                              0,
                                              0.17364817766693033,
                                              0.98480775301220802,
                                              0,
                                              0,
                                              0,
                                              0,
                                              1,
                                              0,
                                              0,
                                              0,
                                              -10000,
                                              1};
    std::array<double, 4 * 4> correctOutMat = {0.98480775301220802,
                                               0.17364817766693033,
                                               0,
                                               0,
                                               -0.17364817766693033,
                                               0.98480775301220802,
                                               0,
                                               0,
                                               0,
                                               0,
                                               1,
                                               0,
                                               0,
                                               0,
                                               10000,
                                               1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitrectangleBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::Rectangle;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 1234;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);

    std::array<double, 4 * 4> correctElementParams = {beamstopWidth / 2,
                                                      beamstopHeight / 2,
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
                                                      0,
                                                      0,
                                                      0};
    std::array<double, 4 * 4> correctObjectParams = {
        -width, -height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), beamstopHeight);
    CHECK_EQ(s.getBeamstopWidth(), beamstopWidth);
    CHECK_EQ(s.getHeight(), -height);
    CHECK_EQ(s.getWidth(), -width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                              0, 0, 1, 0, 0, 0, -1234, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,    0,
                                               0, 0, 1, 0, 0, 0, 1234, 1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitNoBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    RAYX::CentralBeamstop beamstop = RAYX::CentralBeamstop::None;
    double width = 24;
    double height = 3;
    double chi = 12;
    double dist = 1201;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight);

    std::array<double, 4 * 4> correctElementParams = {0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> correctObjectParams = {
        -width, -height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    CHECK_EQ(static_cast<int>(s.getCentralBeamstop()),
             static_cast<int>(beamstop));
    CHECK_EQ(s.getBeamstopHeight(), 0);
    CHECK_EQ(s.getBeamstopWidth(), 0);
    CHECK_EQ(s.getHeight(), -height);
    CHECK_EQ(s.getWidth(), -width);
    CHECK_EQ(s.getElementParameters(), correctElementParams);
    CHECK_EQ(s.getSurfaceParams(), surface);
    CHECK_EQ(s.getObjectParameters(), correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {0.97814760073380569,
                                              -0.20791169081775931,
                                              0,
                                              0,
                                              0.20791169081775931,
                                              0.97814760073380569,
                                              0,
                                              0,
                                              0,
                                              0,
                                              1,
                                              0,
                                              0,
                                              0,
                                              -1201,
                                              1};
    std::array<double, 4 * 4> correctOutMat = {0.97814760073380569,
                                               0.20791169081775931,
                                               0,
                                               0,
                                               -0.20791169081775931,
                                               0.97814760073380569,
                                               0,
                                               0,
                                               0,
                                               0,
                                               1,
                                               0,
                                               0,
                                               0,
                                               1201,
                                               1};
    CHECK_EQ(s.getInMatrix(), correctInMat);
    CHECK_EQ(s.getOutMatrix(), correctOutMat);
}

#endif