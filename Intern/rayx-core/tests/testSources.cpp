#include <fstream>

#include "Shader/LightSources/DipoleSource.h"
#include "setupTests.h"

void checkEnergyDistribution(const std::vector<Ray>& rays, double photonEnergy, double energySpread) {
    for (auto r : rays) { CHECK_IN(r.m_energy, photonEnergy - energySpread, photonEnergy + energySpread); }
}

void checkZDistribution(const std::vector<Ray>& rays, double center, double spread) {
    for (auto r : rays) { CHECK_IN(r.m_position.z, center - spread, center + spread); }
}

void checkPositionDistribution(const std::vector<Ray>& rays, double sourceWidth, double sourceHeight) {
    for (auto r : rays) {
        CHECK_IN(r.m_position.x, -4.5 * sourceWidth, 4.5 * sourceWidth);
        CHECK_IN(r.m_position.y, -4.5 * sourceHeight, 4.5 * sourceHeight);
    }
}

// should only be called on beamlines for which the ImagePlane has the default "unrotated" orientation!
void checkDirectionDistribution(const std::vector<Ray>& rays, double minAngle, double maxAngle) {
    for (auto r : rays) {
        double psi = asin(r.m_direction.y);
        psi        = abs(psi);
        CHECK_IN(psi, minAngle, maxAngle);
    }
}

void roughCompare(std::vector<RAYX::Ray> l, std::vector<RAYX::Ray> r) {
    CHECK_EQ(l.size(), r.size());
    // TODO maybe compare more?
    for (int i = 0; i < l.size(); i++) {
        CHECK_EQ(l[i].m_position, r[i].m_position);
        CHECK_EQ(l[i].m_direction, r[i].m_direction);
        CHECK_EQ(l[i].m_energy, r[i].m_energy);
    }
}

TEST_F(TestSuite, MatrixSource) {
    ADD_FAILURE();
    // auto beamline = loadBeamline("MatrixSource");
    // auto a = beamline.compileSources();
    // auto b = loadCSVRayUI("MatrixSource");
    // roughCompare(a, b);
}

TEST_F(TestSuite, MatrixSourceMoved) {
    ADD_FAILURE();
    // auto beamline = loadBeamline("MatrixSourceMoved");
    // auto a = beamline.compileSources();
    // auto b = loadCSVRayUI("MatrixSource");
    // for (auto& r : b) {
    //     r.m_position += glm::dvec3(5, -5, 3);
    // }
    // roughCompare(a, b);
}

/// this tests tracing an only-lightsource beamline. An error-prone edge case.
TEST_F(TestSuite, MatrixSourceTracedRayUI) {
    ADD_FAILURE();
    // auto a = traceRML("MatrixSource");
    // for (auto hist : a) {
    //     CHECK(!hist.empty());
    // }
}

TEST_F(TestSuite, PointSourceHardEdge) {
    ADD_FAILURE();
    // auto rays = loadBeamline("PointSourceHardEdge").compileSources();
    // checkEnergyDistribution(rays, 120.97, 12.1);
}

TEST_F(TestSuite, PointSourceSoftEdge) {
    ADD_FAILURE();
    // auto rays = loadBeamline("PointSourceSoftEdge").compileSources();
    // checkEnergyDistribution(rays, 151, 6);
}

TEST_F(TestSuite, MatrixSourceEnergyDistribution) {
    ADD_FAILURE();
    // auto rays = loadBeamline("MatrixSourceSpreaded").compileSources();
    // checkEnergyDistribution(rays, 42, 10);
}

TEST_F(TestSuite, DipoleSourcePosition) {
    ADD_FAILURE();
    // auto rays = loadBeamline("dipole_plain").compileSources();
    // checkPositionDistribution(rays, 0.065, 0.04);
}

TEST_F(TestSuite, DipoleEnergyDistribution) {
    ADD_FAILURE();
    // auto rays = loadBeamline("dipole_energySpread").compileSources();
    // checkEnergyDistribution(rays, 1000, 23000);
}

TEST_F(TestSuite, PixelPositionTest) {
    ADD_FAILURE();
    // auto beamline = loadBeamline("PixelSource");
    // auto rays = beamline.compileSources();
    // const DesignSource* src = beamline.getSources()[0];
    // auto width = src->getSourceWidth();
    // auto height = src->getSourceHeight();
    // auto hordiv = src->getHorDivergence();
    // for (auto ray : rays) {
    //     CHECK_IN(abs(ray.m_position.x), width / 6.0, width / 2.0);
    //     CHECK_IN(abs(ray.m_position.y), height / 6.0, height / 2.0);
    //     double phi = atan2(ray.m_direction.x, ray.m_direction.z);  // phi in rad from m_direction
    //     CHECK_IN(abs(phi), 0.0, hordiv / 2.0);
    // }
}

TEST_F(TestSuite, DipoleZDistribution) {
    ADD_FAILURE();
    // auto beamline = loadBeamline("dipole_plain");
    // auto rays = beamline.compileSources();
    // checkZDistribution(rays, 0, 2.2);
}

TEST_F(TestSuite, CircleSourcetest) {
    ADD_FAILURE();
    // auto rays = loadBeamline("CircleSource_default").compileSources();
    // checkPositionDistribution(rays, 0.065, 0.04);
    // checkEnergyDistribution(rays, 99.5, 100.5);
}

TEST_F(TestSuite, testCircleSourceDirections) {
    auto bundle = traceRML("CircleSource_default");
    for (auto rays : bundle) { checkDirectionDistribution(rays, 0.0, 105.0); }
}

TEST_F(TestSuite, testInterpolationFunctionDipole) {
    ADD_FAILURE();
    struct InOutPair {
        double in;
        double out;
    };
    std::vector<InOutPair> inouts = {
        {
            .in  = 1.5298292375594387,
            .out = -3.5010758381905855,
        },
        {
            .in  = 2,
            .out = -6.0742663050458416,
        },
        {
            .in  = -1,
            .out = -0.095123518041340588,
        },
    };

    for (auto values : inouts) {
        auto result = getDipoleInterpolation(values.in);
        CHECK_EQ(result, values.out, 0.01);
    }
}

TEST_F(TestSuite, testVerDivergenceDipole) {
    ADD_FAILURE();
    struct InOutPair {
        double energy;
        double sigv;
        double out;
    };
    std::vector<InOutPair> inouts = {{
        .energy = 100,
        .sigv   = 1,
        .out    = 1.591581814000419,
    }};

    const auto electronEnergy = 1.7;
    const auto criticalEnergy = get_factorCriticalEnergy();

    for (auto values : inouts) {
        auto result = calcVerDivergence(values.energy, values.sigv, electronEnergy, criticalEnergy);
        CHECK_EQ(result, values.out, 0.1);
    }
}

TEST_F(TestSuite, testBesselDipole) {
    struct InOutPair {
        double proportion;
        double zeta;
        double out;
    };
    std::vector<InOutPair> inouts = {{
                                         .proportion = 1 / 3,
                                         .zeta       = 78.126966373103443,
                                         .out        = 1.664046593883771e-35,
                                     },
                                     {
                                         .proportion = 1 / 3,
                                         .zeta       = 73.550785975500432,
                                         .out        = 1.6659366793149262e-33,
                                     },
                                     {
                                         .proportion = 1 / 3,
                                         .zeta       = 46.422887861754496,
                                         .out        = 1.2672053903555623e-21,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta       = 78.126966373103443,
                                         .out        = 1.6675777760881476e-35,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta       = 73.550785975500432,
                                         .out        = 1.6696906039215801e-33,
                                     },
                                     {
                                         .proportion = 2 / 3,
                                         .zeta       = 49.798819164687949,
                                         .out        = 4.1969864622545434e-23,
                                     }};

    for (auto values : inouts) {
        auto result = dipoleBessel(values.proportion, values.zeta);
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
                                         .flux   = 566462407647095.5,
                                     },
                                     {.energy = 100, .flux = 2855336264551178},
                                     {
                                         .energy = 900,
                                         .flux   = 3762078406399219,
                                     },
                                     {
                                         .energy = 2000,
                                         .flux   = 2907004029317153.5,
                                     },
                                     {
                                         .energy = 0.667,
                                         .flux   = 596812742357665.25,
                                     },
                                     {
                                         .energy = 2456,
                                         .flux   = 2526853293939861,
                                     }};

    const auto electronEnergy = 1.7;
    const auto gamma          = calcGamma(electronEnergy);
    const auto criticalEnergy = get_factorCriticalEnergy();

    for (auto values : inouts) {
        auto result = schwinger(values.energy, gamma, criticalEnergy);
        CHECK_EQ(result, values.flux, 0.000000001);
    }
}

TEST_F(TestSuite, testLightsourceGetters) {
    struct RmlInput {
        std::string rmlFile;
        double horDivergence;
        double sourceHeight;
        double sourceWidth;
        double sourceDepth;
    };

    std::vector<RmlInput> rmlinputs = {{
        .rmlFile       = "PointSourceHardEdge",
        .horDivergence = 0.001,  // conversion /1000 in the parser
        .sourceHeight  = 0.04,
        .sourceWidth   = 0.065,
        .sourceDepth   = 1,
    }};
    for (auto values : rmlinputs) {
        auto beamline           = loadBeamline(values.rmlFile);
        const DesignSource* src = beamline.getSources()[0];

        auto test2        = values.horDivergence;
        auto test4        = values.sourceDepth;
        auto test6        = values.sourceWidth;
        auto horResult    = src->getHorDivergence();
        auto heightResult = src->getSourceHeight();
        auto widthResult  = src->getSourceWidth();

        CHECK_EQ(horResult, values.horDivergence);
        CHECK_EQ(heightResult, values.sourceHeight);
        CHECK_EQ(widthResult, values.sourceWidth);
    }
}

#ifndef NO_H5
TEST_F(TestSuite, testH5Writer) {
    const auto beamlineFilename     = "METRIX_U41_G1_H1_318eV_PS_MLearn_v114";
    const auto rayOriginal          = traceRML(beamlineFilename);
    const auto rayOriginalSoA       = bundleHistoryToRays(rayOriginal);
    const auto beamline             = loadBeamline(beamlineFilename);
    const auto sourceNamesOriginal  = beamline.getSourceNames();
    const auto elementNamesOriginal = beamline.getElementNames();

    // test conversion between BundleHistory and Rays
    {
        const auto bundle = raySoAToBundleHistory(rayOriginalSoA);
        CHECK_EQ(rayOriginal, bundle);
    }

    const auto h5Filepath = getBeamlineFilepath(beamlineFilename).replace_extension("h5");

    // test if write and read of BundleHistory work without altering the contents
    {
        writeH5BundleHistory(h5Filepath, sourceNamesOriginal, elementNamesOriginal, rayOriginal);
        const auto bundle = readH5BundleHistory(h5Filepath);
        CHECK_EQ(rayOriginal, bundle);

        const auto sourceNames = readH5SourceNames(h5Filepath);
        if (sourceNamesOriginal != sourceNames) ADD_FAILURE();

        const auto elementNames = readH5ElementNames(h5Filepath);
        if (elementNamesOriginal != elementNames) ADD_FAILURE();
    }

    // test if write and read of partial BundleHistory work without altering the contents
    {
        // ground thruth
        Rays partialRayOriginalSoA;
        partialRayOriginalSoA.energy     = rayOriginalSoA.energy;
        partialRayOriginalSoA.position_x = rayOriginalSoA.position_x;
        partialRayOriginalSoA.position_y = rayOriginalSoA.position_y;
        partialRayOriginalSoA.position_z = rayOriginalSoA.position_z;

        // write only some attributes
        const auto attr = RayAttrFlag::Energy | RayAttrFlag::Position;
        writeH5BundleHistory(h5Filepath, sourceNamesOriginal, elementNamesOriginal, rayOriginal, attr);
        // read only some attributes
        const auto raySoA = readH5Rays(h5Filepath, attr);

        CHECK_EQ(partialRayOriginalSoA, raySoA);
    }
}
#endif

TEST_F(TestSuite, testSelectElementForRecordEvent) {
    // const auto filename = std::filesystem::path("METRIX_U41_G1_H1_318eV_PS_MLearn_v114");
    // const auto beamline = loadBeamline(filename);
    // TODO: add test for recording events for selected element. also multiple beamlines. small -> bigger. big -> smaller buffers
    // - use fixed seed
    // - trace: record all events
    // - trace: record events from only one element
    // - compare
}
