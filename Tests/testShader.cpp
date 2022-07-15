#include "Tracer/CpuTracer.h"
#include "setupTests.h"

// TODO(rudi): shader tests

using namespace RAYX;

TEST_F(TestSuite, testUniformRandom) {
    uint64_t ctr = 13;
    double old = 0;

    for (int i = 0; i < 100; i++) {
        double d = CPU_TRACER::squaresDoubleRNG(ctr);
        if (d == old) {
            RAYX_WARN << "repeating number in testUniformRandom! " << d;
        }
        if (d < 0.0 || d > 1.0) {
            RAYX_ERR << "random number out of range [0, 1]: " << d;
        }
        old = d;
    }
}

TEST_F(TestSuite, testSin) {
    std::vector<double> args = {
        -0.5620816275750421,  -0.082699735953560394, -0.73692442452247864,
        -0.93085577907030514, 0.038832744045494971,  0.86938579245347758,
        0.35772943348137098,  0.059400386282114415,  0.86087298993938566,
        0.3735454248180905,   -0.8663155254748649,   -0.98460362787680167,
        0.40238118899835329,  0.3078379243610454,    0.053857555519812195,
        0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(CPU_TRACER::r8_sin(x), sin(x));
    }
}

TEST_F(TestSuite, testCos) {
    std::vector<double> args = {
        -0.5620816275750421,  -0.082699735953560394, -0.73692442452247864,
        -0.93085577907030514, 0.038832744045494971,  0.86938579245347758,
        0.35772943348137098,  0.059400386282114415,  0.86087298993938566,
        0.3735454248180905,   -0.8663155254748649,   -0.98460362787680167,
        0.40238118899835329,  0.3078379243610454,    0.053857555519812195,
        0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(CPU_TRACER::r8_cos(x), cos(x));
    }
}

TEST_F(TestSuite, testAtan) {
    std::vector<double> args = {
        -0.5620816275750421,  -0.082699735953560394, -0.73692442452247864,
        -0.93085577907030514, 0.038832744045494971,  0.86938579245347758,
        0.35772943348137098,  0.059400386282114415,  0.86087298993938566,
        0.3735454248180905,   -0.8663155254748649,   -0.98460362787680167,
        0.40238118899835329,  0.3078379243610454,    0.053857555519812195,
        0.5128209722651722,
    };

    for (auto x : args) {
        CHECK_EQ(CPU_TRACER::r8_atan(x), atan(x));
    }
}

TEST_F(TestSuite, testExp) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
    for (auto x : args) {
        CHECK_EQ(CPU_TRACER::r8_exp(x), exp(x));
        CHECK_EQ(CPU_TRACER::r8_exp(-x), exp(-x));
    }
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, testLog) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
    for (auto x : args) {
        CHECK_EQ(CPU_TRACER::r8_log(x), log(x));
    }
}

TEST_F(TestSuite, testRefrac2D) {
    std::vector<Ray> input = {
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(0.0001666666635802469, -0.017285764670739875,
                           0.99985057611723738),
            .m_energy = 0,
            .m_stokes =
                glm::dvec4(0.00016514977645243345, 0.012830838024391771, 0, 0),
        },
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(0.00049999999722222276, -0.017285762731583675,
                           0.99985046502305308),
            .m_energy = 0,
            .m_stokes =
                glm::dvec4(-6.2949352042540596e-05, 0.038483898782123105, 0, 0),
        },
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(0.0001666666635802469, -0.017619047234249029,
                           0.99984475864845179),
            .m_energy = 0,
            .m_stokes =
                glm::dvec4(-0.077169530850327184, 0.2686127340088395, 0, 0),
        },
        {
            .m_position = glm::dvec3(0.050470500672820856, 0.95514062789960541,
                                     -0.29182033770349547),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(-0.00049999991666666705, -0.016952478247434233,
                           0.99985617139734351),
            .m_energy = 0,
            .m_stokes =
                glm::dvec4(0.0021599283476277926, -0.050153240660177005, 0, 0),
        },
    };

    // the correct rays should only be altered in weight & direction.
    std::vector<Ray> correct = input;

    correct[0].m_weight = 1;
    correct[0].m_direction = glm::dvec3(
        -0.012664171360811521, 0.021648721107426414, 0.99968542634078494);

    correct[1].m_weight = 0;
    correct[1].m_direction = glm::dvec3(
        0.00049999999722222276, -0.017285762731583675, 0.99985046502305308);

    correct[2].m_weight = 0;
    correct[2].m_direction = glm::dvec3(
        0.0001666666635802469, -0.017619047234249029, 0.99984475864845179);

    correct[3].m_weight = 1;
    correct[3].m_direction = glm::dvec3(
        0.080765992839840872, 0.57052382524991363, 0.81730007905468893);

    CHECK_EQ(input.size(), correct.size());
    for (uint i = 0; i < input.size(); i++) {
        auto r = input[i];

        glm::dvec4 normal = glm::dvec4(r.m_position, 0);
        double az = r.m_stokes.x;
        double ax = r.m_stokes.y;

        auto out = CPU_TRACER::refrac2D(r, normal, az, ax);

        CHECK_EQ(out, correct[i]);
    }
}

TEST_F(TestSuite, testNormalCartesian) {
    struct InOutPair {
        glm::dvec4 in_normal;
        double in_slopeX;
        double in_slopeZ;

        glm::dvec4 out;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_slopeX = 0,
            .in_slopeZ = 0,
            .out = glm::dvec4(0, 1, 0, 0),
        },
        {
            .in_normal = glm::dvec4(5.0465463027123736, 10470.451695989539,
                                    -28.532199794465537, 0),
            .in_slopeX = 0,
            .in_slopeZ = 0,
            .out = glm::dvec4(5.0465463027123736, 10470.451695989539,
                              -28.532199794465537, 0),
        },
        {
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_slopeX = 2,
            .in_slopeZ = 3,
            .out = glm::dvec4(-0.90019762973551742, 0.41198224566568298,
                              -0.14112000805986721, 0),
        },
        {
            .in_normal = glm::dvec4(5.0465463027123736, 10470.451695989539,
                                    -28.532199794465537, 0),
            .in_slopeX = 2,
            .in_slopeZ = 3,
            .out = glm::dvec4(-9431.2371568647086, 4310.7269916467494,
                              -1449.3435640204684, 0),
        }};

    for (auto p : inouts) {
        auto out =
            CPU_TRACER::normal_cartesian(p.in_normal, p.in_slopeX, p.in_slopeZ);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testNormalCylindrical) {
    struct InOutPair {
        glm::dvec4 in_normal;
        double in_slopeX;
        double in_slopeZ;

        glm::dvec4 out;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_slopeX = 0,
            .in_slopeZ = 0,
            .out = glm::dvec4(0, 1, 0, 0),
        },
        {
            .in_normal = glm::dvec4(5.0465463027123736, 10470.451695989539,
                                    -28.532199794465537, 0),
            .in_slopeX = 0,
            .in_slopeZ = 0,
            .out = glm::dvec4(5.0465463027115769, 10470.451695989539,
                              -28.532199794465537, 0),
        },
        {
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_slopeX = 2,
            .in_slopeZ = 3,
            .out = glm::dvec4(0.90019762973551742, 0.41198224566568292,
                              -0.14112000805986721, 0),
        },
        {.in_normal = glm::dvec4(5.0465463027123736, 10470.451695989539,
                                 -28.532199794465537, 0),
         .in_slopeX = 2,
         .in_slopeZ = 3,
         .out = glm::dvec4(9431.2169472441783, 4310.7711493493844,
                           -1449.3437356459144, 0)}};

    for (auto p : inouts) {
        auto out = CPU_TRACER::normal_cylindrical(p.in_normal, p.in_slopeX,
                                                  p.in_slopeZ);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testRefrac) {
    std::vector<Ray> input = {
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(-0.00049999991666667084, -0.99558611855684065,
                           0.093851108341926226),
            .m_energy = 0.01239852,
        },
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction = glm::dvec3(-1.6666664506172892e-05,
                                      -0.995586229182718, 0.093851118714515264),
            .m_energy = 0.01239852,
        },
        {
            .m_position = glm::dvec3(0.0027574667592826954, 0.99999244446428082,
                                     -0.0027399619384214182),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(-0.00049999991666667084, -0.99558611855684065,
                           0.093851108341926226),
            .m_energy = 0.01239852,
        },
        {
            .m_position = glm::dvec3(0, 1, 0),
            .m_weight = 1,
            .m_direction =
                glm::dvec3(-0.99991341437509562, 0.013149667401360443,
                           -0.00049999997222215965),
            .m_energy = -0.038483898782123105,
        },
    };

    // the correct rays should only be altered in weight & direction.
    std::vector<Ray> correct = input;

    correct[0].m_weight = 1;
    correct[0].m_direction = glm::dvec3(
        -0.00049999991666667084, 0.99667709206767885, 0.08145258834192623);

    correct[1].m_weight = 1;
    correct[1].m_direction = glm::dvec3(
        -1.6666664506160695e-05, 0.9966772027014974, 0.081452598714515267);

    correct[2].m_weight = 1;
    correct[2].m_direction = glm::dvec3(
        0.0049947959329671825, 0.99709586573547515, 0.07599267429701162);

    correct[3].m_weight = 0;
    correct[3].m_direction = glm::dvec3(
        -0.99991341437509562, 0.013149667401360443, -0.00049999997222215965);

    CHECK_EQ(input.size(), correct.size());
    for (uint i = 0; i < input.size(); i++) {
        auto r = input[i];

        glm::dvec4 normal = glm::dvec4(r.m_position, 0);
        double a = r.m_energy;

        auto out = CPU_TRACER::refrac(r, normal, a);

        CHECK_EQ(out, correct[i]);
    }
}

TEST_F(TestSuite, testWasteBox) {
    struct InOutPair {
        double in_x;
        double in_z;
        double in_xLength;
        double in_zLength;
        double in_w;

        double out;
    };

    std::vector<InOutPair> inouts = {{
                                         .in_x = -5.0466620698997637,
                                         .in_z = 28.760236725599515,
                                         .in_xLength = 50,
                                         .in_zLength = 200,
                                         .in_w = 1,
                                         .out = 1,
                                     },
                                     {
                                         .in_x = -5.0466620698997637,
                                         .in_z = 28.760236725599515,
                                         .in_xLength = 5,
                                         .in_zLength = 20,
                                         .in_w = 1,
                                         .out = 0,
                                     },
                                     {
                                         .in_x = -1.6822205656320104,
                                         .in_z = 28.760233508097873,
                                         .in_xLength = 5,
                                         .in_zLength = 20,
                                         .in_w = 1,
                                         .out = 0,
                                     },
                                     {
                                         .in_x = -5.0466620698997637,
                                         .in_z = 28.760236725599515,
                                         .in_xLength = 50,
                                         .in_zLength = 200,
                                         .in_w = 0,
                                         .out = 0,
                                     }};

    for (auto p : inouts) {
        auto out = CPU_TRACER::wasteBox(p.in_x, p.in_z, p.in_xLength,
                                        p.in_zLength, p.in_w);

        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testRZPLineDensityDefaulParams) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        int in_imageType;
        int in_rzpType;
        int in_derivationMethod;
        double in_zOffsetCenter;
        double in_risag;
        double in_rosag;
        double in_rimer;
        double in_romer;
        double in_alpha;
        double in_beta;
        double in_Ord;
        double in_WL;

        double out_DX;
        double out_DZ;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_ray =
                {
                    .m_position =
                        glm::dvec3(-5.0805095016939532, 0, 96.032788311782269),
                    .m_direction = glm::dvec3(0, 1, 0),
                },
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_imageType = 0,
            .in_rzpType = 0,
            .in_derivationMethod = 0,
            .in_zOffsetCenter = 0,
            .in_risag = 100,
            .in_rosag = 500,
            .in_rimer = 100,
            .in_romer = 500,
            .in_alpha = 0.017453292519943295,
            .in_beta = 0.017453292519943295,
            .in_Ord = -1,
            .in_WL = 1.239852e-05,

            .out_DX = 3103.9106911246749,
            .out_DZ = 5.0771666330055218,
        },
        {.in_ray =
             {
                 .m_position =
                     glm::dvec3(-1.6935030407867075, 0, 96.032777495754004),
                 .m_direction = glm::dvec3(0, 1, 0),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 1034.8685185321938,
         .out_DZ = -13.320120179862876},
        {.in_ray = {.m_position =
                        glm::dvec3(-5.047050067282087, 4.4859372100394515,
                                   29.182033770349552),
                    .m_direction =
                        glm::dvec3(0.05047050067282087, 0.95514062789960552,
                                   -0.29182033770349552)},
         .in_normal = glm::dvec4(0.05047050067282087, 0.95514062789960552,
                                 -0.29182033770349552, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 4045.0989844091882,
         .out_DZ = -174.2085626048659},
        {.in_ray =
             {
                 .m_position =
                     glm::dvec3(-1.6802365843267262, 1.3759250917712356,
                                16.445931214643075),
                 .m_direction =
                     glm::dvec3(0.016802365843267261, 0.98624074908228765,
                                -0.16445931214643075),
             },
         .in_normal = glm::dvec4(0.016802365843267261, 0.98624074908228765,
                                 -0.16445931214643075, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 1418.1004208892475,
         .out_DZ = 253.09836635775162},
    };

    for (auto p : inouts) {
        double DX;
        double DZ;
        CPU_TRACER::RZPLineDensity(p.in_ray, p.in_normal, p.in_imageType,
                                   p.in_rzpType, p.in_derivationMethod,
                                   p.in_zOffsetCenter, p.in_risag, p.in_rosag,
                                   p.in_rimer, p.in_romer, p.in_alpha,
                                   p.in_beta, p.in_Ord, p.in_WL, DX, DZ);
        CHECK_EQ(DX, p.out_DX);
        CHECK_EQ(DZ, p.out_DZ);
    }
}

TEST_F(TestSuite, testRZPLineDensityAstigmatic) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        int in_imageType;
        int in_rzpType;
        int in_derivationMethod;
        double in_zOffsetCenter;
        double in_risag;
        double in_rosag;
        double in_rimer;
        double in_romer;
        double in_alpha;
        double in_beta;
        double in_Ord;
        double in_WL;

        double out_DX;
        double out_DZ;
    };

    std::vector<InOutPair> inouts = {

        {
            .in_ray =
                {
                    .m_position =
                        glm::dvec3(-5.0805095016939532, 0, 96.032788311782269),
                    .m_direction = glm::dvec3(0, 1, 0),
                },
            .in_normal = glm::dvec4(0, 1, 0, 0),
            .in_imageType = 0,
            .in_rzpType = 0,
            .in_derivationMethod = 0,
            .in_zOffsetCenter = 0,
            .in_risag = 100,
            .in_rosag = 500,
            .in_rimer = 100,
            .in_romer = 500,
            .in_alpha = 0.017453292519943295,
            .in_beta = 0.017453292519943295,
            .in_Ord = -1,
            .in_WL = 1.239852e-05,

            .out_DX = 3103.9106911246749,
            .out_DZ = 5.0771666330055218,
        },
        {.in_ray =
             {
                 .m_position =
                     glm::dvec3(-1.6935030407867075, 0, 96.032777495754004),
                 .m_direction = glm::dvec3(0, 1, 0),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 1034.8685185321938,
         .out_DZ = -13.320120179862876},
        {.in_ray = {.m_position =
                        glm::dvec3(-5.047050067282087, 4.4859372100394515,
                                   29.182033770349552),
                    .m_direction =
                        glm::dvec3(0.05047050067282087, 0.95514062789960552,
                                   -0.29182033770349552)},
         .in_normal = glm::dvec4(0.05047050067282087, 0.95514062789960552,
                                 -0.29182033770349552, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 4045.0989844091882,
         .out_DZ = -174.2085626048659},
        {.in_ray =
             {
                 .m_position =
                     glm::dvec3(-1.6802365843267262, 1.3759250917712356,
                                16.445931214643075),
                 .m_direction =
                     glm::dvec3(0.016802365843267261, 0.98624074908228765,
                                -0.16445931214643075),
             },
         .in_normal = glm::dvec4(0.016802365843267261, 0.98624074908228765,
                                 -0.16445931214643075, 0),
         .in_imageType = 0,
         .in_rzpType = 0,
         .in_derivationMethod = 0,
         .in_zOffsetCenter = 0,
         .in_risag = 100,
         .in_rosag = 500,
         .in_rimer = 100,
         .in_romer = 500,
         .in_alpha = 0.017453292519943295,
         .in_beta = 0.017453292519943295,
         .in_Ord = -1,
         .in_WL = 1.239852e-05,
         .out_DX = 1418.1004208892475,
         .out_DZ = 253.09836635775162},
    };

    for (auto p : inouts) {
        double DX;
        double DZ;
        CPU_TRACER::RZPLineDensity(p.in_ray, p.in_normal, p.in_imageType,
                                   p.in_rzpType, p.in_derivationMethod,
                                   p.in_zOffsetCenter, p.in_risag, p.in_rosag,
                                   p.in_rimer, p.in_romer, p.in_alpha,
                                   p.in_beta, p.in_Ord, p.in_WL, DX, DZ);
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
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                                    14, 15, 16),
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
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                                    14, 15, 16),
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
            .in_matrix = glm::dmat4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                                    14, 15, 16),
            .out_ray =
                {
                    .m_position = glm::dvec3(51, 58, 65),
                    .m_direction = glm::dvec3(83, 98, 113),
                },
        },

    };

    for (auto p : inouts) {
        auto out_ray = CPU_TRACER::rayMatrixMult(p.in_ray, p.in_matrix);
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
        auto out = CPU_TRACER::dpow(p.in_a, p.in_b);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testCosini) {
    struct InOutPair {
        double in_phi;
        double in_psi;

        glm::dvec3 out_direction;
    };

    std::vector<InOutPair> inouts = {
        {
            .in_phi = 0,
            .in_psi = 0,
            .out_direction =
                glm::dvec3(6.1257422745431001e-17, -6.1257422745431001e-17, 1),
        },
        {
            .in_phi = 1,
            .in_psi = 1,
            .out_direction = glm::dvec3(
                0.45464871341284091, -0.8414709848078965, 0.29192658172642888),
        },
        {
            .in_phi = 1,
            .in_psi = 0,
            .out_direction =
                glm::dvec3(0.8414709848078965, -6.1257422745431001e-17,
                           0.54030230586813977),
        },
        {
            .in_phi = 0,
            .in_psi = 1,
            .out_direction =
                glm::dvec3(3.3097526760895799e-17, -0.8414709848078965,
                           0.54030230586813977),
        },
        {
            .in_phi = 3.1415926535897931,
            .in_psi = 3.1415926535897931,
            .out_direction =
                glm::dvec3(-6.1257422745430988e-17, -6.1257422745431001e-17,
                           0.99999999999999978),
        },
        {
            .in_phi = 0,
            .in_psi = 1.5707963267948966,
            .out_direction =
                glm::dvec3(3.7524718414124473e-33, -1, 6.1257422745431001e-17),
        },

    };

    for (auto p : inouts) {
        Ray out_ray;
        CPU_TRACER::cosini(out_ray, p.in_phi, p.in_psi);
        CHECK_EQ(out_ray.m_direction, p.out_direction);
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
        auto out = CPU_TRACER::fact(p.in);
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
        auto out = CPU_TRACER::bessel1(p.in);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testDiffraction) {
    struct In {
        int in_iopt;
        double in_xLength;
        double in_yLength;
        double in_wl;
        uint64_t in_ctr;
    };

    std::vector<In> ins = {{
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 0UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 1844674407370955161UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 3689348814741910322UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 5534023222112865483UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 7378697629483820644UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 9223372036854775805UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 11068046444225730966UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 12912720851596686127UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 14757395258967641288UL,
                           },
                           {
                               .in_iopt = 1,
                               .in_xLength = 20,
                               .in_yLength = 2,
                               .in_wl = 12.39852,
                               .in_ctr = 16602069666338596449UL,
                           }

    };

    const double lowerDphi = 1e-11;
    const double upperDphi = 1e-05;
    const double lowerDpsi = 1e-09;
    const double upperDpsi = 1e-04;

    for (auto i : ins) {
        double dphi = 0;
        double dpsi = 0;
        CPU_TRACER::diffraction(i.in_iopt, i.in_xLength, i.in_yLength, i.in_wl,
                                dphi, dpsi, i.in_ctr);
        double abs_dphi = abs(dphi);
        double abs_dpsi = abs(dpsi);
        if (abs_dphi < lowerDphi || abs_dphi > upperDphi) {
            RAYX_ERR << "wrong dphi";
        }
        if (abs_dpsi < lowerDpsi || abs_dpsi > upperDpsi) {
            RAYX_ERR << "wrong dpsi";
        }
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
        auto out = CPU_TRACER::vlsGrating(p.in_lineDensity, p.in_z, p.in_vls);
        CHECK_EQ(out, p.out);
    }
}

TEST_F(TestSuite, testPlaneRefrac) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;
        double in_a;

        Ray out_ray;
    };

    std::vector<InOutPair> inouts = {
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(0, -0.99558611855684065, 0.093851108341926615),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(0, 0.99667709206767885, 0.081452588341926618),
             }},
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, -0.99558611855684065,
                                           0.093851108341926615),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, 0.99667709206767885,
                                           0.081452588341926618),
             }},
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, -0.99567947186812988,
                                           0.0928554753392902),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, 0.99675795875308415,
                                           0.080456955339290204),
             }},
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, -0.99567947186812988,
                                           0.0928554753392902),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction = glm::dvec3(0.01239852, 0.99675795875308415,
                                           0.080456955339290204),
             }},
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(-0.00049999991666660004, -0.99558611855684065,
                                0.093851108341926226),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(-0.00049999991666660004, 0.99667709206767885,
                                0.08145258834192623),
             }},
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(-0.00049999991666660004, -0.995586229182718,
                                0.093851118714515264),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .in_a = 0.01239852,
         .out_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_weight = 0.01239852,
                 .m_direction =
                     glm::dvec3(-0.00049999991666660004, 0.9966772027014974,
                                0.081452598714515267),
             }},

    };

    for (auto p : inouts) {
        auto out_ray = CPU_TRACER::refrac_plane(p.in_ray, p.in_normal, p.in_a);
        CHECK_EQ(out_ray, p.out_ray);
    }
}

TEST_F(TestSuite, testIteratTo) {
    struct InOutPair {
        Ray in_ray;
        double in_longRadius;
        double in_shortRadius;

        Ray out_ray;
        glm::dvec4 out_vec;
    };

    std::vector<InOutPair> inouts = {
        {.in_ray =
             {
                 .m_position =
                     glm::dvec3(-0.017500000000000002, 1736.4751598838836,
                                -9848.1551798768887),
                 .m_weight = 1,
                 .m_direction =
                     glm::dvec3(-0.00026923073232438285, -0.17315574581145807,
                                0.984894418304465),
             },
         .in_longRadius = 10470.491787499999,
         .in_shortRadius = 315.72395939400002,
         .out_ray =
             {
                 .m_position =
                     glm::dvec3(-2.7173752216893443, 0.050407875158271054,
                                28.473736158432885),
                 .m_weight = 1,
                 .m_direction =
                     glm::dvec3(-0.00026923073232438285, -0.17315574581145807,
                                0.984894418304465),
             },
         .out_vec = glm::dvec4(0.0086068071179840333, 0.99995926323042061,
                               -0.0027193291283604047, 0)},

    };

    for (auto p : inouts) {
        Ray r = p.in_ray;
        auto out_vec =
            CPU_TRACER::iteratTo(r, p.in_longRadius, p.in_shortRadius);
        CHECK_EQ(out_vec, p.out_vec);
        CHECK_EQ(r, p.out_ray);
    }
}

TEST_F(TestSuite, testGetIncidenceAngle) {
    struct InOutPair {
        Ray in_ray;
        glm::dvec4 in_normal;

        double out;
    };

    std::vector<InOutPair> inouts = {
        {.in_ray =
             {
                 .m_position = glm::dvec3(0, 1, 0),
                 .m_direction =
                     glm::dvec3(-0.00049999997222222275, -0.17381228817387082,
                                0.98477867487054738),
             },
         .in_normal = glm::dvec4(0, 1, 0, 0),
         .out = 1.3960967569703167}};

    for (auto p : inouts) {
        auto out = CPU_TRACER::getIncidenceAngle(p.in_ray, p.in_normal);
        CHECK_EQ(out, p.out);
    }
}

// TODO(rudi): this test is and was broken before the refactor. See the nans.
TEST_F(TestSuite, testReflectance) {
    {  // add copper material to cpu tracer
        std::array<bool, 92> mats;
        mats.fill(false);
        mats[29] = true;  // copper is relevant!
        auto materialTables = loadMaterialTables(mats);
        CPU_TRACER::mat.data = materialTables.materialTable;
        CPU_TRACER::matIdx.data = materialTables.indexTable;
    }

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
        CPU_TRACER::reflectance(p.in_energy, p.in_incidenceAngle, complex_S,
                                complex_P, p.in_material);
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
        {.in_cosIncidence =
             glm::dvec2(0.16307067260397731, 0.0027314608130525712),
         .in_cn1 = glm::dvec2(0.99816471240025439, 0.00045674598468145697),
         .in_cn2 = glm::dvec2(0.99514154037883318, 0.0047593281563246184),
         .out = glm::dvec2(0.14743465849863333, 0.031766878855366699)},

    };

    for (auto p : inouts) {
        auto out = CPU_TRACER::snell(p.in_cosIncidence, p.in_cn1, p.in_cn2);
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
         .in_cosIncidence =
             glm::dvec2(0.10897754475504851, 0.40807275584607544),
         .in_cosTransmittance = glm::dvec2(0.17315572500228882, 0),
         .out_complex_S = glm::dvec2(0.57163467986230054, 0.62486367906829521),
         .out_complex_P = glm::dvec2(0.63080662811278632, 0.52640331936127849)},
        {.in_cn1 = glm::dvec2(0.91452118089946777, 0.035187568837614078),
         .in_cn2 = glm::dvec2(1, 0),
         .in_cosIncidence =
             glm::dvec2(0.10906363669865969, 0.40789272144618016),
         .in_cosTransmittance = glm::dvec2(0.1736481785774231, 0),
         .out_complex_S = glm::dvec2(0.56981824812215454, 0.62585833416785819),
         .out_complex_P = glm::dvec2(0.62929764490597007, 0.52731592442193231)},

    };

    for (auto p : inouts) {
        glm::dvec2 out_complex_S;
        glm::dvec2 out_complex_P;
        CPU_TRACER::fresnel(p.in_cn1, p.in_cn2, p.in_cosIncidence,
                            p.in_cosTransmittance, out_complex_S,
                            out_complex_P);
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
        {.in_complex = glm::dvec2(0.63080662811278621, 0.52640331936127871),
         .out = glm::dvec2(0.67501745670559532, 0.69542190922049119)},
        {.in_complex = glm::dvec2(0.57163467986230043, 0.62486367906829532),
         .out = glm::dvec2(0.71722082464004022, 0.82985616444880206)},
        {.in_complex = glm::dvec2(1, 0), .out = glm::dvec2(1, 0)},
        {.in_complex = glm::dvec2(0, 1),
         .out = glm::dvec2(1, 1.5707963267948966)}};

    for (auto p : inouts) {
        auto out = CPU_TRACER::cartesian_to_euler(p.in_complex);
        CHECK_EQ(out, p.out);
    }
}