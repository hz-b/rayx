#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Beamline/Beamline.h"
#include "Beamline/ReflectionZonePlate.h"
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
    int curvatureType = 0;
    int imageType = 0;
    int rzp_type = 0;
    int designType = 0;
    int elementOffsetType = 0;
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
    std::vector<double> mis = {1,2,3, 0.001,0.002,0.003};
    RAY::ReflectionZonePlate rzp = RAY::ReflectionZonePlate ("RZP", mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, elementOffsetZ, beta_in, mis, NULL);
    
    double alpha = 0.017453292519943295;
    double beta = 0.017453292519941554;
    double d_alpha = 0.017453292519943295;
    double d_beta = 0.017453292519943295;
    double wl = 12.39852;
    double order = -1;
    double d_order = -1;
    std::vector<double> quad = {0,0,0,0, width,0,0,-1, height,0,0,0, 4,0,0,0}; // plane

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getQuadric(), quad);
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentParams(), mis);
    std::vector<double> zeros = {0,0,0, 0,0,0};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentParams(), mis); // rzp stores misalignment in m_misalignment (to add to the m_inMatrix/m_outMatrix) AND tempMisalignment, because it has to be removed separated from the coordinate transformation matrices
    ASSERT_DOUBLE_EQ (rzp.getWidth(),  width);
    ASSERT_DOUBLE_EQ (rzp.getHeight(),  height);
    ASSERT_DOUBLE_EQ (rzp.getGratingMount(), mount);
    ASSERT_DOUBLE_EQ (rzp.getDesignEnergy(), designEnergy);
    ASSERT_DOUBLE_EQ (rzp.getOrderOfDiffraction(), order);
    ASSERT_DOUBLE_EQ (rzp.getDesignOrderOfDiffraction(), d_order);
    ASSERT_DOUBLE_EQ (rzp.getWaveLength(), wl);
    EXPECT_NEAR(rzp.getDesignAlphaAngle(), d_alpha, 0.000000001);
    EXPECT_NEAR(rzp.getDesignBetaAngle(), d_beta, 0.000000001);
    EXPECT_NEAR (rzp.getBeta(), beta, 0.00000001);
    EXPECT_NEAR(rzp.getAlpha(), alpha, 0.000000001);
    
    
}

TEST(RZP, testParams) {
    int mount = 0;
    int curvatureType = 0;
    int designType = 0;
    int elementOffsetType = 0;
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
    std::vector<double> mis = {0,0,0, 0,0,0};
    RAY::ReflectionZonePlate rzp = RAY::ReflectionZonePlate ("RZP", mount, curvatureType, designType, elementOffsetType, width, height, deviation, grazingIncidence, azimuthal, dist, designEnergy, sourceEnergy, orderOfDiffraction, designOrderOfDiffraction, dAlpha, dBeta, sEntrance, sExit, mEntrance, mExit, shortRadius, longRadius, elementOffsetZ, beta, mis, NULL);
    std::vector<double> correctMis = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getMisalignmentMatrix(), correctMis); 
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getTempMisalignmentMatrix(), correctMis); 
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseMisalignmentMatrix(), correctMis); 
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, rzp.getInverseTempMisalignmentMatrix(), correctMis); 

}