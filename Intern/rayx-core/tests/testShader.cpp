#include <numeric>

#include "Tracer/CpuTracer.h"
#include "setupTests.h"

#include "Shader/Utils.h"
#include "Shader/Approx.h"
#include "Shader/SphericalCoords.h"
#include "Shader/Rand.h"
#include "Shader/Refrac.h"
#include "Shader/ApplySlopeError.h"
#include "Shader/LineDensity.h"

TEST_F(TestSuite, testUniformRandom) {
    uint64_t ctr = 13;
    double old = 0;

    for (int i = 0; i < 100; i++) {
        double d = squaresDoubleRNG(ctr);
        CHECK(d != old)  // repeating numbers are forbidden!
        CHECK_IN(d, 0.0, 1.0)
        old = d;
    }
}

TEST_F(TestSuite, testNormalRandom) {
    uint64_t ctr = 13;   // seed value
    double mu = 0.0;     // mean
    double sigma = 1.0;  // standard deviation

    // Vectors to store the generated random numbers and their z-scores
    std::vector<double> random_numbers;
    std::vector<double> z_scores;

    for (int i = 0; i < 1000; i++) {
        double Z = squaresNormalRNG(ctr, mu, sigma);
        random_numbers.push_back(Z);

        // Calculate z-score and store
        double z_score = (Z - mu) / sigma;
        z_scores.push_back(z_score);
    }

    // Calculate mean and standard deviation of the generated random numbers
    double sum = std::accumulate(random_numbers.begin(), random_numbers.end(), 0.0);
    double mean = sum / random_numbers.size();

    double sq_sum = std::inner_product(random_numbers.begin(), random_numbers.end(), random_numbers.begin(), 0.0);
    double std_dev = std::sqrt(sq_sum / random_numbers.size() - mean * mean);

    // Check if mean is close to 0 and standard deviation is close to 1
    CHECK_EQ(mean, mu, 0.1);
    CHECK_EQ(std_dev, sigma, 0.1);

    // Check the distribution by checking if the absolute value of z-scores for the majority of elements is less than 2
    int count = std::count_if(z_scores.begin(), z_scores.end(), [](double d) { return std::abs(d) <= 2.0; });
    CHECK(count > int(0.95 * z_scores.size()))
}

TEST_F(TestSuite, testSin) {
    std::vector<double> args = {
        -0.5620816275750421, -0.082699735953560394, -0.73692442452247864, -0.93085577907030514, 0.038832744045494971, 0.86938579245347758,
        0.35772943348137098, 0.059400386282114415,  0.86087298993938566,  0.3735454248180905,   -0.8663155254748649,  -0.98460362787680167,
        0.40238118899835329, 0.3078379243610454,    0.053857555519812195, 0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(r8_sin(x), sin(x));
    }
}

TEST_F(TestSuite, testCos) {
    std::vector<double> args = {
        -0.5620816275750421, -0.082699735953560394, -0.73692442452247864, -0.93085577907030514, 0.038832744045494971, 0.86938579245347758,
        0.35772943348137098, 0.059400386282114415,  0.86087298993938566,  0.3735454248180905,   -0.8663155254748649,  -0.98460362787680167,
        0.40238118899835329, 0.3078379243610454,    0.053857555519812195, 0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(r8_cos(x), cos(x));
    }
}

TEST_F(TestSuite, testAtan) {
    std::vector<double> args = {
        -0.5620816275750421, -0.082699735953560394, -0.73692442452247864, -0.93085577907030514, 0.038832744045494971, 0.86938579245347758,
        0.35772943348137098, 0.059400386282114415,  0.86087298993938566,  0.3735454248180905,   -0.8663155254748649,  -0.98460362787680167,
        0.40238118899835329, 0.3078379243610454,    0.053857555519812195, 0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(r8_atan(x), atan(x));
    }
}

TEST_F(TestSuite, testExp) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
    for (auto x : args) {
        CHECK_EQ(r8_exp(x), exp(x));
        CHECK_EQ(r8_exp(-x), exp(-x));
    }
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, testLog) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
    for (auto x : args) {
        CHECK_EQ(r8_log(x), log(x));
    }
}

TEST_F(TestSuite, testNormalCartesian) {
    struct InOutPair {
        glm::dvec3 in_normal;
        double in_slopeX;
        double in_slopeZ;

        glm::dvec3 out;
    };

    std::vector<InOutPair> inouts = {{
                                         .in_normal = glm::dvec3(0, 1, 0),
                                         .in_slopeX = 0,
                                         .in_slopeZ = 0,
                                         .out = glm::dvec3(0, 1, 0),
                                     },
                                     {
                                         .in_normal = glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537),
                                         .in_slopeX = 0,
                                         .in_slopeZ = 0,
                                         .out = glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537),
                                     },
                                     {
                                         .in_normal = glm::dvec3(0, 1, 0),
                                         .in_slopeX = 2,
                                         .in_slopeZ = 3,
                                         .out = glm::dvec3(-0.90019762973551742, 0.41198224566568298, -0.14112000805986721),
                                     },
                                     {
                                         .in_normal = glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537),
                                         .in_slopeX = 2,
                                         .in_slopeZ = 3,
                                         .out = glm::dvec3(-9431.2371568647086, 4310.7269916467494, -1449.3435640204684),
                                     }};

    for (auto p : inouts) {
        auto out = normalCartesian(p.in_normal, p.in_slopeX, p.in_slopeZ);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testNormalCylindrical) {
    struct InOutPair {
        glm::dvec3 in_normal;
        double in_slopeX;
        double in_slopeZ;

        glm::dvec3 out;
    };

    std::vector<InOutPair> inouts = {{
                                         .in_normal = glm::dvec3(0, 1, 0),
                                         .in_slopeX = 0,
                                         .in_slopeZ = 0,
                                         .out = glm::dvec3(0, 1, 0),
                                     },
                                     {
                                         .in_normal = glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537),
                                         .in_slopeX = 0,
                                         .in_slopeZ = 0,
                                         .out = glm::dvec3(5.0465463027115769, 10470.451695989539, -28.532199794465537),
                                     },
                                     {
                                         .in_normal = glm::dvec3(0, 1, 0),
                                         .in_slopeX = 2,
                                         .in_slopeZ = 3,
                                         .out = glm::dvec3(0.90019762973551742, 0.41198224566568292, -0.14112000805986721),
                                     },
                                     {.in_normal = glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537),
                                      .in_slopeX = 2,
                                      .in_slopeZ = 3,
                                      .out = glm::dvec3(9431.2169472441783, 4310.7711493493844, -1449.3437356459144)}};

    for (auto p : inouts) {
        auto out = normalCylindrical(p.in_normal, p.in_slopeX, p.in_slopeZ);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testRZPLineDensityDefaulParams) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        RZPBehaviour in_b;

        double out_DX;
        double out_DZ;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_ray =
                Ray{
                    .m_position = glm::dvec3(-5.0805095016939532, 0, 96.032788311782269),
                    .m_direction = glm::dvec3(0, 1, 0),
                },
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_b =
                RZPBehaviour{
                    .m_imageType = 0,
                    .m_rzpType = 0,
                    .m_derivationMethod = 0,
                    .m_designWavelength = 1.239852e-05 * 1e6,
                    .m_orderOfDiffraction = -1,
                    .m_fresnelZOffset = 0,
                    .m_designSagittalEntranceArmLength = 100,
                    .m_designSagittalExitArmLength = 500,
                    .m_designMeridionalEntranceArmLength = 100,
                    .m_designMeridionalExitArmLength = 500,
                    .m_designAlphaAngle = 0.017453292519943295,
                    .m_designBetaAngle = 0.017453292519943295,
                },

            .out_DX = 3103.9106911246749,
            .out_DZ = 5.0771666330055218,
        },
        {.in_ray =
             Ray{
                 .m_position = glm::dvec3(-1.6935030407867075, 0, 96.032777495754004),
                 .m_direction = glm::dvec3(0, 1, 0),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_b =
             RZPBehaviour{
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 1034.8685185321938,
         .out_DZ = -13.320120179862876},
        {.in_ray = Ray{.m_position = glm::dvec3(-5.047050067282087, 4.4859372100394515, 29.182033770349552),
                       .m_direction = glm::dvec3(0.05047050067282087, 0.95514062789960552, -0.29182033770349552)},
         .in_normal = glm::dvec4(0.05047050067282087, 0.95514062789960552, -0.29182033770349552, 0),
         .in_b =
             RZPBehaviour{
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 4045.0989844091882,
         .out_DZ = -174.2085626048659},
        {.in_ray =
             Ray{
                 .m_position = glm::dvec3(-1.6802365843267262, 1.3759250917712356, 16.445931214643075),
                 .m_direction = glm::dvec3(0.016802365843267261, 0.98624074908228765, -0.16445931214643075),
             },
         .in_normal = glm::dvec4(0.016802365843267261, 0.98624074908228765, -0.16445931214643075, 0),
         .in_b =
             RZPBehaviour{
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 1418.1004208892475,
         .out_DZ = 253.09836635775162},
    };

    for (auto p : inouts) {
        double DX;
        double DZ;
        RZPLineDensity(p.in_ray, p.in_normal, p.in_b, DX, DZ);
        CHECK_EQ(DX, p.out_DX);
        CHECK_EQ(DZ, p.out_DZ);
    }
}

TEST_F(TestSuite, testRZPLineDensityAstigmatic) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        RZPBehaviour in_b;

        double out_DX;
        double out_DZ;
    };

    std::vector<InOutPair> inouts = {

        {
            .in_ray =
                {
                    .m_position = glm::dvec3(-5.0805095016939532, 0, 96.032788311782269),
                    .m_direction = glm::dvec3(0, 1, 0),
                },
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_b =
                {
                    .m_imageType = 0,
                    .m_rzpType = 0,
                    .m_derivationMethod = 0,
                    .m_designWavelength = 1.239852e-05 * 1e6,
                    .m_orderOfDiffraction = -1,
                    .m_fresnelZOffset = 0,
                    .m_designSagittalEntranceArmLength = 100,
                    .m_designSagittalExitArmLength = 500,
                    .m_designMeridionalEntranceArmLength = 100,
                    .m_designMeridionalExitArmLength = 500,
                    .m_designAlphaAngle = 0.017453292519943295,
                    .m_designBetaAngle = 0.017453292519943295,
                },

            .out_DX = 3103.9106911246749,
            .out_DZ = 5.0771666330055218,
        },
        {.in_ray =
             {
                 .m_position = glm::dvec3(-1.6935030407867075, 0, 96.032777495754004),
                 .m_direction = glm::dvec3(0, 1, 0),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_b =
             {
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 1034.8685185321938,
         .out_DZ = -13.320120179862876},
        {.in_ray = {.m_position = glm::dvec3(-5.047050067282087, 4.4859372100394515, 29.182033770349552),
                    .m_direction = glm::dvec3(0.05047050067282087, 0.95514062789960552, -0.29182033770349552)},
         .in_normal = glm::dvec4(0.05047050067282087, 0.95514062789960552, -0.29182033770349552, 0),
         .in_b =
             {
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 4045.0989844091882,
         .out_DZ = -174.2085626048659},
        {.in_ray =
             {
                 .m_position = glm::dvec3(-1.6802365843267262, 1.3759250917712356, 16.445931214643075),
                 .m_direction = glm::dvec3(0.016802365843267261, 0.98624074908228765, -0.16445931214643075),
             },
         .in_normal = glm::dvec4(0.016802365843267261, 0.98624074908228765, -0.16445931214643075, 0),
         .in_b =
             {
                 .m_imageType = 0,
                 .m_rzpType = 0,
                 .m_derivationMethod = 0,
                 .m_designWavelength = 1.239852e-05 * 1e6,
                 .m_orderOfDiffraction = -1,
                 .m_fresnelZOffset = 0,
                 .m_designSagittalEntranceArmLength = 100,
                 .m_designSagittalExitArmLength = 500,
                 .m_designMeridionalEntranceArmLength = 100,
                 .m_designMeridionalExitArmLength = 500,
                 .m_designAlphaAngle = 0.017453292519943295,
                 .m_designBetaAngle = 0.017453292519943295,
             },
         .out_DX = 1418.1004208892475,
         .out_DZ = 253.09836635775162},
    };

    for (auto p : inouts) {
        double DX;
        double DZ;
        RZPLineDensity(p.in_ray, p.in_normal, p.in_b, DX, DZ);
        CHECK_EQ(DX, p.out_DX);
        CHECK_EQ(DZ, p.out_DZ);
    }
}

TEST_F(TestSuite, testRayMatrixMult) {
    struct InOutPair {
        Ray in_ray;
        glm::dmat4 in_matrix;

        Ray out_ray;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_ray =
                {
                    .m_position = glm::dvec3(0, 0, 0),
                    .m_direction = glm::dvec3(0, 0, 0),
                },
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
            .out_ray =
                {
                    .m_position = glm::dvec3(13, 14, 15),
                    .m_direction = glm::dvec3(0, 0, 0),
                },
        },
        {
            .in_ray =
                {
                    .m_position = glm::dvec3(1, 1, 0),
                    .m_direction = glm::dvec3(0, 1, 1),
                },
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
            .out_ray =
                {
                    .m_position = glm::dvec3(19, 22, 25),
                    .m_direction = glm::dvec3(14, 16, 18),
                },
        },
        {
            .in_ray =
                {
                    .m_position = glm::dvec3(1, 2, 3),
                    .m_direction = glm::dvec3(4, 5, 6),
                },
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
            .out_ray =
                {
                    .m_position = glm::dvec3(51, 58, 65),
                    .m_direction = glm::dvec3(83, 98, 113),
                },
        },

    };

    for (auto p : inouts) {
        auto out_ray = rayMatrixMult(p.in_ray, p.in_matrix);
        CHECK_EQ(out_ray, p.out_ray);
    }
}

TEST_F(TestSuite, testDPow) {
    struct InOutPair {
        double in_a;
        int in_b;

        double out;
    };

    std::vector<InOutPair> inouts = {{
                                         .in_a = 0,
                                         .in_b = 0,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = 0,
                                         .in_b = 1,
                                         .out = 0,
                                     },
                                     {
                                         .in_a = 0,
                                         .in_b = -1,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = 2,
                                         .in_b = 0,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = 2,
                                         .in_b = 1,
                                         .out = 2,
                                     },
                                     {
                                         .in_a = 3,
                                         .in_b = 7,
                                         .out = 2187,
                                     },
                                     {
                                         .in_a = 0,
                                         .in_b = 4,
                                         .out = 0,
                                     },
                                     {
                                         .in_a = 0,
                                         .in_b = -4,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = 0,
                                         .in_b = 2,
                                         .out = 0,
                                     },
                                     {
                                         .in_a = 0.20000000000000001,
                                         .in_b = 4,
                                         .out = 0.0016000000000000005,
                                     },
                                     {
                                         .in_a = 9.9949999999999992,
                                         .in_b = 3,
                                         .out = 998.50074987499977,
                                     },
                                     {
                                         .in_a = 3.1415926535897931,
                                         .in_b = 6,
                                         .out = 961.38919357530415,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = -4,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = 3,
                                         .out = -1,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = 0,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = 4,
                                         .out = 1,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = 5,
                                         .out = -1,
                                     },
                                     {
                                         .in_a = -1,
                                         .in_b = 6,
                                         .out = 1,
                                     }};

    for (auto p : inouts) {
        auto out = dpow(p.in_a, p.in_b);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testFact) {
    struct InOutPair {
        int in;

        double out;
    };

    std::vector<InOutPair> inouts = {
        {
            .in = 2,
            .out = 2,
        },
        {
            .in = 1,
            .out = 1,
        },
        {
            .in = 0,
            .out = 1,
        },
        {
            .in = 17,
            .out = 355687428096000,
        },
        {
            .in = 4,
            .out = 24,
        },
        {
            .in = -1,
            .out = -1,
        },
        {
            .in = -2,
            .out = -2,
        },
        {
            .in = 0,
            .out = 1,
        },
        {
            .in = 12,
            .out = 479001600,
        },
        {
            .in = -4,
            .out = -4,
        },
        {
            .in = 10,
            .out = 3628800,
        },
        {
            .in = 0,
            .out = 1,
        },

    };

    for (auto p : inouts) {
        auto out = fact(p.in);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testBessel1) {
    struct InOutPair {
        double in;

        double out;
    };

    std::vector<InOutPair> inouts = {
        {
            .in = 100,
            .out = 0,
        },
        {
            .in = 20.100000000000001,
            .out = 0,
        },
        {
            .in = -12.122999999999999,
            .out = 0,
        },
        {
            .in = 23.100000000000001,
            .out = 0,
        },
        {
            .in = 0,
            .out = 0,
        },
        {
            .in = 20,
            .out = 0.066833545658411236,
        },
        {
            .in = -0.10000000000000001,
            .out = 0,
        },
        {
            .in = 1e-08,
            .out = 5.0000000000000001e-09,
        },
        {
            .in = 2,
            .out = 0.57672480775687363,
        },
        {
            .in = 12.122999999999999,
            .out = -0.21368198451302897,
        },
        {
            .in = 3.1415926535897931,
            .out = 0.28461534317975273,
        },
        {
            .in = 10.199999999999999,
            .out = -0.0066157432977083167,
        },
        {
            .in = 19.989999999999998,
            .out = 0.065192988349741909,
        },
        {
            .in = 4,
            .out = -0.06604332802354923,
        },

    };

    for (auto p : inouts) {
        auto out = bessel1(p.in);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testVlsGrating) {
    struct InOutPair {
        double in_lineDensity;
        double in_z;
        double in_vls[6];

        double out;
    };

    std::vector<InOutPair> inouts = {{
                                         .in_lineDensity = 0.01239852,
                                         .in_z = 5.0020783775947848,
                                         .in_vls = {0, 0, 0, 0, 0, 0},
                                         .out = 0.01239852,
                                     },
                                     {
                                         .in_lineDensity = 0.01239852,
                                         .in_z = 5.0020783775947848,
                                         .in_vls = {1, 2, 3, 4, 5, 6},
                                         .out = 9497.4799596119265,
                                     }};

    for (auto p : inouts) {
        auto out = vlsGrating(p.in_lineDensity, p.in_z, p.in_vls);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testRefracPlane) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        double in_a;

        Ray out_ray;
    };

    std::vector<InOutPair>
        inouts =
            {
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0, -0.99558611855684065, 0.093851108341926615),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0, 0.99667709206767885, 0.081452588341926618),
                     }},
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, -0.99558611855684065, 0.093851108341926615),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, 0.99667709206767885, 0.081452588341926618),
                     }},
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, -0.99567947186812988, 0.0928554753392902),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, 0.99675795875308415, 0.080456955339290204),
                     }},
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, -0.99567947186812988, 0.0928554753392902),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(0.01239852, 0.99675795875308415, 0.080456955339290204),
                     }},
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(-0.00049999991666660004, -0.99558611855684065, 0.093851108341926226),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(-0.00049999991666660004, 0.99667709206767885, 0.08145258834192623),
                     }},
                {.in_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(-0.00049999991666660004, -0.995586229182718, 0.093851118714515264),
                     },
                 .in_normal = glm::dvec4(0, 1, 0, 0),
                 .in_a = 0.01239852,
                 .out_ray =
                     {
                         .m_position = glm::dvec3(0, 1, 0),
                         .m_eventType = 0.01239852,
                         .m_direction = glm::dvec3(-0.00049999991666660004, 0.9966772027014974, 0.081452598714515267),
                     }},

            };

    for (auto p : inouts) {
        auto out_ray = refracPlane(p.in_ray, p.in_normal, p.in_a);
        CHECK_EQ(out_ray, p.out_ray);
    }
}

TEST_F(TestSuite, testGetIncidenceAngle) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;

        double out;
    };

    std::vector<InOutPair> inouts = {{.in_ray =
                                          {
                                              .m_position = glm::dvec3(0, 1, 0),
                                              .m_direction = glm::dvec3(-0.00049999997222222275, -0.17381228817387082, 0.98477867487054738),
                                          },
                                      .in_normal = glm::dvec4(0, 1, 0, 0),
                                      .out = 1.3960967569703167}};

    for (auto p : inouts) {
        auto out = getIncidenceAngle(p.in_ray, p.in_normal);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testReflectance) {
    updateCpuTracerMaterialTables({Material::Cu});

    struct InOutPair {
        double in_energy;
        double in_incidenceAngle;
        int in_material;

        glm::dvec2 out_complex_S;
        glm::dvec2 out_complex_P;
    };

    std::vector<InOutPair> inouts = {{.in_energy = 100,
                                      .in_incidenceAngle = 1.3962634006709251,
                                      .in_material = 29,
                                      .out_complex_S = glm::dvec2(-NAN, -NAN),
                                      .out_complex_P = glm::dvec2(NAN, NAN)}};

    for (auto p : inouts) {
        glm::dvec2 complex_S;
        glm::dvec2 complex_P;
        reflectance(p.in_energy, p.in_incidenceAngle, complex_S, complex_P, p.in_material);
        CHECK_EQ(complex_S, p.out_complex_S);
        CHECK_EQ(complex_P, p.out_complex_P);
    }
}

TEST_F(TestSuite, testSnell) {
    struct InOutPair {
        glm::dvec2 in_cosIncidence;
        glm::dvec2 in_cn1;
        glm::dvec2 in_cn2;

        glm::dvec2 out;
    };

    std::vector<InOutPair> inouts = {
        {.in_cosIncidence = glm::dvec2(0.17364817766693041, 0),
         .in_cn1 = glm::dvec2(1, 0),
         .in_cn2 = glm::dvec2(0.91452118089946777, 0.035187568837614078),
         .out = glm::dvec2(0.10906363661670573, 0.40789272188567433)},
        {.in_cosIncidence = glm::dvec2(0.17315572500228882, 0),
         .in_cn1 = glm::dvec2(1, 0),
         .in_cn2 = glm::dvec2(0.91453807092958361, 0.035170965000031584),
         .out = glm::dvec2(0.10897754475504863, 0.40807275584607489)},
        {.in_cosIncidence = glm::dvec2(0.1736481785774231, 0),
         .in_cn1 = glm::dvec2(1, 0),
         .in_cn2 = glm::dvec2(0.96684219999999998, 0.06558986),
         .out = glm::dvec2(0.24302165191294173, 0.28697207607552533)},
        {.in_cosIncidence = glm::dvec2(0.16307067260397731, 0.0027314608130525712),
         .in_cn1 = glm::dvec2(0.99816471240025439, 0.00045674598468145697),
         .in_cn2 = glm::dvec2(0.99514154037883318, 0.0047593281563246184),
         .out = glm::dvec2(0.14743465849863333, 0.031766878855366699)},

    };

    for (auto p : inouts) {
        auto out = snell(p.in_cosIncidence, p.in_cn1, p.in_cn2);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testFresnel) {
    struct InOutPair {
        glm::dvec2 in_cn1;
        glm::dvec2 in_cn2;
        glm::dvec2 in_cosIncidence;
        glm::dvec2 in_cosTransmittance;

        glm::dvec2 out_complex_S;
        glm::dvec2 out_complex_P;
    };

    std::vector<InOutPair> inouts = {
        {.in_cn1 = glm::dvec2(0.91453807092958361, 0.035170965000031584),
         .in_cn2 = glm::dvec2(1, 0),
         .in_cosIncidence = glm::dvec2(0.10897754475504851, 0.40807275584607544),
         .in_cosTransmittance = glm::dvec2(0.17315572500228882, 0),
         .out_complex_S = glm::dvec2(0.57163467986230054, 0.62486367906829521),
         .out_complex_P = glm::dvec2(0.63080662811278632, 0.52640331936127849)},
        {.in_cn1 = glm::dvec2(0.91452118089946777, 0.035187568837614078),
         .in_cn2 = glm::dvec2(1, 0),
         .in_cosIncidence = glm::dvec2(0.10906363669865969, 0.40789272144618016),
         .in_cosTransmittance = glm::dvec2(0.1736481785774231, 0),
         .out_complex_S = glm::dvec2(0.56981824812215454, 0.62585833416785819),
         .out_complex_P = glm::dvec2(0.62929764490597007, 0.52731592442193231)},

    };

    for (auto p : inouts) {
        glm::dvec2 out_complex_S;
        glm::dvec2 out_complex_P;
        fresnel(p.in_cn1, p.in_cn2, p.in_cosIncidence, p.in_cosTransmittance, out_complex_S, out_complex_P);
        CHECK_EQ(out_complex_S, p.out_complex_S);
        CHECK_EQ(out_complex_P, p.out_complex_P);
    }
}

TEST_F(TestSuite, testCartesianToEuler) {
    struct InOutPair {
        glm::dvec2 in_complex;
        glm::dvec2 out;
    };

    std::vector<InOutPair> inouts = {
        {.in_complex = glm::dvec2(0.63080662811278621, 0.52640331936127871), .out = glm::dvec2(0.67501745670559532, 0.69542190922049119)},
        {.in_complex = glm::dvec2(0.57163467986230043, 0.62486367906829532), .out = glm::dvec2(0.71722082464004022, 0.82985616444880206)},
        {.in_complex = glm::dvec2(1, 0), .out = glm::dvec2(1, 0)},
        {.in_complex = glm::dvec2(0, 1), .out = glm::dvec2(1, 1.5707963267948966)}};

    for (auto p : inouts) {
        auto out = cartesian_to_euler(p.in_complex);
        CHECK_EQ(out, p.out);
    }
}

// both the Cpp code as well as the shader define hvlam.
TEST_F(TestSuite, testHvlam) {
    double hv = 100;
    double linedensity = 1000;
    double orderOfDiff = 1;
    double a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    CHECK_EQ(a, 0.01239852);

    a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    CHECK_EQ(a, 0.01239852);
}

TEST_F(TestSuite, testGetAtomicMassAndRho) {
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::H)), glm::dvec2(1.00794, 0.0708));
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::He)), glm::dvec2(4.0026, 0.122));
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::Li)), glm::dvec2(6.941, 0.533));
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::Be)), glm::dvec2(9.01218, 1.845));
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::B)), glm::dvec2(10.81, 2.34));
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::C)), glm::dvec2(12.011, 2.26));
    // ...
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::Cu)), glm::dvec2(63.546, 8.94));
    // ...
    CHECK_EQ(getAtomicMassAndRho(static_cast<int>(Material::U)), glm::dvec2(238.0289, 18.92));
}

TEST_F(TestSuite, testPalik) {
    updateCpuTracerMaterialTables({Material::Cu, Material::Au});

    int Cu = static_cast<int>(Material::Cu);
    CHECK_EQ(getPalikEntryCount(Cu), 324);

    auto Cu0 = getPalikEntry(0, Cu);
    CHECK_EQ(Cu0.m_energy, 1.0);
    CHECK_EQ(Cu0.m_n, 0.433);
    CHECK_EQ(Cu0.m_k, 8.46);

    auto Cu10 = getPalikEntry(10, Cu);
    CHECK_EQ(Cu10.m_energy, 2.3);
    CHECK_EQ(Cu10.m_n, 1.04);
    CHECK_EQ(Cu10.m_k, 2.59);

    int Au = static_cast<int>(Material::Au);
    CHECK_EQ(getPalikEntryCount(Au), 386);

    auto Au0 = getPalikEntry(0, Au);
    CHECK_EQ(Au0.m_energy, 0.04959);
    CHECK_EQ(Au0.m_n, 20.3);
    CHECK_EQ(Au0.m_k, 76.992);

    auto Au10 = getPalikEntry(10, Au);
    CHECK_EQ(Au10.m_energy, 0.11158);
    CHECK_EQ(Au10.m_n, 12.963);
    CHECK_EQ(Au10.m_k, 57.666);
}

TEST_F(TestSuite, testNff) {
    updateCpuTracerMaterialTables({Material::Cu, Material::Au});

    int Cu = static_cast<int>(Material::Cu);
    CHECK_EQ(getNffEntryCount(Cu), 504);

    auto Cu0 = getNffEntry(0, Cu);

    CHECK_EQ(Cu0.m_energy, 10.0);
    CHECK_EQ(Cu0.m_f1, -9999.0);
    CHECK_EQ(Cu0.m_f2, 1.30088);

    auto Cu10 = getNffEntry(10, Cu);
    CHECK_EQ(Cu10.m_energy, 11.7404);
    CHECK_EQ(Cu10.m_f1, -9999.0);
    CHECK_EQ(Cu10.m_f2, 1.66946);

    int Au = static_cast<int>(Material::Au);
    CHECK_EQ(getNffEntryCount(Au), 506);

    auto Au0 = getNffEntry(0, Au);
    CHECK_EQ(Au0.m_energy, 10.0);
    CHECK_EQ(Au0.m_f1, -9999.0);
    CHECK_EQ(Au0.m_f2, 1.73645);

    auto Au10 = getNffEntry(10, Au);
    CHECK_EQ(Au10.m_energy, 11.7404);
    CHECK_EQ(Au10.m_f1, -9999.0);
    CHECK_EQ(Au10.m_f2, 2.67227);
}

TEST_F(TestSuite, testRefractiveIndex) {
    updateCpuTracerMaterialTables({Material::Cu});

    // vacuum
    CHECK_EQ(getRefractiveIndex(42.0, -1), glm::dvec2(1.0, 0.0));

    // palik tests for Cu
    // data taken from Data/PALIK
    CHECK_EQ(getRefractiveIndex(1.0, 29), glm::dvec2(0.433, 8.46));
    CHECK_EQ(getRefractiveIndex(1.8, 29), glm::dvec2(0.213, 4.05));
    CHECK_EQ(getRefractiveIndex(1977.980, 29), glm::dvec2(1.000032, 9.4646668E-05));

    // nff tests for Cu
    // data taken from
    // https://refractiveindex.info/?shelf=main&book=Cu&page=Hagemann
    CHECK_EQ(getRefractiveIndex(25146.2, 29), glm::dvec2(1.0, 1.0328e-7), 1e-5);
}

TEST_F(TestSuite, testBesselDipole) {
    struct InOutPair {
        double proportion;
        double zeta;
        double out;
    };
    std::vector<InOutPair> inouts = {{
                                         .proportion = 1 / 3,
                                         .zeta = 78.126966373103443,
                                         .out = 1.664046593883771e-35,
                                     },
                                     {
                                         .proportion = 1 / 3,
                                         .zeta = 73.550785975500432,
                                         .out = 1.6659366793149262e-33,
                                     },
                                     {
                                         .proportion = 1 / 3,
                                         .zeta = 46.422887861754496,
                                         .out = 1.2672053903555623e-21,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta = 78.126966373103443,
                                         .out = 1.6675777760881476e-35,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta = 73.550785975500432,
                                         .out = 1.6696906039215801e-33,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta = 49.798819164687949,
                                         .out = 4.1969864622545434e-23,
                                     }};

    auto beamline = loadBeamline("dipole_plain");
    std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*src);

    for (auto values : inouts) {
        auto result = dipolesource->bessel(values.proportion, values.zeta);
        CHECK_EQ(result, values.out, 0.1);
    }
}

TEST_F(TestSuite, testSchwingerDipole) {
    struct InOutPair {
        double energy;
        double flux;
    };
    std::vector<InOutPair> inouts = {{
                                         .energy = 6520.0878532052693,
                                         .flux = 566462407647095.5,
                                     },
                                     {.energy = 100, .flux = 2855336264551178},
                                     {
                                         .energy = 900,
                                         .flux = 3762078406399219,
                                     },
                                     {
                                         .energy = 2000,
                                         .flux = 2907004029317153.5,
                                     },
                                     {
                                         .energy = 0.667,
                                         .flux = 596812742357665.25,
                                     },
                                     {
                                         .energy = 2456,
                                         .flux = 2526853293939861,
                                     }};

    auto beamline = loadBeamline("dipole_plain");
    std::shared_ptr<LightSource> src = beamline.m_LightSources[0];
    DipoleSource* dipolesource = dynamic_cast<DipoleSource*>(&*src);

    for (auto values : inouts) {
        auto result = dipolesource->schwinger(values.energy);
        CHECK_EQ(result, values.flux, 0.000000001);
    }
}

TEST_F(TestSuite, testSphericalCoords) {
    std::vector<dvec3> directions = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
        {-1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, 0.0, -1.0},
    };

    for (auto dir : directions) {
        double phi, psi;
        directionToSphericalCoords(dir, phi, psi);

        dvec3 dir2;
        sphericalCoordsToDirection(phi, psi, dir2);

        CHECK_EQ(dir, dir2, 1e-11);
    }
}
