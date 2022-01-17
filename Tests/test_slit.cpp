#include "setupTests.h"
#if RUN_TEST_SLIT

TEST(Slit, defaultParams) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    int beamstop = 0;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {
        0, 0, 0, 0, wavelength, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> correctObjectParams = {
        width, height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), 0);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), 0);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, rectangleBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    int beamstop = 1;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {beamstopWidth / 2,
                                                      beamstopHeight / 2,
                                                      0,
                                                      0,
                                                      wavelength,
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
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::RECTANGLE;
    int beamstop = 2;
    double width = 20;
    double height = 2;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {-beamstopWidth / 2,
                                                      beamstopHeight / 2,
                                                      0,
                                                      0,
                                                      wavelength,
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
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), -beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,      0,
                                              0, 0, 1, 0, 0, 0, -10000, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                               0, 0, 1, 0, 0, 0, 10000, 1};
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitellipticalBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    int beamstop = 2;
    double width = 24;
    double height = 3;
    double chi = 10;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {-beamstopWidth / 2,
                                                      beamstopHeight / 2,
                                                      0,
                                                      0,
                                                      wavelength,
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
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), -beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), -height);
    ASSERT_DOUBLE_EQ(s.getWidth(), -width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

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
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitrectangleBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    int beamstop = 1;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 1234;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {beamstopWidth / 2,
                                                      beamstopHeight / 2,
                                                      0,
                                                      0,
                                                      wavelength,
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
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), -height);
    ASSERT_DOUBLE_EQ(s.getWidth(), -width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

    std::array<double, 4 * 4> correctInMat = {1, 0, 0, 0, 0, 1, 0,     0,
                                              0, 0, 1, 0, 0, 0, -1234, 1};
    std::array<double, 4 * 4> correctOutMat = {1, 0, 0, 0, 0, 1, 0,    0,
                                               0, 0, 1, 0, 0, 0, 1234, 1};
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalSlitNoBeamstop) {
    RAYX::Geometry::GeometricalShape geometricalShape =
        RAYX::Geometry::GeometricalShape::ELLIPTICAL;
    int beamstop = 0;
    double width = 24;
    double height = 3;
    double chi = 12;
    double dist = 1201;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::array<double, 6> misalignment = {0, 0, 0, 0, 0, 0};

    RAYX::WorldUserParams w_coord =
        RAYX::WorldUserParams(0, 0, degToRad(chi), dist, misalignment);
    // RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width,
    // height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment,
    // nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height,
                              w_coord.calcPosition(), w_coord.calcOrientation(),
                              beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::array<double, 4 * 4> correctElementParams = {
        0, 0, 0, 0, wavelength, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> correctObjectParams = {
        -width, -height, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<double, 4 * 4> surface = {0, 0, 0, 0,  0, 0, 0, 0,
                                         0, 0, 0, -1, 3, 0, 0, 0};
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), 0);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), 0);
    ASSERT_DOUBLE_EQ(s.getHeight(), -height);
    ASSERT_DOUBLE_EQ(s.getWidth(), -width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getElementParameters(),
                                  correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getObjectParameters(),
                                  correctObjectParams);

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
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ_ARR(4 * 4, s.getOutMatrix(), correctOutMat);
}

#endif