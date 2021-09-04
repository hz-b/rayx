#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Beamline/Beamline.h"
#include "Beamline/Objects/ReflectionZonePlate.h"
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

TEST(RZP, testdefaultParams) {
    int mount = 1;
    int geometricalShape = 0;
    int curvatureType = 0;
    int imageType = 0;
    int rzp_type = 0;
    int designType = 0;
    int elementOffsetType = 0;
    int additionalOrder = 1;
    double beta_in = 0;
    double width = 50.0;
    double height = 200.0;
    double deviation = 170;
    double grazingIncidence = 1; // auto == true -> taken from designAlpha
    double azimuthal = 0;
    double dist = 10000.0;
    double designEnergy = 100;
    double sourceEnergy = 100; // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1; // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double elementOffsetZ = 2;
    double fresnelOffset = 0;
    int icurv = 1;
    std::vector<double> mis = { 1,2,3, 0.001,0.002,0.003 };
    std::vector<double> sE = { 1,2,3,4,5,6,7 };
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate("RZP", geometricalShape, mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder, elementOffsetZ, fresnelOffset, beta_in, mis, sE, NULL, false);

    double alpha = 0.017453292519943295;
    double beta = 0.017453292519941554;
    double d_alpha = 0.017453292519943295;
    double d_beta = 0.017453292519943295;
    double wl = 12.39852;
    std::vector<double> quad = { 0,0,0,0, double(icurv),0,0,-1, 0,0,0,0, 4,0,0,0 }; // plane
    std::vector<double> correctElementParams = { 0, 0, 0, inm2eV / sourceEnergy,
                        sourceEnergy, designOrderOfDiffraction, orderOfDiffraction, fresnelOffset,
                        sEntrance, sExit, mEntrance, mExit,
                        rad(dAlpha), rad(dBeta), elementOffsetZ, double(additionalOrder) };
    std::vector<double> correctObjectParams = {width,height,sE[0],sE[1], 
                        sE[2],sE[3],sE[4],sE[5],
                        sE[6],alpha,0,0,
                        0,0,0,0};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getObjectParameters(), correctObjectParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSurfaceParams(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentParams(), mis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSlopeError(), sE);
    std::vector<double> zeros = { 0,0,0, 0,0,0 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentParams(), mis); // rzp stores misalignment in m_misalignment (to add to the m_inMatrix/m_outMatrix) AND tempMisalignment, because it has to be removed separated from the coordinate transformation matrices
    ASSERT_DOUBLE_EQ(rzp.getWidth(), width);
    ASSERT_DOUBLE_EQ(rzp.getHeight(), height);
    ASSERT_DOUBLE_EQ(rzp.getGratingMount(), mount);
    ASSERT_DOUBLE_EQ(rzp.getDesignEnergy(), designEnergy);
    ASSERT_DOUBLE_EQ(rzp.getOrderOfDiffraction(), orderOfDiffraction);
    ASSERT_DOUBLE_EQ(rzp.getDesignOrderOfDiffraction(), designOrderOfDiffraction);
    ASSERT_DOUBLE_EQ(rzp.getWaveLength(), wl);
    EXPECT_NEAR(rzp.getDesignAlphaAngle(), d_alpha, 0.000000001);
    EXPECT_NEAR(rzp.getDesignBetaAngle(), d_beta, 0.000000001);
    EXPECT_NEAR(rzp.getBeta(), beta, 0.00000001);
    EXPECT_NEAR(rzp.getAlpha(), alpha, 0.000000001);


}

TEST(RZP, testdefaultParamsElliptical) {
    int mount = 1;
    int geometricalShape = 1;
    int curvatureType = 0;
    int imageType = 0;
    int rzp_type = 0;
    int designType = 0;
    int elementOffsetType = 0;
    int additionalOrder = 1;
    double beta_in = 0;
    double width = 50.0;
    double height = 200.0;
    double deviation = 170;
    double grazingIncidence = 1; // auto == true -> taken from designAlpha
    double azimuthal = 0;
    double dist = 10000.0;
    double designEnergy = 100;
    double sourceEnergy = 100; // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1; // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double elementOffsetZ = 2;
    double fresnelOffset = 0;
    int icurv = 1;
    std::vector<double> mis = { 1,2,3, 0.001,0.002,0.003 };
    std::vector<double> sE = { 1,2,3,4,5,6,7 };
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate("RZP", geometricalShape, mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder, elementOffsetZ, fresnelOffset, beta_in, mis, sE, NULL, false);

    double alpha = 0.017453292519943295;
    double beta = 0.017453292519941554;
    double d_alpha = 0.017453292519943295;
    double d_beta = 0.017453292519943295;
    double wl = 12.39852;
    std::vector<double> quad = { 0,0,0,0, double(icurv),0,0,-1, 0,0,0,0, 4,0,0,0 }; // plane
    std::vector<double> correctElementParams = { 0, 0, 0, inm2eV / sourceEnergy,
                        sourceEnergy, designOrderOfDiffraction, orderOfDiffraction, fresnelOffset,
                        sEntrance, sExit, mEntrance, mExit,
                        rad(dAlpha), rad(dBeta), elementOffsetZ, double(additionalOrder) };
    std::vector<double> correctObjectParams = {-width,-height,sE[0],sE[1], 
                        sE[2],sE[3],sE[4],sE[5],
                        sE[6],alpha,0,0,
                        0,0,0,0};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getObjectParameters(), correctObjectParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSurfaceParams(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentParams(), mis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSlopeError(), sE);
    std::vector<double> zeros = { 0,0,0, 0,0,0 };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentParams(), mis); // rzp stores misalignment in m_misalignment (to add to the m_inMatrix/m_outMatrix) AND tempMisalignment, because it has to be removed separated from the coordinate transformation matrices
    ASSERT_DOUBLE_EQ(rzp.getWidth(), width);
    ASSERT_DOUBLE_EQ(rzp.getHeight(), height);
    ASSERT_DOUBLE_EQ(rzp.getGratingMount(), mount);
    ASSERT_DOUBLE_EQ(rzp.getDesignEnergy(), designEnergy);
    ASSERT_DOUBLE_EQ(rzp.getOrderOfDiffraction(), orderOfDiffraction);
    ASSERT_DOUBLE_EQ(rzp.getDesignOrderOfDiffraction(), designOrderOfDiffraction);
    ASSERT_DOUBLE_EQ(rzp.getWaveLength(), wl);
    EXPECT_NEAR(rzp.getDesignAlphaAngle(), d_alpha, 0.000000001);
    EXPECT_NEAR(rzp.getDesignBetaAngle(), d_beta, 0.000000001);
    EXPECT_NEAR(rzp.getBeta(), beta, 0.00000001);
    EXPECT_NEAR(rzp.getAlpha(), alpha, 0.000000001);


}

TEST(RZP, testParamsBeta1) {
    int mount = 0;
    int geometricShape = 0;
    int curvatureType = 0;
    int designType = 0;
    int elementOffsetType = 0;
    int additionalOrder = 0;
    double beta = 1;
    double width = 151.74;
    double height = 354.3;
    double deviation = 13.7;
    double grazingIncidence = 12.5;
    double azimuthal = 32.412;
    double dist = 5123.753;
    double designEnergy = 331.5;
    double sourceEnergy = 100; // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1; // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double elementOffsetZ = 0;
    double fresnelOffset = 12;
    int icurv = 1;
    std::vector<double> mis = { 0,0,0, 0,0,0 };
    std::vector<double> sE = { 1,3,4,5,6,7,9 };
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate("RZP", geometricShape, mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder, elementOffsetZ, fresnelOffset, beta, mis, sE, NULL, false);
    std::vector<double> quad = { 0,0,0,0, double(icurv),0,0,-1, 0,0,0,0, 4,0,0,0 }; // plane
    std::vector<double> correctMis = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    std::vector<double> correctElementParams = { 0, 0, 0, inm2eV / sourceEnergy,
                        sourceEnergy, designOrderOfDiffraction, orderOfDiffraction, fresnelOffset,
                        sEntrance, sExit, mEntrance, mExit,
                        rad(dAlpha), rad(dBeta), elementOffsetZ, double(additionalOrder) };
    std::vector<double> correctObjectParams = { width, height, sE[0], sE[1],
                        sE[2], sE[3], sE[4], sE[5], 
                        sE[6], rzp.getAlpha(), 0, 0,
                        0, 0, 0, 0};  
    double beta_correct = 0.017453292519943295;  
    EXPECT_DOUBLE_EQ(beta_correct, rzp.getBeta());
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getObjectParameters(), correctObjectParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSlopeError(), sE);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSurfaceParams(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseTempMisalignmentMatrix(), correctMis);

}

TEST(RZP, testParams) {
    int mount = 0;
    int geometricShape = 0;
    int curvatureType = 0;
    int designType = 0;
    int elementOffsetType = 0;
    int additionalOrder = 0;
    double beta = 0;
    double width = 151.74;
    double height = 354.3;
    double deviation = 13.7;
    double grazingIncidence = 12.5;
    double azimuthal = 32.412;
    double dist = 5123.753;
    double designEnergy = 331.5;
    double sourceEnergy = 100; // using this per default (auto==true)
    double orderOfDiffraction = 1;
    double designOrderOfDiffraction = -1;
    double dAlpha = 1; // degree
    double dBeta = 1;
    double sEntrance = 100;
    double sExit = 500;
    double mEntrance = 100;
    double mExit = 500;
    double shortRadius = 0;
    double longRadius = 0;
    double elementOffsetZ = 0;
    double fresnelOffset = 12;
    int icurv = 1;
    std::vector<double> mis = { 0,0,0, 0,0,0 };
    std::vector<double> sE = { 1,3,4,5,6,7,9 };
    RAYX::ReflectionZonePlate rzp = RAYX::ReflectionZonePlate("RZP", geometricShape, mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, additionalOrder, elementOffsetZ, fresnelOffset, beta, mis, sE, NULL, false);
    std::vector<double> quad = { 0,0,0,0, double(icurv),0,0,-1, 0,0,0,0, 4,0,0,0 }; // plane
    std::vector<double> correctMis = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    std::vector<double> correctElementParams = { 0, 0, 0, inm2eV / sourceEnergy,
                        sourceEnergy, designOrderOfDiffraction, orderOfDiffraction, fresnelOffset,
                        sEntrance, sExit, mEntrance, mExit,
                        rad(dAlpha), rad(dBeta), elementOffsetZ, double(additionalOrder) };
    std::vector<double> correctObjectParams = { width, height, sE[0], sE[1],
                        sE[2], sE[3], sE[4], sE[5], 
                        sE[6], rzp.getAlpha(), 0, 0,
                        0, 0, 0, 0};    
    
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getObjectParameters(), correctObjectParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getElementParameters(), correctElementParams);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSlopeError(), sE);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getSurfaceParams(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseMisalignmentMatrix(), correctMis);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseTempMisalignmentMatrix(), correctMis);

}