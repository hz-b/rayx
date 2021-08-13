#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Beamline/Beamline.h"
#include "Beamline/Objects/PlaneMirror.h"
#include "Beamline/Objects/SphereMirror.h"
#include "Beamline/Objects/Ellipsoid.h"
#include "Core.h"
#include "Ray.h"

using ::testing::ElementsAre;

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

//! Check that all elements of two same-type containers are equal
#define EXPECT_ITERABLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, TYPE, TYPE, ref, target )

//! Check that all elements of two different-type containers are equal
#define EXPECT_ITERABLE_EQ2( REFTYPE, TARTYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, REFTYPE, TARTYPE, ref, target )

//! Check that all elements of two same-type containers of doubles are equal
#define EXPECT_ITERABLE_DOUBLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_DOUBLE_EQ, TYPE, TYPE, ref, target )

TEST(PlaneMirror, testSimpleParams) {
    //arrange
    //act
    //assert
    double width = 68.12;
    double height = 123.6;
    double incidenceAngle = 13.2;
    double azimuthalAngle = 0.0;
    double dist = 12005;
    std::vector<double> mis = { 0,0,0,0,0,0 };
    std::vector<double> sE = { 0,0,0,0,0, 0,0 };
    RAYX::PlaneMirror plM = RAYX::PlaneMirror("planemirror",width, height, incidenceAngle, azimuthalAngle, dist, mis, sE, NULL, false); // {1,2,3,0.01,0.02,0.03}

    ASSERT_DOUBLE_EQ(plM.getWidth(), width);
    ASSERT_DOUBLE_EQ(plM.getHeight(), height);
    ASSERT_DOUBLE_EQ(plM.getAlpha(), rad(incidenceAngle));
    ASSERT_DOUBLE_EQ(plM.getBeta(), rad(incidenceAngle));
    ASSERT_DOUBLE_EQ(plM.getChi(), rad(azimuthalAngle));
    ASSERT_DOUBLE_EQ(plM.getDistanceToPreceedingElement(), dist);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, mis, plM.getMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, plM.getSlopeError());
}

TEST(PlaneMirror, testAdvancedParams) {
    double width = 124.12;
    double height = 26;
    double incidenceAngle = 23;
    double azimuthalAngle = 8.2;
    double dist = 12005;
    std::vector<double> mis = { 1,2,3,0.01,0.02,0.03 };
    std::vector<double> sE = { 0.1,0.2,0.3,0.4,0.5, 0.6,0.7 };
    RAYX::PlaneMirror plM = RAYX::PlaneMirror("planemirror",width, height, incidenceAngle, azimuthalAngle, dist, mis, sE, NULL, false); // {1,2,3,0.01,0.02,0.03}

    ASSERT_DOUBLE_EQ(plM.getWidth(), width);
    ASSERT_DOUBLE_EQ(plM.getHeight(), height);
    ASSERT_DOUBLE_EQ(plM.getAlpha(), rad(incidenceAngle));
    ASSERT_DOUBLE_EQ(plM.getBeta(), rad(incidenceAngle));
    ASSERT_DOUBLE_EQ(plM.getChi(), rad(azimuthalAngle));
    ASSERT_DOUBLE_EQ(plM.getDistanceToPreceedingElement(), dist);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, mis, plM.getMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, plM.getSlopeError());
}

TEST(Sphere, testParams) {
    double width = 21.62;
    double height = 813.12;
    double incidence = 12.75;
    double azimuthal = 41.2;
    double dist = 12.12;
    double entranceArmLength = 12.7;
    double exitArmLength = 123.1;
    std::vector<double> misalignmentParams = { 10,51,2,0.1,5,0.241 };
    std::vector<double> sE = { 0.7,0.5,0.3,0.7,0.3, 3,2 };
    RAYX::SphereMirror sM = RAYX::SphereMirror("spheremirror", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, misalignmentParams, sE, NULL, false); 

    double radius = 104.326518296; // from old RAY, rounded to 9 digits
    ASSERT_DOUBLE_EQ(sM.getWidth(), width);
    ASSERT_DOUBLE_EQ(sM.getHeight(), height);
    EXPECT_NEAR(sM.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(sM.getAlpha(), rad(incidence));
    ASSERT_DOUBLE_EQ(sM.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(sM.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(sM.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(sM.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(sM.getEntranceArmLength(), entranceArmLength);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, misalignmentParams, sM.getMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, sM.getSlopeError());
}

TEST(Ellips, testParamsCSCurvature) {
    double width = 123.51;
    double height = 85.234;
    double incidence = 31.513;
    double azimuthal = 51.2452;
    double dist = 12451.51535;
    double entranceArmLength = 6421.7234;
    double exitArmLength = 1762.622123;
    int coordSys = 0; // {CS_CURVATURE, CS_MIRROR};
    int figRot = 0; // { FR_YES, FR_PLANE, FR_A11};
    double a_11 = 12.62;
    std::vector<double>  mis = { 12,72,1.12, 0.1,0.7341,2.5 };
    std::vector<double> sE = { 1,2,3,4,5,6,7 };
    RAYX::Ellipsoid e = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 

    // in old RAY
    double shortHalfAxis = 1758.53601375;
    double longHalfAxis = 4092.1727615;
    double surfaceCenterY0 = 1365.026842783936; // actually neg. depending on which calc in old RAY is used
    double surfaceCenterZ0 = 2579.914108547892;
    double tangentAngle = 0.3358074867515476;
    double halfAxisC = 1758.536013753882;
    double radius = -1365.026842783936;
    double alpha = 0.2141981204294254;
    double a33 = 0.1846692617906655;
    double a34 = 476.4308339088623;
    double a44 = -4.65661e-10;
    double alphaE = 0.2141981204294254;
    double betaE = 0.8858130939325208;
    std::vector<double> correctTempMis = { 0,0,0,0,0,0 };

    ASSERT_DOUBLE_EQ(e.getWidth(), width);
    ASSERT_DOUBLE_EQ(e.getHeight(), height);
    EXPECT_NEAR(e.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e.getMz0(), surfaceCenterZ0, 0.0000001);
    EXPECT_NEAR(e.getHalfAxisC(), halfAxisC, 0.0000001);
    EXPECT_NEAR(e.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e.getSlopeError());

    figRot = 1;
    RAYX::Ellipsoid e2 = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 
    halfAxisC = INFINITY;

    ASSERT_DOUBLE_EQ(e2.getWidth(), width);
    ASSERT_DOUBLE_EQ(e2.getHeight(), height);
    EXPECT_NEAR(e2.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e2.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e2.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e2.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e2.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e2.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e2.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e2.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e2.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e2.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e2.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e2.getMz0(), surfaceCenterZ0, 0.0000001);
    ASSERT_DOUBLE_EQ(e2.getHalfAxisC(), halfAxisC);
    EXPECT_NEAR(e2.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e2.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e2.getSlopeError());

    figRot = 2;
    RAYX::Ellipsoid e3 = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 
    halfAxisC = 495.0186818473859;

    ASSERT_DOUBLE_EQ(e3.getWidth(), width);
    ASSERT_DOUBLE_EQ(e3.getHeight(), height);
    EXPECT_NEAR(e3.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e3.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e3.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e3.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e3.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e3.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e3.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e3.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e3.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e3.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e3.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e3.getMz0(), surfaceCenterZ0, 0.0000001);
    EXPECT_NEAR(e3.getHalfAxisC(), halfAxisC, 0.00000001);
    EXPECT_NEAR(e3.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e3.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e3.getSlopeError());

}

TEST(Ellips, testParamsCSMirror) {
    double width = 123.51;
    double height = 85.234;
    double incidence = 31.513;
    double azimuthal = 51.2452;
    double dist = 12451.51535;
    double entranceArmLength = 6421.7234;
    double exitArmLength = 1762.622123;
    int coordSys = 1; // {CS_CURVATURE, CS_MIRROR};
    int figRot = 0; // { FR_YES, FR_PLANE, FR_A11};
    double a_11 = 12.62;
    std::vector<double>  mis = { 12,72,1.12, 0.1,0.7341,2.5 };
    std::vector<double> sE = { 7,6,5,4,3,2,1 };
    RAYX::Ellipsoid e = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 

    // in old RAY
    double shortHalfAxis = 1758.53601375;
    double longHalfAxis = 4092.1727615;
    double surfaceCenterY0 = 1365.026842783936; // actually neg. depending on which calc in old RAY is used
    double surfaceCenterZ0 = 2579.914108547892;
    double tangentAngle = 0.3358074867515476;
    double halfAxisC = 1758.536013753882;
    double radius = -1365.026842783936;
    double alpha = 0.2141981204294254;
    double a33 = 0.1846692617906655;
    double a34 = 476.4308339088623;
    double a44 = -4.65661e-10;
    double alphaE = 0.2141981204294254;
    double betaE = 0.8858130939325208;
    std::vector<double> correctTempMis = { 0,0,0,0,0,tangentAngle };

    ASSERT_DOUBLE_EQ(e.getWidth(), width);
    ASSERT_DOUBLE_EQ(e.getHeight(), height);
    EXPECT_NEAR(e.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e.getMz0(), surfaceCenterZ0, 0.0000001);
    EXPECT_NEAR(e.getHalfAxisC(), halfAxisC, 0.0000001);
    EXPECT_NEAR(e.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e.getSlopeError());

    figRot = 1;
    RAYX::Ellipsoid e2 = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 
    halfAxisC = INFINITY;

    ASSERT_DOUBLE_EQ(e2.getWidth(), width);
    ASSERT_DOUBLE_EQ(e2.getHeight(), height);
    EXPECT_NEAR(e.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e2.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e2.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e2.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e2.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e2.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e2.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e2.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e2.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e2.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e2.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e2.getMz0(), surfaceCenterZ0, 0.0000001);
    ASSERT_DOUBLE_EQ(e2.getHalfAxisC(), halfAxisC);
    EXPECT_NEAR(e2.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e2.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e2.getSlopeError());

    figRot = 2;
    RAYX::Ellipsoid e3 = RAYX::Ellipsoid("ellipsoid", width, height, incidence, azimuthal, dist, entranceArmLength, exitArmLength, coordSys, figRot, a_11, mis, sE, NULL, false); 
    halfAxisC = 495.0186818473859;

    ASSERT_DOUBLE_EQ(e3.getWidth(), width);
    ASSERT_DOUBLE_EQ(e3.getHeight(), height);
    EXPECT_NEAR(e3.getRadius(), radius, 0.00000001);
    ASSERT_DOUBLE_EQ(e3.getIncidenceAngle(), rad(incidence));
    ASSERT_DOUBLE_EQ(e3.getBeta(), rad(incidence));
    ASSERT_DOUBLE_EQ(e3.getChi(), rad(azimuthal));
    ASSERT_DOUBLE_EQ(e3.getDistanceToPreceedingElement(), dist);
    ASSERT_DOUBLE_EQ(e3.getExitArmLength(), exitArmLength);
    ASSERT_DOUBLE_EQ(e3.getEntranceArmLength(), entranceArmLength);
    EXPECT_NEAR(e3.getTangentAngle(), tangentAngle, 0.00000001);
    EXPECT_NEAR(e3.getShortHalfAxisB(), shortHalfAxis, 0.0000001);
    EXPECT_NEAR(e3.getLongHalfAxisA(), longHalfAxis, 0.0000001);
    EXPECT_NEAR(e3.getMy0(), surfaceCenterY0, 0.0000001);
    EXPECT_NEAR(e3.getMz0(), surfaceCenterZ0, 0.0000001);
    EXPECT_NEAR(e3.getHalfAxisC(), halfAxisC, 0.00000001);
    EXPECT_NEAR(e3.getAlpha(), alphaE, 0.000001);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correctTempMis, e3.getTempMisalignmentParams());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, sE, e3.getSlopeError());

}