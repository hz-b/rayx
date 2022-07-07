#include "Tracer/CpuTracer.h"
#include "setupTests.h"

// TODO(rudi): shader tests

namespace RAYX {
namespace CPP_TRACER {
double r8_exp(double);
double r8_log(double);
double squaresDoubleRNG(uint64_t&);
Ray refrac2D(Ray, glm::dvec4, double, double);
Ray refrac(Ray, glm::dvec4, double);
glm::dvec4 normal_cartesian(glm::dvec4, double, double);
glm::dvec4 normal_cylindrical(glm::dvec4, double, double);
double wasteBox(double, double, double, double, double);
void RZPLineDensity(Ray r, glm::dvec4 normal, int IMAGE_TYPE, int RZP_TYPE,
                    int DERIVATION_METHOD, double zOffsetCenter, double risag,
                    double rosag, double rimer, double romer, double alpha,
                    double beta, double Ord, double WL, double& DX, double& DZ);

}  // namespace CPP_TRACER
}  // namespace RAYX

using namespace RAYX;

TEST_F(TestSuite, testUniformRandom) {
    uint64_t ctr = 13;
    double old = 0;

    for (int i = 0; i < 100; i++) {
        double d = CPP_TRACER::squaresDoubleRNG(ctr);
        if (d == old) {
            RAYX_WARN << "repeating number in testUniformRandom! " << d;
        }
        if (d < 0.0 || d > 1.0) {
            RAYX_ERR << "random number out of range [0, 1]: " << d;
        }
        old = d;
    }
}

TEST_F(TestSuite, ExpTest) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
    for (auto x : args) {
        CHECK_EQ(CPP_TRACER::r8_exp(x), exp(x));
        CHECK_EQ(CPP_TRACER::r8_exp(-x), exp(-x));
    }
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, LogTest) {
    std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
    for (auto x : args) {
        CHECK_EQ(CPP_TRACER::r8_log(x), log(x));
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

        auto out = CPP_TRACER::refrac2D(r, normal, az, ax);

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
            CPP_TRACER::normal_cartesian(p.in_normal, p.in_slopeX, p.in_slopeZ);
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
        auto out = CPP_TRACER::normal_cylindrical(p.in_normal, p.in_slopeX,
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

        auto out = CPP_TRACER::refrac(r, normal, a);

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
        auto out = CPP_TRACER::wasteBox(p.in_x, p.in_z, p.in_xLength,
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
        double in_DX;
        double in_DZ;

        double out_DX;
        double out_DZ;
    };

    std::vector<InOutPair> inouts;

    for (auto p : inouts) {
        CPP_TRACER::RZPLineDensity(
            p.in_ray, p.in_normal, p.in_imageType, p.in_rzpType,
            p.in_derivationMethod, p.in_zOffsetCenter, p.in_risag, p.in_rosag,
            p.in_rimer, p.in_romer, p.in_alpha, p.in_beta, p.in_Ord, p.in_WL,
            p.in_DX, p.in_DZ);
        double out_DX = p.in_DX;  // p.in_DX was mutated by RZPLineDensity.
        double out_DZ = p.in_DZ;  // p.in_DZ was mutated by RZPLineDensity.
        CHECK_EQ(out_DX, p.out_DX);
        CHECK_EQ(out_DZ, p.out_DZ);
    }
}