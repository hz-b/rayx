#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Model/Beamline/Beamline.h"
#include "Model/Beamline/Objects/Slit.h"
#include "UserParameter/WorldCoordinates.h"
#include "UserParameter/GeometricUserParams.h"
#include "Core.h"
#include "Ray.h"

// to compare eg vectors
//! Using the google test framework, check all elements of two containers
#define EXPECT_ITERABLE_BASE( PREDICATE, REFTYPE, TARTYPE, ref, target) \
    { \
    const REFTYPE& ref_(ref); \
    const TARTYPE& target_(target); \
    REFTYPE::const_iterator refIter = ref_.begin(); \
    TARTYPE::const_iterator tarIter = target_.begin(); \
    unsigned int i = 0; \
    while(refIter != ref_.end()) { \
        if ( tarIter == target_.end() ) { \
            ADD_FAILURE() << #target " has a smaller length than " #ref ; \
            break; \
        } \
        PREDICATE(* refIter, * tarIter) \
            << "Containers " #ref  " (refIter) and " #target " (tarIter)" \
               " differ at index " << i; \
        ++refIter; ++tarIter; ++i; \
    } \
    EXPECT_TRUE( tarIter == target_.end() ) \
        << #ref " has a smaller length than " #target ; \
    }


#define EXPECT_NEAR_ITERABLE_BASE( PREDICATE, REFTYPE, TARTYPE, ref, target, tolerance) \
    { \
    const REFTYPE& ref_(ref); \
    const TARTYPE& target_(target); \
    REFTYPE::const_iterator refIter = ref_.begin(); \
    TARTYPE::const_iterator tarIter = target_.begin(); \
    unsigned int i = 0; \
    while(refIter != ref_.end()) { \
        if ( tarIter == target_.end() ) { \
            ADD_FAILURE() << #target " has a smaller length than " #ref ; \
            break; \
        } \
        PREDICATE(* refIter, * tarIter, tolerance) \
            << "Containers " #ref  " (refIter) and " #target " (tarIter)" \
               " differ at index " << i; \
        ++refIter; ++tarIter; ++i; \
    } \
    EXPECT_TRUE( tarIter == target_.end() ) \
        << #ref " has a smaller length than " #target ; \
    }

//! Check that all elements of two same-type containers are equal
#define EXPECT_ITERABLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, TYPE, TYPE, ref, target )

//! Check that all elements of two different-type containers are equal
#define EXPECT_ITERABLE_EQ2( REFTYPE, TARTYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, REFTYPE, TARTYPE, ref, target )

//! Check that all elements of two same-type containers of doubles are equal
#define EXPECT_ITERABLE_DOUBLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_DOUBLE_EQ, TYPE, TYPE, ref, target )

// check that they are equal with a certain tolerance
#define EXPECT_ITERABLE_DOUBLE_NEAR( TYPE, ref, target, tolerance) \
    EXPECT_NEAR_ITERABLE_BASE( EXPECT_NEAR, TYPE, TYPE, ref, target, tolerance)


TEST(Slit, defaultParams) {
    int geometricalShape = 0;
    int beamstop = 0;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { 0,0,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { width,height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), 0);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), 0);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);

    std::vector<double> correctInMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -10000, 1 };
    std::vector<double> correctOutMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 10000, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);


}

TEST(Slit, rectangleBeamstop) {
    int geometricalShape = 0;
    int beamstop = 1;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { beamstopWidth / 2, beamstopHeight / 2,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { width,height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);

    std::vector<double> correctInMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -10000, 1 };
    std::vector<double> correctOutMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 10000, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);
}

TEST(Slit, ellipticalBeamstop) {
    int geometricalShape = 0;
    int beamstop = 2;
    double width = 20;
    double height = 2;
    double chi = 0;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { -beamstopWidth / 2, beamstopHeight / 2,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { width,height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), -beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);

    std::vector<double> correctInMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -10000, 1 };
    std::vector<double> correctOutMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 10000, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);
}


TEST(Slit, ellipticalSlitellipticalBeamstop) {
    int geometricalShape = 1;
    int beamstop = 2;
    double width = 24;
    double height = 3;
    double chi = 10;
    double dist = 10000;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { -beamstopWidth / 2, beamstopHeight / 2,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { -width,-height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), -beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);

    std::vector<double> correctInMat = { 0.98480775301220802, -0.17364817766693033, 0, 0, 0.17364817766693033, 0.98480775301220802, 0, 0, 0, 0, 1, 0, 0, 0, -10000, 1 };
    std::vector<double> correctOutMat = { 0.98480775301220802, 0.17364817766693033, 0, 0, -0.17364817766693033, 0.98480775301220802, 0, 0, 0, 0, 1, 0, 0, 0, 10000, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);
}


TEST(Slit, ellipticalSlitrectangleBeamstop) {
    int geometricalShape = 1;
    int beamstop = 1;
    double width = 24;
    double height = 3;
    double chi = 0;
    double dist = 1234;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { beamstopWidth / 2, beamstopHeight / 2,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { -width,-height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), beamstopHeight);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), beamstopWidth);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);

    std::vector<double> correctInMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -1234, 1 };
    std::vector<double> correctOutMat = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1234, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);
}


TEST(Slit, ellipticalSlitNoBeamstop) {
    int geometricalShape = 1;
    int beamstop = 0;
    double width = 24;
    double height = 3;
    double chi = 12;
    double dist = 1201;
    double beamstopWidth = 20;
    double beamstopHeight = 1;
    double energy = 100;
    std::vector<double> misalignment = { 0,0,0, 0,0,0 };

    RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(0, 0, degToRad(chi), dist, misalignment);
    //RAYX::Slit s = RAYX::Slit("slitt", geometricalShape, beamstop, width, height, chi, dist, beamstopWidth, beamstopHeight, energy, misalignment, nullptr, true);
    RAYX::Slit s = RAYX::Slit("slit", geometricalShape, beamstop, width, height, w_coord.calcPosition(), w_coord.calcOrientation(), beamstopWidth, beamstopHeight, energy);

    double wavelength = 12.39852;
    std::vector<double> correctElementParams = { 0,0,0,0, wavelength,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> correctObjectParams = { -width,-height,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    std::vector<double> surface = { 0,0,0,0, 0,0,0,0, 0,0,0,-1, 3,0,0,0 };
    ASSERT_DOUBLE_EQ(s.getCentralBeamstop(), beamstop);
    ASSERT_DOUBLE_EQ(s.getBeamstopHeight(), 0);
    ASSERT_DOUBLE_EQ(s.getBeamstopWidth(), 0);
    ASSERT_DOUBLE_EQ(s.getHeight(), height);
    ASSERT_DOUBLE_EQ(s.getWidth(), width);
    ASSERT_DOUBLE_EQ(s.getWaveLength(), wavelength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getSurfaceParams(), surface);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getObjectParameters(), correctObjectParams);


    std::vector<double> correctInMat = { 0.97814760073380569, -0.20791169081775931, 0, 0, 0.20791169081775931, 0.97814760073380569, 0, 0, 0, 0, 1, 0, 0, 0, -1201, 1 };
    std::vector<double> correctOutMat = { 0.97814760073380569, 0.20791169081775931, 0, 0, -0.20791169081775931, 0.97814760073380569, 0, 0, 0, 0, 1, 0, 0, 0, 1201, 1 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getInMatrix(), correctInMat);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, s.getOutMatrix(), correctOutMat);
}