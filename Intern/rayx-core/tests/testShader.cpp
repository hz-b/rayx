#include <numeric>

#include "Beamline/Objects/DipoleSource.h"
#include "Shader/ApplySlopeError.h"
#include "Shader/Approx.h"
#include "Shader/LineDensity.h"
#include "Shader/Rand.h"
#include "Shader/Refrac.h"
#include "Shader/SphericalCoords.h"
#include "Shader/Utils.h"
#include "setupTests.h"

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
        CHECK_EQ(glm::sin(x), sin(x));
    }
}

TEST_F(TestSuite, testCos) {
    std::vector<double> args = {
        -0.5620816275750421, -0.082699735953560394, -0.73692442452247864, -0.93085577907030514, 0.038832744045494971, 0.86938579245347758,
        0.35772943348137098, 0.059400386282114415,  0.86087298993938566,  0.3735454248180905,   -0.8663155254748649,  -0.98460362787680167,
        0.40238118899835329, 0.3078379243610454,    0.053857555519812195, 0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(glm::cos(x), cos(x));
    }
}

TEST_F(TestSuite, testAtan) {
    std::vector<double> args = {
        -0.5620816275750421, -0.082699735953560394, -0.73692442452247864, -0.93085577907030514, 0.038832744045494971, 0.86938579245347758,
        0.35772943348137098, 0.059400386282114415,  0.86087298993938566,  0.3735454248180905,   -0.8663155254748649,  -0.98460362787680167,
        0.40238118899835329, 0.3078379243610454,    0.053857555519812195, 0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(glm::atan(x), atan(x));
    }
}

TEST_F(TestSuite, testExp) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
    for (auto x : args) {
        CHECK_EQ(glm::exp(x), exp(x));
        CHECK_EQ(glm::exp(-x), exp(-x));
    }
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, testLog) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
    for (auto x : args) {
        CHECK_EQ(glm::log(x), log(x));
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
        auto out_ray = rayMatrixMult(p.in_matrix, p.in_ray);
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
        auto out = vlsGrating(p.in_lineDensity, glm::dvec3(0, 1, 0), p.in_z, p.in_vls);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testElectricField) {
    CHECK_EQ(1.0, intensity(LocalElectricField({1, 0}, {0, 0})));
    CHECK_EQ(1.0, intensity(ElectricField({1, 0}, {0, 0}, {0, 0})));
    CHECK_EQ(LocalElectricField({1, 0}, {0, 0}), LocalElectricField(ElectricField({1, 0}, {0, 0}, {0, 0})));
    CHECK_EQ(ElectricField({1, 0}, {0, 0}, {0, 0}), ElectricField(LocalElectricField({1, 0}, {0, 0}), complex::Complex{0, 0}));
}

TEST_F(TestSuite, testStokes) {
    CHECK_EQ(0.1, intensity(Stokes{0.1, 0.2, 0.3, 0.4}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, 1, 0, 0}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, -1, 0, 0}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, 0, 1, 0}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, 0, -1, 0}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, 0, 0, 1}));
    CHECK_EQ(1.0, degreeOfPolarization(Stokes{1, 0, 0, -1}));
    CHECK_EQ(0.0, degreeOfPolarization(Stokes{1, 0, 0, 0}));
}

TEST_F(TestSuite, testElectricFieldRotation) {
    const auto incidentField0 = ElectricField{{1, 0}, {0, 0}, {0, 0}};
    const auto incidentField1 = ElectricField{{0, 0}, {0, 0}, {1, 0}};
    const auto incidentField2 = ElectricField{{0, 0}, {0, 0}, {0, 1}};

    const auto doNothing = [](ElectricField f) { return rotationMatrix(glm::dvec3{0, 0, 1}, glm::dvec3{0, 1, 0}) * f; };
    CHECK_EQ(incidentField0, doNothing(incidentField0));
    CHECK_EQ(incidentField1, doNothing(incidentField1));
    CHECK_EQ(incidentField2, doNothing(incidentField2));

    const auto rotateUp = [](ElectricField f) { return rotationMatrix(glm::dvec3{0, 1, 0}, glm::dvec3{0, 0, -1}) * f; };
    CHECK_EQ(incidentField0, rotateUp(incidentField0));
    CHECK_EQ(ElectricField({0, 0}, {1, 0}, {0, 0}), rotateUp(incidentField1));
    CHECK_EQ(ElectricField({0, 0}, {0, 1}, {0, 0}), rotateUp(incidentField2));

    const auto rotateRight = [](ElectricField f) { return rotationMatrix(glm::dvec3{1, 0, 0}, glm::dvec3{0, 1, 0}) * f; };
    CHECK_EQ(ElectricField({0, 0}, {0, 0}, {-1, 0}), rotateRight(incidentField0));
    CHECK_EQ(ElectricField({1, 0}, {0, 0}, {0, 0}), rotateRight(incidentField1));
    CHECK_EQ(ElectricField({0, 1}, {0, 0}, {0, 0}), rotateRight(incidentField2));
}

TEST_F(TestSuite, testElectricFieldRotationWithBaseConvention) {
    const auto forward = glm::dvec3(0, 0, 1);
    const auto right = glm::dvec3(1, 0, 0);
    const auto up = glm::dvec3(0, 1, 0);

    {
        // test if the convention uses the correct forward, up and right

        // rotation into forward yields identity matrix
        CHECK_EQ(rotationMatrixWithBaseConvention(forward), glm::dmat3(1.0));

        // rotate forward into right yields right
        CHECK_EQ(rotationMatrixWithBaseConvention(right) * forward, right);

        // rotate forward into up yields up
        CHECK_EQ(rotationMatrixWithBaseConvention(up) * forward, up);
    }

    {
        // in order to test if the rotation function is steadily,
        // we gradually increase the rotation angle and compare the resulting angle to the previous.

        const auto steps = 100;
        const auto stepAngle = 1.0 / steps * PI * 2.0;

        auto get_dir = [](double angle) { return glm::dvec3(glm::cos(angle), glm::sin(angle), 0); };

        auto prev_vec = get_dir(0.0f);

        for (int i = 1; i < steps + 1; ++i) {
            auto angle = i / static_cast<double>(steps) * PI * 2.0;
            auto dir = get_dir(angle);
            auto new_vec = rotationMatrixWithBaseConvention(dir) * forward;
            auto angle_between_vecs = angleBetweenUnitVectors(prev_vec, new_vec);
            CHECK_EQ(angle_between_vecs, stepAngle);
            prev_vec = new_vec;
        }
    }
}

TEST_F(TestSuite, testElectricFieldRotationWithBaseConvention2) {
    using namespace complex;

    // convention for incident field
    // forward = (0, 0, -1)
    // up      = (0, 1, 0)
    // right   = (1, 0, 0)
    const auto incidentElectricField = ElectricField({1, 0}, {0, 0}, {0, 0});

    struct InOutPair {
        ElectricField in_field;
        glm::dvec3 in_forward;
        glm::dvec3 in_up;
        ElectricField out_field;
        ElectricField out_field_rotationWithoutUp;
    };

    const auto inouts = std::vector<InOutPair>{
        {
            .in_field = {{1, 0}, {0, 0}, {0, 0}},
            .in_forward = {0, 0, -1},
            .in_up = {0, 1, 0},
            .out_field = {{-1, 0}, {0, 0}, {0, 0}},
            .out_field_rotationWithoutUp = {{-1, 0}, {0, 0}, {0, 0}},
        },
        {
            .in_field = {{1, 0}, {0, 0}, {0, 0}},
            .in_forward = {1, 0, 0},
            .in_up = {0, 0, 1},
            .out_field = {{0, 0}, {1, 0}, {0, 0}},
            .out_field_rotationWithoutUp = {{0, 0}, {0, 0}, {-1, 0}},
        },
    };

    for (const auto& p : inouts) {
        const auto rotation = rotationMatrix(p.in_forward, p.in_up);
        const auto field = rotation * incidentElectricField;
        CHECK_EQ(field, p.out_field);

        const auto rotationWithoutUp = rotationMatrixWithBaseConvention(p.in_forward);
        const auto field_rotationWithoutUp = rotationWithoutUp * incidentElectricField;
        CHECK_EQ(field_rotationWithoutUp, p.out_field_rotationWithoutUp);
    }
}

TEST_F(TestSuite, testElectricFieldToStokesConversion) {
    const auto h = std::sqrt(2.0) / 2.0;

    CHECK_EQ(Stokes(1, 1, 0, 0), localElectricFieldToStokes(LocalElectricField({1, 0}, {0, 0})));
    CHECK_EQ(Stokes(1, -1, 0, 0), localElectricFieldToStokes(LocalElectricField({0, 0}, {1, 0})));
    CHECK_EQ(Stokes(1, 0, 1, 0), localElectricFieldToStokes(LocalElectricField({h, 0}, {h, 0})));
    CHECK_EQ(Stokes(1, 0, -1, 0), localElectricFieldToStokes(LocalElectricField({-h, 0}, {h, 0})));
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

TEST_F(TestSuite, testSnell) {
    using namespace complex;

    struct InOutPair {
        Complex in_ior_i;
        Complex in_ior_t;
        Complex in_incident_angle;

        Complex out_refract_angle;
    };

    const auto inouts = std::vector<InOutPair>{
        {
            .in_ior_i = Complex(1.0, 0),
            .in_ior_t = Complex(1.0, 0),
            .in_incident_angle = Complex(0.5, 0.5),
            .out_refract_angle = Complex(0.5, 0.5),
        },
        {
            .in_ior_i = Complex(1.0, 0),
            .in_ior_t = Complex(1.2, 0),
            .in_incident_angle = Complex(PI * 0.5, 0),
            .out_refract_angle = Complex(0.98511078333774571, 0),
        },
        {
            .in_ior_i = Complex(1.0, 0),
            .in_ior_t = Complex(1.2, 0),
            .in_incident_angle = Complex(PI * 0.25, 0),
            .out_refract_angle = Complex(0.63013724606456445, 0),
        },
        {
            .in_ior_i = Complex(1.2, 0),
            .in_ior_t = Complex(1.0, 0),
            .in_incident_angle = Complex(0.63013724606456445, 0),
            .out_refract_angle = Complex(PI * 0.25, 0),
        },
        {
            .in_ior_i = Complex(1.0, 0),
            .in_ior_t = Complex(0.05, 5.0),
            .in_incident_angle = Complex(PI * 0.25, 0),
            .out_refract_angle = Complex(0.0014001432455587293, -0.14094028035483516),
        },
        {
            .in_ior_i = Complex(1.0, 0),
            .in_ior_t = Complex(2.0, 0),
            .in_incident_angle = Complex(PI * 0.45, 0),
            .out_refract_angle = Complex(0.51650510236395286, 0),
        },
    };

    for (const auto p : inouts) {
        const auto refractAngle = calcRefractAngle(p.in_incident_angle, p.in_ior_i, p.in_ior_t);
        CHECK_EQ(refractAngle, p.out_refract_angle);
    }
}

TEST_F(TestSuite, testFresnel) {
    using namespace complex;

    struct InOutPair {
        Complex in_ior_i;
        Complex in_ior_t;
        Complex in_incident_angle;
        Complex in_refract_angle;

        ComplexFresnelCoeffs out_reflect_amplitude;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_ior_i = Complex(0.91453807092958361, 0.035170965000031584),
            .in_ior_t = Complex(1, 0),
            .in_incident_angle = acos(Complex(0.10897754475504851, 0.40807275584607544)),
            .in_refract_angle = acos(Complex(0.17315572500228882, 0)),
            .out_reflect_amplitude =
                {
                    .s = Complex(0.57163467986230054, 0.62486367906829521),
                    .p = Complex(0.63080662811278632, 0.52640331936127849),
                },
        },
        {
            .in_ior_i = Complex(0.91452118089946777, 0.035187568837614078),
            .in_ior_t = Complex(1, 0),
            .in_incident_angle = acos(Complex(0.10906363669865969, 0.40789272144618016)),
            .in_refract_angle = acos(Complex(0.1736481785774231, 0)),
            .out_reflect_amplitude =
                {
                    .s = Complex(0.56981824812215454, 0.62585833416785819),
                    .p = Complex(0.62929764490597007, 0.52731592442193231),
                },
        },
    };

    for (auto p : inouts) {
        const auto reflectAmplitude = calcReflectAmplitude(p.in_incident_angle, p.in_refract_angle, p.in_ior_i, p.in_ior_t);
        CHECK_EQ(reflectAmplitude.p, p.out_reflect_amplitude.p);
        CHECK_EQ(reflectAmplitude.s, p.out_reflect_amplitude.s);
    }
}

TEST_F(TestSuite, testPolarizationIntensity) {
    using namespace complex;

    const auto iorI = Complex(1.0, 0);
    const auto refractIor = Complex(1.5, 0);
    const auto incidentVec = glm::dvec3(1, 0, 0);
    const auto normalVec = glm::normalize(glm::dvec3(-1, 1, 0));

    const auto reflectVec = glm::reflect(incidentVec, normalVec);
    const auto incidentAngle = angleBetweenUnitVectors(incidentVec, -normalVec);
    const auto refractAngle = calcRefractAngle(incidentAngle, iorI, refractIor);

    const auto reflectAmplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, refractIor);
    const auto refractAmplitude = calcRefractAmplitude(incidentAngle, refractAngle, iorI, refractIor);

    const auto reflectIntensity = calcReflectIntensity(reflectAmplitude);
    const auto refractIntensity = calcRefractIntensity(refractAmplitude, incidentAngle, refractAngle, iorI, refractIor);
    CHECK_EQ(reflectIntensity.s + refractIntensity.s, 1.0);
    CHECK_EQ(reflectIntensity.p + refractIntensity.p, 1.0);
}

TEST_F(TestSuite, testPolarizingReflectionScenario) {
    using namespace complex;

    const auto incidentVec = glm::normalize(glm::dvec3(-0.195, -0.195, 0.961));
    const auto normal_vec_0 = glm::normalize(glm::dvec3(0, -1, -1));

    const auto reflect_vec_0 = glm::reflect(incidentVec, -normal_vec_0);
    CHECK_EQ(reflect_vec_0, glm::normalize(glm::dvec3(-0.195, -0.961, 0.195)));

    const auto incident_angle_0 = angleBetweenUnitVectors(incidentVec, -normal_vec_0);
    CHECK_EQ(Rad(incident_angle_0).toDeg().deg, 57.19646879265609);

    const auto reflect_amplitude_0 = ComplexFresnelCoeffs{
        .s = polar(0.992, 2.918),
        .p = polar(0.975, -0.751),
    };

    const auto reflect_polarization_matrix_0 = calcPolaririzationMatrix(incidentVec, reflect_vec_0, normal_vec_0, reflect_amplitude_0);

    CHECK_EQ(reflect_polarization_matrix_0,
             glm::transpose(cmat3{
                 {-0.889, 0.219},
                 {0.106, 0.046},
                 {-0.361, 0.054},
                 {0.361, -0.054},
                 {0.314, -0.137},
                 {-0.863, -0.039},
                 {-0.106, -0.046},
                 {0.655, -0.628},
                 {0.314, -0.137},
             }),
             1e-3);

    const auto normal_vec_1 = glm::normalize(glm::dvec3(1, 1, 0));
    const auto reflect_vec_1 = glm::reflect(reflect_vec_0, -normal_vec_1);
    CHECK_EQ(reflect_vec_1, glm::normalize(glm::dvec3(0.961, 0.195, 0.195)));

    const auto incident_angle_1 = angleBetweenUnitVectors(reflect_vec_0, -normal_vec_1);
    CHECK_EQ(Rad(incident_angle_1).toDeg().deg, 35.155651179977404);

    const auto reflect_amplitude_1 = ComplexFresnelCoeffs{
        .s = polar(0.988, 2.80),
        .p = polar(0.982, -0.507),
    };

    const auto reflect_polarization_matrix_1 = calcPolaririzationMatrix(reflect_vec_0, reflect_vec_1, normal_vec_1, reflect_amplitude_1);

    CHECK_EQ(reflect_polarization_matrix_1,
             glm::transpose(cmat3{
                 {-0.352, 0.081},
                 {-0.855, -0.028},
                 {0.365, -0.056},
                 {0.792, -0.450},
                 {-0.352, 0.081},
                 {0.052, -0.052},
                 {-0.054, 0.052},
                 {-0.365, 0.056},
                 {-0.853, 0.327},
             }),
             1e-2  // TODO: should be 1e-3
    );
}

TEST_F(TestSuite, testInterceptReflectPartiallyPolarizing) {
    using namespace complex;

    struct IorPair {
        Complex iorI;
        Complex refractIor;
    };

    const auto ior_pairs = std::vector<IorPair>{
        {
            .iorI = {1.0, 0},
            .refractIor = {1.5, 0},
        },
        {
            .iorI = {1.5, 0},
            .refractIor = {1.0, 0},
        },
        {
            .iorI = {1.0, 0},
            .refractIor = {0.05, 5.0},
        },
    };

    for (const auto ior_pair : ior_pairs) {
        const auto iorI = ior_pair.iorI;
        const auto refractIor = ior_pair.refractIor;
        const auto incidentVec = glm::dvec3(1, 0, 0);
        const auto normalVec = glm::normalize(glm::dvec3(-1, 1, 0));

        const auto reflectVec = glm::reflect(incidentVec, normalVec);
        const auto incidentAngle = angleBetweenUnitVectors(incidentVec, -normalVec);
        const auto refractAngle = calcRefractAngle(incidentAngle, iorI, refractIor);

        const auto incidentElectricField = ElectricField({0, 0}, {1, 0}, {1, 0});
        const auto reflectElectricField = interceptReflect(incidentElectricField, incidentVec, reflectVec, normalVec, iorI, refractIor);

        const auto amplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, refractIor);
        const auto expected_reflect_field = ElectricField(
            // p polarized part
            // the value from the y component of the incident field is now carried in the x compnent, due to a 90 degrees reflection
            // the amplitude is negated due to a 180 degrees phase shift
            -amplitude.p * incidentElectricField.y,

            // y component is now 0
            {0, 0},

            // s polarized part
            // only gets an amplitude change
            amplitude.s * incidentElectricField.z);

        CHECK_EQ(reflectElectricField, expected_reflect_field);
    }
}

// fully polarizing reflection (reflection at brewsters angle)
TEST_F(TestSuite, testInterceptReflectFullyPolarizing) {
    using namespace complex;

    const auto iorI = Complex(1.0, 0);
    const auto refractIor = Complex(2.0, 0);
    const auto incidentVec = glm::dvec3(1, 0, 0);
    const auto normalVec = glm::normalize(glm::dvec3(-1, refractIor.real(), 0));

    const auto reflectVec = glm::reflect(incidentVec, normalVec);
    const auto incidentAngle = angleBetweenUnitVectors(incidentVec, -normalVec);
    const auto refractAngle = calcRefractAngle(incidentAngle, iorI, refractIor);

    const auto brewstersAngle = calcBrewstersAngle(iorI, refractIor);
    CHECK_EQ(incidentAngle, brewstersAngle.real());

    const auto incidentElectricField = ElectricField({0, 0}, {1, 0}, {1, 0});
    const auto reflectElectricField = interceptReflect(incidentElectricField, incidentVec, reflectVec, normalVec, iorI, refractIor);

    const auto amplitude = calcReflectAmplitude(incidentAngle, refractAngle, iorI, refractIor);
    const auto expectedReflectElectricField = ElectricField(
        // p polarized part is lost due to a fully polarizing reflection at brewsters angle
        {0, 0},

        {0, 0},

        // s polarized part
        amplitude.s * incidentElectricField.z);

    CHECK_EQ(reflectElectricField, expectedReflectElectricField);
}

// non-polarizing reflection (reflection at normal incidence)
TEST_F(TestSuite, testInterceptReflectNonPolarizing) {
    using namespace complex;

    const auto iorI = Complex(1.0, 0);
    const auto refractIor = Complex(1.5, 0);
    const auto incidentVec = glm::normalize(glm::dvec3(1, 1, 0));
    const auto normalVec = -incidentVec;
    const auto reflectVec = glm::reflect(incidentVec, normalVec);
    const auto incidentElectricField = ElectricField({0, 0}, {0, 0}, {1, 0});

    const auto reflectElectricField = interceptReflect(incidentElectricField, incidentVec, reflectVec, normalVec, iorI, refractIor);

    const auto expected_reflect_field = ElectricField({0, 0}, {0, 0}, {-0.2, 0});

    CHECK_EQ(reflectElectricField, expected_reflect_field);

    // check if normal-incidence and near-normal-incidence are similar to each other
    {
        const auto eps = 1e-3;
        const auto normalVec = glm::normalize(-incidentVec + eps);
        const auto reflectVec = glm::reflect(incidentVec, normalVec);

        const auto reflectElectricField = interceptReflect(incidentElectricField, incidentVec, reflectVec, normalVec, iorI, refractIor);

        CHECK_EQ(reflectElectricField, expected_reflect_field, eps);
    }
}

// TODO(Sven): rethink this test
// TEST_F(TestSuite, testStokesToElectricFieldAndElectricFieldToStokes) {
//     using namespace complex;
//
//     struct InOutPair {
//         Stokes stokes;
//         LocalElectricField field;
//     };
//
//     const auto diag = 1.0 / glm::sqrt(2.0);
//
//     std::vector<InOutPair> inouts{
//         {
//             // linearly polarized (horizontal)
//             .stokes = Stokes(1, 1, 0, 0),
//             .field = LocalElectricField({1, 0}, {0, 0}),
//         },
//         {
//             // linearly polarized (vertical)
//             .stokes = Stokes(1, -1, 0, 0),
//             .field = LocalElectricField({0, 0}, {1, 0}),
//         },
//         {
//             // linearly polarized (diagonal +45 degrees)
//             .stokes = Stokes(1, 0, 1, 0),
//             .field = LocalElectricField({diag, 0}, {diag, 0}),
//         },
//         {
//             // linearly polarized (diagonal -45 degrees)
//             .stokes = Stokes(1, 0, -1, 0),
//             .field = LocalElectricField({diag, 0}, {-diag, 0}),
//         },
//         {
//             // circular polarized (right) (clockwise)
//             .stokes = Stokes(1, 0, 0, 1),
//             .field = LocalElectricField({diag, 0}, {0, -diag}),
//         },
//         {
//             // circular polarized (left) (counter-clockwise)
//             .stokes = Stokes(1, 0, 0, -1),
//             .field = LocalElectricField({diag, 0}, {0, diag}),
//         },
//     };
//
//     for (auto p : inouts) {
//         // stokes must be polarized in order to convert to an electric field
//         CHECK_EQ(degreeOfPolarization(p.stokes), 1.0);
//
//         // convert stokes to field and back and check if stokes are equal to initial stokes
//         {
//             const auto field = stokesToLocalElectricField(p.stokes);
//             CHECK_EQ(intensity(field), intensity(p.stokes));
//
//             const auto stokes = fieldToStokes(field);
//             CHECK_EQ(intensity(stokes), intensity(p.stokes));
//             CHECK_EQ(stokes, p.stokes);
//
//             // lets shift the phase of our field and check if the stokes are still preserved,
//             // since stokes should not carry phase information
//             const auto mag = abs(field);
//             const auto theta = arg(field);
//             for (double shift = 0.0; shift < PI * 2.0; shift += PI / 5.0) {
//                 const auto field = polar(mag, theta + shift);
//                 const auto stokes = fieldToStokes(field);
//                 CHECK_EQ(stokes, p.stokes);
//             }
//         }
//
//         // convert field to stokes and back and check if magnitude and polarization is preserved
//         // the phase is not being checked, because stokes parameters do not preserve phase
//         {
//             const auto stokes = fieldToStokes(p.field);
//             CHECK_EQ(intensity(stokes), intensity(p.field));
//             CHECK_EQ(stokes, p.stokes);
//
//             const auto field = stokesToLocalElectricField(stokes);
//
//             // check if the magnitude is preserved
//             const auto mag = abs(field);
//             const auto expected_mag = abs(p.field);
//             CHECK_EQ(mag, expected_mag);
//
//             // check if polarization is preserved
//             // by checking if the difference in phase between x and y component is preserved
//             const auto theta = arg(field);
//             const auto delta = theta.x - theta.y;
//
//             const auto expected_theta = arg(p.field);
//             const auto expected_delta = expected_theta.x - expected_theta.y;
//
//             CHECK_EQ(delta, expected_delta);
//         }
//     }
// }

// both the Cpp code as well as the shader define hvlam.
TEST_F(TestSuite, testHvlam) {
    double hv = 100;
    double linedensity = 1000;
    double orderOfDiff = 1;
    double a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    CHECK_EQ(a, 0.012398419843320024);

    a = abs(hvlam(hv)) * abs(linedensity) * orderOfDiff * 1e-06;
    CHECK_EQ(a, 0.012398419843320024);
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
    auto mat = createMaterialTables({Material::Cu, Material::Au});

    int Cu = static_cast<int>(Material::Cu);
    CHECK_EQ(getPalikEntryCount(Cu, mat.indices.data()), 324);

    auto Cu0 = getPalikEntry(0, Cu, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Cu0.m_energy, 1.0);
    CHECK_EQ(Cu0.m_n, 0.433);
    CHECK_EQ(Cu0.m_k, 8.46);

    auto Cu10 = getPalikEntry(10, Cu, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Cu10.m_energy, 2.3);
    CHECK_EQ(Cu10.m_n, 1.04);
    CHECK_EQ(Cu10.m_k, 2.59);

    int Au = static_cast<int>(Material::Au);
    CHECK_EQ(getPalikEntryCount(Au, mat.indices.data()), 386);

    auto Au0 = getPalikEntry(0, Au, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Au0.m_energy, 0.04959);
    CHECK_EQ(Au0.m_n, 20.3);
    CHECK_EQ(Au0.m_k, 76.992);

    auto Au10 = getPalikEntry(10, Au, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Au10.m_energy, 0.11158);
    CHECK_EQ(Au10.m_n, 12.963);
    CHECK_EQ(Au10.m_k, 57.666);
}

TEST_F(TestSuite, testNff) {
    auto mat = createMaterialTables({Material::Cu, Material::Au});

    int Cu = static_cast<int>(Material::Cu);
    CHECK_EQ(getNffEntryCount(Cu, mat.indices.data()), 504);

    auto Cu0 = getNffEntry(0, Cu, mat.indices.data(), mat.materials.data());

    CHECK_EQ(Cu0.m_energy, 10.0);
    CHECK_EQ(Cu0.m_f1, -9999.0);
    CHECK_EQ(Cu0.m_f2, 1.30088);

    auto Cu10 = getNffEntry(10, Cu, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Cu10.m_energy, 11.7404);
    CHECK_EQ(Cu10.m_f1, -9999.0);
    CHECK_EQ(Cu10.m_f2, 1.66946);

    int Au = static_cast<int>(Material::Au);
    CHECK_EQ(getNffEntryCount(Au, mat.indices.data()), 506);

    auto Au0 = getNffEntry(0, Au, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Au0.m_energy, 10.0);
    CHECK_EQ(Au0.m_f1, -9999.0);
    CHECK_EQ(Au0.m_f2, 1.73645);

    auto Au10 = getNffEntry(10, Au, mat.indices.data(), mat.materials.data());
    CHECK_EQ(Au10.m_energy, 11.7404);
    CHECK_EQ(Au10.m_f1, -9999.0);
    CHECK_EQ(Au10.m_f2, 2.67227);
}

TEST_F(TestSuite, testRefractiveIndex) {
    auto mat = createMaterialTables({Material::Cu, Material::Au});

    // vacuum
    CHECK_EQ(getRefractiveIndex(42.0, -1, mat.indices.data(), mat.materials.data()), glm::dvec2(1.0, 0.0));

    // palik tests for Cu
    // data taken from Data/PALIK
    CHECK_EQ(getRefractiveIndex(1.0, 29, mat.indices.data(), mat.materials.data()), glm::dvec2(0.433, 8.46));
    CHECK_EQ(getRefractiveIndex(1.8, 29, mat.indices.data(), mat.materials.data()), glm::dvec2(0.213, 4.05));
    CHECK_EQ(getRefractiveIndex(1977.980, 29, mat.indices.data(), mat.materials.data()), glm::dvec2(1.000032, 9.4646668E-05));

    // nff tests for Cu
    // data taken from
    // https://refractiveindex.info/?shelf=main&book=Cu&page=Hagemann
    CHECK_EQ(getRefractiveIndex(25146.2, 29, mat.indices.data(), mat.materials.data()), glm::dvec2(1.0, 1.0328e-7), 1e-5);
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
    const auto src = beamline.getSources()[0];
    DipoleSource dipolesource(*src);

    for (auto values : inouts) {
        auto result = dipolesource.bessel(values.proportion, values.zeta);
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
    const auto src = beamline.getSources()[0];

    DipoleSource dipolesource(*src);

    for (auto values : inouts) {
        auto result = dipolesource.schwinger(values.energy);
        CHECK_EQ(result, values.flux, 0.000000001);
    }
}

TEST_F(TestSuite, testSphericalCoords) {
    std::vector<glm::dvec3> directions = {
        {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0},
    };

    for (auto dir : directions) {
        double phi, psi;
        directionToSphericalCoords(dir, phi, psi);

        glm::dvec3 dir2;
        sphericalCoordsToDirection(phi, psi, dir2);

        CHECK_EQ(dir, dir2, 1e-11);
    }
}
