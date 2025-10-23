#include <fstream>

#include "Shader/LightSources/DipoleSource.h"
#include "setupTests.h"

void checkEnergyDistribution(const Rays& rays, double photonEnergy, double energySpread) {
    CHECK(rays.energy.size() > 0);
    for (const auto energy : rays.energy) { CHECK_IN(energy, photonEnergy - energySpread, photonEnergy + energySpread); }
}

void checkZDistribution(const Rays& rays, double center, double spread) {
    CHECK(rays.position_z.size() > 0);
    for (const auto position_z : rays.position_z) { CHECK_IN(position_z, center - spread, center + spread); }
}

void checkPositionDistribution(const Rays& rays, double sourceWidth, double sourceHeight) {
    CHECK(rays.position_x.size() > 0);
    for (const auto position_x : rays.position_x) CHECK_IN(position_x, -4.5 * sourceWidth, 4.5 * sourceWidth);
    CHECK(rays.position_y.size() > 0);
    for (const auto position_y : rays.position_y) CHECK_IN(position_y, -4.5 * sourceHeight, 4.5 * sourceHeight);
}

// should only be called on beamlines for which the ImagePlane has the default "unrotated" orientation!
void checkDirectionDistribution(const Rays& rays, double minAngle, double maxAngle) {
    CHECK(rays.direction_y.size() > 0);
    for (const auto direction_y : rays.direction_y) {
        double psi = asin(direction_y);
        psi        = abs(psi);
        CHECK_IN(psi, minAngle, maxAngle);
    }
}

TEST_F(TestSuite, MatrixSource) {
    auto a = traceRml("MatrixSource").filterByObjectId(0);
    auto b = loadCsvRayUi("MatrixSource");
    compareRayUiCompatible(a, b);
}

TEST_F(TestSuite, SourceEventsAreInLocalCoordinates) {
    // no matter if the source was moved, the events are in local coordinates
    auto a = traceRml("MatrixSourceMoved").filterByObjectId(0);
    auto b = loadCsvRayUi("MatrixSource");
    compareRayUiCompatible(a, b);
}

/// this tests tracing an only-lightsource beamline. An error-prone edge case.
TEST_F(TestSuite, MatrixSourceTracedRayUI) {
    const auto numRayPaths = loadBeamline("MatrixSource").numRayPaths();
    CHECK_EQ(traceRml("MatrixSource", RayAttrMask::PathId /* which attribute does not matter */).size(), numRayPaths);
}

TEST_F(TestSuite, PointSourceHardEdge) { checkEnergyDistribution(traceRml("PointSourceHardEdge", RayAttrMask::Energy), 120.97, 12.1); }

TEST_F(TestSuite, PointSourceSoftEdge) { checkEnergyDistribution(traceRml("PointSourceSoftEdge", RayAttrMask::Energy), 151, 6); }

TEST_F(TestSuite, MatrixSourceEnergyDistribution) { checkEnergyDistribution(traceRml("PointSourceSoftEdge", RayAttrMask::Energy), 151, 6); }

TEST_F(TestSuite, DipoleSourcePosition) {
    checkPositionDistribution(traceRml("dipole_plain", RayAttrMask::PositionX | RayAttrMask::PositionY), 0.065, 0.04);
}

TEST_F(TestSuite, DipoleEnergyDistribution) { checkEnergyDistribution(traceRml("dipole_energySpread", RayAttrMask::Energy), 1000, 23000); }

TEST_F(TestSuite, PixelPositionTest) {
    const auto [beamline, rays] =
        loadBeamlineAndTrace("PixelSource", RayAttrMask::PositionX | RayAttrMask::PositionY | RayAttrMask::DirectionX | RayAttrMask::DirectionZ);
    const DesignSource* src = beamline.getSources()[0];
    auto width              = src->getSourceWidth();
    auto height             = src->getSourceHeight();
    auto hordiv             = src->getHorDivergence();

    for (const auto position_x : rays.position_x) { CHECK_IN(position_x, -width / 2.0, width / 2.0); }
    for (const auto position_y : rays.position_y) { CHECK_IN(position_y, -height / 2.0, height / 2.0); }
    for (int i = 0; i < rays.size(); ++i) {
        double phi = atan2(rays.direction_x[i], rays.direction_z[i]);  // phi in rad from m_direction
        CHECK_IN(abs(phi), 0.0, hordiv / 2.0);
    }
}

TEST_F(TestSuite, DipoleZDistribution) { checkZDistribution(traceRml("dipole_plain", RayAttrMask::PositionZ), 0, 2.2); }

TEST_F(TestSuite, CircleSourcetest) {
    const auto rays = traceRml("CircleSource_default", RayAttrMask::PositionX | RayAttrMask::PositionY | RayAttrMask::Energy | RayAttrMask::ObjectId)
                          .filterByObjectId(0);
    checkPositionDistribution(rays, 0.065, 0.04);
    checkEnergyDistribution(rays, 99.5, 100.5);
}

TEST_F(TestSuite, testCircleSourceDirections) { checkDirectionDistribution(traceRml("CircleSource_default", RayAttrMask::DirectionY), 0.0, 105.0); }

TEST_F(TestSuite, testInterpolationFunctionDipole) {
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

TEST_F(TestSuite, testRayListSource) {
    // generate rays from some other source
    auto matrixSourceBeamline = loadBeamline("MatrixSource");
    auto expectedNumRayPaths  = matrixSourceBeamline.numRayPaths();
    EXPECT_GE(expectedNumRayPaths, 1);  // at least one ray would be nice
    auto inputRays = tracer->trace(matrixSourceBeamline);
    EXPECT_EQ(inputRays.size(), expectedNumRayPaths);

    // create a RayListSource, using the rays generated previously
    auto rayListSource = std::make_unique<DesignSource>("testRayListSource");
    rayListSource->setType(ElementType::RayListSource);
    rayListSource->setRayList(inputRays.copy());
    rayListSource->setNumberOfRays(expectedNumRayPaths);
    // add the RayListSource to a beamline that comes without a source
    auto beamline = loadBeamline("NoSource");
    beamline.addChild(std::move(rayListSource));
    EXPECT_EQ(beamline.numRayPaths(), expectedNumRayPaths);

    // trace the beamline with the RayListSource added
    auto rays = tracer->trace(beamline);
    // test if the number of rays is ok
    EXPECT_EQ(rays.filterByObjectId(2).size(), expectedNumRayPaths);
    // test if the source id is 0
    expectEqual(rays.source_id, 0);
    // test if the generated rays from the first beamline are equal to the source rays from RayListSource
    auto allAttrExceptPathEventId = exclude(RayAttrMask::All, RayAttrMask::PathEventId);
    compare(rays.filterByObjectId(0), inputRays, allAttrExceptPathEventId, DEFAULT_TOLERANCE);
}
