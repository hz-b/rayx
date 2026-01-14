#include "rayx.h"

using namespace rayx;
using namespace rayx::literals;

auto metrix = std::make_shared<Beamline>("METRIX");

metrix
    ->append("U41_318eV", PointSource {
        .numRays = 1<<20,
        .rayOrigin = {
            .x = GaussianDistribution<double> { .standartDeviation = 0.076 },
            .y = GaussianDistribution<double> { .standartDeviation = 0.027 },
            .z = WhiteNoiseDistribution<double> { .range = 1.0 },
        },
        .rayAngle = {
            .x = GaussianDistribution<Angle> { .standardDeviation = 0.078_rad }, // TODO: check
            .y = GaussianDistribution<Angle> { .standardDeviation = 0.04_rad },
        },
        .rayPhotonEnergy = 318_eV,
        .rayPolarization = Stokes{1, 1, 0, 0},
    })
    ->append(Translation {
        .z = 10000,
    })
    ->append("ASBL", SurfaceElement {
        .area = RectangularArea {
            .width = 100,
            .height = 100,
        },
        .behavior = AbsorbBehavior {},
        .aperture = DiffractiveAperture {
            .area = RectangularArea {
                .width = 2,
                .height = 2,
            },
        },
    })
    ->append(RotationBase {
        .x = {0, 1, 0},
        .y = {-0.9995335908367129, 0, -0.0305385132098227},
        .z = {-0.0305385132098227, 0, 0.9995335908367129},
    }),
    ->append(Translation {
        .z = 2000,
    })
    ->append("M1Cylinder", SurfaceElement {
        .area = RectangularArea {
            .width = 20,
            .height = 320,
        },
        .curvature = CylindricalCurvature {
            .cylinderDirection = CylinderDirection::ShortRadiusR,
            .radius = 174.21,
            .grazingIncAngle = 1.75_rad, // TODO: why would we need the grazing inc angle for a curvature?
            .entranceArmLength = 12000, // TODO: why is this the exact distance to the source?
            .exitArmLength = 3750,
        },
        .behavior = ReflectBehavior {
            .substrace = Material::Au,
        },
        .slopeError = SlopeError {
            .saggital = 1.01,
            .meridional = 0.16,
        }
    })
    // TODO: append transform
    ->append("EntranceSlit", SurfaceElement {
        .area = RectangularArea {
            .width = 100,
            .height = 100,
        },
        .behavior = AbsorbBehavior {},
        .aperture = DiffractiveAperture {
            .area = RectangularArea {
                .width = 10,
                .height = 10,
            },
        },
    })
    // TODO: append transform
    ->append("E1", SurfaceElement {
        .area = RectangularArea {
            .width = 20,
            .height = 390,
        },
        .behavior = ReflectBehavior {
            .substrace = Material::Au,
        },
        .curvature = EllipticalCurvature {
            .shortHalfAxisB = 302.721702601,
            .longHalfAxisA = 20750,
            .entranceArmLength = 39600,
            .exitArmLength = 1900,
            .designGrazingIncAngle = 2.00735,
            .figureRotation = FigureRotation::Plane,
            .parameterAA = 1,
        },
        .slopeError = SlopeError {
            .saggital = 0.037,
            .meridional = 0.023,
        },
    })
    // TODO: append transform
    ->append("E2", SurfaceElement {
        .area = RectangularArea {
            .width = 20,
            .height = 390,
        },
        .behavior = ReflectBehavior {
            .substrace = Material::Au,
        },
        .curvature = EllipticalCurvature {
            .shortHalfAxisB = 97.1560870104,
            .longHalfAxisA = 4375,
            .entranceArmLength = 7750,
            .exitArmLength = 1000,
            .designGrazingIncAngle = 2.00735,
            .figureRotation = FigureRotation::Plane,
            .parameterAA = 1,
        },
        .slopeError = SlopeError {
            .saggital = 0.045,
            .meridional = 0.024,
        },
    })
    // TODO: append transform
    ->append("PlaneMirror", SurfaceElement {
        .area = RectangularArea {
            .width = 20,
            .height = 310,
        },
        .behavior = RefectiveBehavior {
            .substrace = Material::Au,
        },
        .slopeError = SlopeError {
            saggital = 0.084,
            meridional = 0.023,
        },
    })
    // TODO: append transform
    ->append("SphericalGrating", SurfaceElement {
        .area = RectangularArea {
            .width = 20,
            .height = 100,
        },
        .behavior = GratingBehavior {
            .lineDensity = 1199.918,
            .orderOfDiffraction = 1,
        },
        .slopeError = SlopeError {
            saggital = 0.124,
            meridional = 0.102,
        },
    })
    // TODO: append transform
    ->append("ExitSlit", SurfaceElement {
        .area = RectangularArea {
            .width = 100,
            .height = 100,
        },
        .behavior = AbsorbBehavior {},
        .aperture = DiffractiveAperture {
            .area = RectangularArea {
                .widht = 20,
                .height = 0.1,
            },
        },
    })
    // TODO: append transform
    ->append("ImagePlane", SurfaceElement{
        .area = UnlimitedArea {},
        .behavior = DetectorBehavior {},
    });
