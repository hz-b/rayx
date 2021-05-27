#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Beamline/Beamline.h"
#include "Beamline/PlaneMirror.h"
#include "Beamline/SphereMirror.h"
#include "Beamline/Ellipsoid.h"
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

TEST (Quadric, MatrixProduct) {
    std::vector<double> A = {23,6,12,7, 8.1,53.1,4.1,0.51, 4.1,5.1,2.7,3.2, 12.5,12.9,4.2,9.1};
    std::vector<double> B = {9.2,6,2.4,72.1, 8.6,21.3,78.2,12, 2.6,2.6,6.52,7.7, 2.25,62.1,5,2.2};
    std::vector<double> result =  getMatrixProductAsVector(A, B);
    std::vector<double> correct = {1171.29 , 1316.13 , 444.29999999999995 , 731.25 , 840.9499999999999 , 1736.25 , 452.07000000000005 , 430.503 , 203.84199999999998 , 286.242 , 91.804 , 110.46000000000001 , 602.76 , 3364.8900000000003 , 304.34999999999997 , 83.441};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, correct, result);
}

TEST (Quadric, testMisalignment) {
    double width = 68.12;
    double height = 123.6;
    double incidenceAngle = 13.2;
    double azimuthalAngle = 0.0;
    double dist = 12005;
    std::vector<double> mis = {1,2,3,0.03,0.02,0.01}; // psi, phi, chi
    RAY::PlaneMirror plM = RAY::PlaneMirror("planemirror", width, height, incidenceAngle, azimuthalAngle, dist, mis); // {1,2,3,0.01,0.02,0.03}

    //std::vector<double> Mis_out = plM.d_misalignmentMatrix();
    std::vector<double> Mis_in = plM.getMisalignmentMatrix();
    std::vector<double> correctMisIn = {0.9997500170828264 , -0.009395493729051622 , 0.02028861849598634 , 0.0 , 0.009997833434164497 , 0.9995060552639085 , -0.02979410709189573 , 0.0 , -0.01999866669333308 , 0.029989501302422495 , 0.9993501304058158 , 0.0 , -1.0 , -2.0 , -3.0 , 1.0};
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, Mis_in, correctMisIn);

    std::vector<double> Mis_out = plM.getInverseMisalignmentMatrix();
    std::vector<double> correctMisOut = {0.9997500170828264 , 0.009997833434164499 , -0.019998666693333084 , 0.0 , -0.00939549372905162 , 0.9995060552639086 , 0.029989501302422495 , 0.0 , 0.02028861849598634 , -0.02979410709189573 , 0.9993501304058158 , 0.0 , 1.0418248851126823 , 1.9196276226862943 , 3.0380307271289593 , 1.0  };
    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, Mis_out, correctMisOut);
}

TEST(Quadric, testInMat) {
    double width = 68.12;
    double height = 123.6;
    double incidenceAngle = 13.2;
    double azimuthalAngle = 0.0;
    double dist = 12005;
    std::vector<double> mis = {0,0,0,0,0,0};
    RAY::PlaneMirror plM = RAY::PlaneMirror("planemirror",width, height, incidenceAngle, azimuthalAngle, dist, mis); // {1,2,3,0.01,0.02,0.03}

    std::vector<double> Mis_out = plM.getInverseTempMisalignmentMatrix();
    std::vector<double> Mis_in = plM.getTempMisalignmentMatrix();
    EXPECT_THAT(Mis_out, ElementsAre(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1));
    EXPECT_THAT(Mis_in, ElementsAre(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1));

    std::vector<double> inMat = plM.getInMatrix();
    std::vector<double> correctInMat = {1.00000000e+00,  0.00000000e+00,  0.00000000e+00, 0.00000000e+00,
       -0.00000000e+00,  0.9735789028731603, 0.22835087011065572, 0.00000000e+00,
       -0.00000000e+00,  -0.22835087011065572,  0.9735789028731603, 0.00000000e+00,
       0.00000000e+00,  2741.352195678422,  -11687.814728992289, 1.00000000e+00};

    EXPECT_ITERABLE_DOUBLE_EQ(std::vector<double>, inMat, correctInMat);
}
/*
TEST(Quadric, testGlobalCoordinates) {
    RAY::PlaneMirror p1 = RAY::PlaneMirror("PlaneMirror1", 50, 200, 10, 7, 0, {0,0,0, 0,0,0}, NULL); // {1,2,3,0.01,0.02,0.03}
    RAY::PlaneMirror p2 = RAY::PlaneMirror("PlaneMirror2", 50, 200, 15, 4, 0, {0,0,0, 0,0,0}, &p1); // {1,2,3,0.01,0.02,0.03}
    RAY::PlaneMirror p3 = RAY::PlaneMirror("PlaneMirror3", 50, 200, 7, 10, 0, {0,0,0, 0,0,0}, &p2); // {1,2,3,0.01,0.02,0.03}
    RAY::PlaneMirror p4 = RAY::PlaneMirror("PlaneMirror4", 50, 200, 22, 17, 0, {0,0,0, 0,0,0}, &p3); // {1,2,3,0.01,0.02,0.03}
    
    std::vector<double> worldCoord = {0.993, -0.120, -0.021, 0,
                                        0.122, 0.977, 0.172, 0,
                                        0.000, -0.174, 0.985, 0,
                                        0, 0, 0, 1};
    std::vector<double> result = p1.getInMatrix();
    for(int i = 0; i<16; i++) {
        EXPECT_NEAR(worldCoord[i], result[i], 0.001);
    }

    // values from RAY-UI interface
    worldCoord = {0.997, -0.050, -0.056, 0,
                                        0.073, 0.818, 0.571, 0,
                                        0.018, -0.573, 0.819, 0,
                                        0, 0, 0, 1};
    result = p2.getInMatrix();
    for(int i = 0; i<16; i++) {
        EXPECT_NEAR(worldCoord[i], result[i], 0.001);
    }

    worldCoord = {0.988, -0.128, -0.084, 0,
                                        0.140, 0.534, 0.834, 0,
                                        -0.062, -0.836, 0.545, 0,
                                        0, 0, 0, 1};
    result = p3.getInMatrix();
    for(int i = 0; i<16; i++) {
        EXPECT_NEAR(worldCoord[i], result[i], 0.001);
    }

    worldCoord = {0.967, -0.182, -0.180, 0,
                                        0.191, 0.044, 0.981, 0,
                                        -0.171, -0.982, 0.077, 0,
                                        0, 0, 0, 1};
    result = p4.getInMatrix();
    for(int i = 0; i<16; i++) {
        EXPECT_NEAR(worldCoord[i], result[i], 0.001);
    }
}*/