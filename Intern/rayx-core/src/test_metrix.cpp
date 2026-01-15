#include "Design/Beamline.h"

using namespace rayx::design;
using namespace rayx::design::literals;
using namespace rayx::math;

std::shared_ptr<Beamline> createMetrixBeamline() {
    auto U41_318eV = PointSource{
        .numRays = 1 << 20,
        .rayOrigin =
            {
                .x = GaussianDistribution<double>{.standardDeviation = 0.076},
                .y = GaussianDistribution<double>{.standardDeviation = 0.027},
                .z = WhiteNoiseDistribution<double>{.range = 1.0},
            },
        .rayAngle =
            {
                .x = GaussianDistribution<Angle>{.standardDeviation = 0.078_rad},  // TODO: check
                .y = GaussianDistribution<Angle>{.standardDeviation = 0.04_rad},
            },
        .rayPhotonEnergy = 318.0_eV,
        .rayPolarization = Stokes{1, 1, 0, 0},
    };

    auto ASBL = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 100,
                .height = 100,
            },
        .behavior = AbsorbBehavior{},
        .aperture =
            DiffractiveAperture{
                .area =
                    RectangularArea{
                        .width  = 2,
                        .height = 2,
                    },
            },
    };

    auto M1Cylinder = SurfaceElement{.area =
                                         RectangularArea{
                                             .width  = 20,
                                             .height = 320,
                                         },
                                     .behavior =
                                         ReflectBehavior{
                                             .substrate = Material::Au,
                                         },
                                     .curvature =
                                         CylindricalCurvature{
                                             .direction         = CylinderDirection::ShortRadiusRho,
                                             .radius            = 174.21,
                                             .grazingIncAngle   = 1.75_rad,  // TODO: why would we need the grazing inc angle for a curvature?
                                             .entranceArmLength = 12000,     // TODO: why is this the exact distance to the source?
                                             .exitArmLength     = 3750,
                                         },
                                     .slopeError = SlopeError{
                                         .saggital   = 1.01,
                                         .meridional = 0.16,
                                     }};

    auto EntranceSlit = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 100,
                .height = 100,
            },
        .behavior = AbsorbBehavior{},
        .aperture =
            DiffractiveAperture{
                .area =
                    RectangularArea{
                        .width  = 10,
                        .height = 10,
                    },
            },
    };

    auto E1 = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 20,
                .height = 390,
            },
        .behavior =
            ReflectBehavior{
                .substrate = Material::Au,
            },
        .curvature =
            EllipticalCurvature{
                .shortHalfAxisB        = 302.721702601,
                .longHalfAxisA         = 20750,
                .entranceArmLength     = 39600,
                .exitArmLength         = 1900,
                .designGrazingIncAngle = 2.00735_rad,
                .figureRotation        = FigureRotation::Plane,
                .parameterA11          = 1,
            },
        .slopeError =
            SlopeError{
                .saggital   = 0.037,
                .meridional = 0.023,
            },
    };

    auto E2 = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 20,
                .height = 390,
            },
        .behavior =
            ReflectBehavior{
                .substrate = Material::Au,
            },
        .curvature =
            EllipticalCurvature{
                .shortHalfAxisB        = 97.1560870104,
                .longHalfAxisA         = 4375,
                .entranceArmLength     = 7750,
                .exitArmLength         = 1000,
                .designGrazingIncAngle = 2.00735_rad,
                .figureRotation        = FigureRotation::Plane,
                .parameterA11          = 1,
            },
        .slopeError =
            SlopeError{
                .saggital   = 0.045,
                .meridional = 0.024,
            },
    };

    auto PlaneMirror = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 20,
                .height = 310,
            },
        .behavior =
            ReflectBehavior{
                .substrate = Material::Au,
            },
        .slopeError =
            SlopeError{
                .saggital   = 0.084,
                .meridional = 0.023,
            },
    };

    auto SphericalGrating = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 20,
                .height = 100,
            },
        .behavior =
            GratingBehavior{
                .lineDensity        = 1199.918,
                .orderOfDiffraction = 1,
            },
        .slopeError =
            SlopeError{
                .saggital   = 0.124,
                .meridional = 0.102,
            },
    };

    auto ExitSlit = SurfaceElement{
        .area =
            RectangularArea{
                .width  = 100,
                .height = 100,
            },
        .behavior = AbsorbBehavior{},
        .aperture =
            DiffractiveAperture{
                .area =
                    RectangularArea{
                        .width  = 20,
                        .height = 0.1,
                    },
            },
    };

    auto ImagePlane = SurfaceElement{
        .area     = UnlimitedArea{},
        .behavior = DetectBehavior{},
    };

    auto metrix = std::make_shared<Beamline>("METRIX");

    metrix->append("U41_318eV_Source", U41_318eV);

    metrix->append(Translation{0, 0, 10000})
        ->append(RotationBase{{0.0, 1.0, 0.0}, {-0.9995335908367129, 0.0, -0.0305385132098227}, {-0.0305385132098227, 0.0, 0.9995335908367129}})
        ->append("M1_Cylinder", M1Cylinder);

    metrix->append(Translation{-228.9320232557132613, 0.0, 15743.0054940820009506})
        ->append(RotationBase{{0.9981347984218669, 0.0, 0.0610485395348569}, {0.0, 1.0, 0.0}, {-0.0610485395348569, 0.0, 0.9981347984218669}})
        ->append("EntranceSlit", EntranceSlit);

    metrix->append(Translation{-1684.7966035493911932, 30.5439200414999590, 39546.1809746561484644})
        ->append(RotationBase{{0.0000000499072879, 0.9999999999996659, -0.0000008159769440},
                              {-0.9953961983671795, -0.0000000285303996, -0.0958457525202124},
                              {-0.0958457525202036, 0.0000008170037496, 0.9953961983668456}})
        ->append("E1", E1);

    metrix->append(Translation{-1802.2701765474193962, 30.5446540008207528, 40438.4813498922812869})
        ->append(RotationBase{{0.9914448613738118, 0.0000000570260393, 0.1305261922200283},
                              {0.0045553482918587, 0.9993907985578609, -0.0346017421500882},
                              {-0.1304466774486913, 0.0349003117160589, 0.9908408714746880}})
        ->append("E2", E2);

    metrix->append(Translation{-583.0135525578830311, 0.0, 21532.1873249288291845})
        ->append(RotationBase{{0.9981347984218669, 0.0, 0.0610485395348569},
                              {0.0046731386016528, 0.9970659036601054, -0.0764051407568056},
                              {-0.0608694172384517, 0.0765479180543634, 0.9952061747630959}})
        ->append("PlaneMirror", PlaneMirror);

    metrix->append(Translation{-595.0801728525600538, 30.5293276352694036, 21729.4748228317694156})
        ->append(RotationBase{{-0.9981347984218669, 0.0, -0.0610485395348569},
                              {-0.0065310641596197, -0.9942610116376781, 0.1067819551149172},
                              {-0.0606981826769296, 0.1069814971722740, 0.9924065144296953}})
        ->append("SphericalGrating", SphericalGrating);

    metrix->append(Translation{-1266.6141077357613085, 30.5383201545151231, 32708.9576054686403950})
        ->append(RotationBase{{0.9981347984218669, 0.0, 0.0610485395348569},
                              {0.0000000499072879, 0.9999999999996659, -0.0000008159769440},
                              {-0.0610485395348365, 0.0000008175017496, 0.9981347984215335}})
        ->append("ExitSlit", ExitSlit);

    metrix->append(Translation{-1932.4784100420697541, 100.3019412687369396, 41427.5110444936217391})
        ->append(RotationBase{{0.9914448613738118, 0.0000000570260393, 0.1305261922200283},
                              {0.0091050966862335, 0.9975639933723643, -0.0691605114304122},
                              {-0.1302082334946503, 0.0697572872679162, 0.9890296946013423}})
        ->append("ImagePlane", ImagePlane);

    return metrix;
}
