#pragma once

namespace rayx::design {

struct BeamlineNode {
    std::optional<std::string> name;
    Transform transform;
    std::vector<std:variant<std::shared_ptr<BeamlineNode>, std::shared_ptr<Object>>> children;
};

struct Beamline {
    std::optional<std::string> name;
    BeamlineNode root;

    std::tuple<int, std::shared_ptr<BeamlineNode>> findNodeByName(const std::string& name) const;
    std::tuple<int, std::shared_ptr<Object>> findObjectByName(const std::string& name) const;
};

} // namespace rayx::design

/*
 * Declare Beamline tree all at once
 */

auto beamline = Node {
    .name = "beamline",
    .children = {
        SourceNode {
            .name = "point_source",
            .source = PointSource {
                .numRays = 100000,
                .rayAngle {
                    .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
                    .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
                },
                .rayEnergy = 300.0_eV,
                .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
            }
            .children = {
                ElementNode {
                    .name = "flat_mirror",
                    .transform = Transform {
                        .position = glm::dvec3 {0.0, 0.0, 1000.0},
                        .rotation = RotationAroundAxis {
                            .angle = 10.0_deg,
                            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
                        },
                    },
                    .element = SurfaceElement {
                        .area = RectangularArea { .width = 50.0, .height = 50.0 },
                        .behavior = ReflectBehavior { .substrate = materials::Si },
                    },
                    .children = {
                        ElementNode {
                            .name = "detector",
                            .transform = Transform {
                                .position = glm::dvec3 {0.0, 0.0, 2000.0},
                            },
                            .element = SurfaceElement {
                                .area = RectangularArea { .width = 100.0, .height = 100.0 },
                                .behavior = AbsorbBehavior {},
                            },
                        }
                    },
                },
            },
        },
    },
};

/*
 * Declare Beamline tree step by step. NOTE: this allows cyclic references! Not recommended.
 */

auto pointSource = PointSource {
    .name = "point_source",
    .numRays = 100000,
    .rayAngle {
        .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
        .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
    },
    .rayEnergy = 300.0_eV,
    .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
};

auto flatMirror = SurfaceElement {
    .name = "flat_mirror",
    .transform = RelativeTransform {
        .relativeTo = "point_source",
        .position   = glm::dvec3 {0.0, 0.0, 1000.0},
        .rotation   = RotationAroundAxis {
            .angle = 10.0_deg,
            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
        },
    },
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = ReflectBehavior { .substrate = materials::Si },
};

auto pgm = EmptyNode {
    .name = "pgm",
    .transform = AlongBeamTransform {
        .relativeTo         = "flat_mirror",
        .distance           = 1000.0,
        .angleAroundBeam    = 0.0_deg,
    },
};

auto pgmMirrorAxis = EmptyNode {
    .name = "pgm_mirror_axis",
    .transform = RotationAroundBeamTransform {
        .relativeTo      = "flat_mirror",
        .angleAroundBeam = 0.0_deg,
    },
};

auto pgmMirror = SurfaceElement {
    .name = "pgm_mirror",
    .transform = RelativeTransform {
        .relativeTo = "pgm_mirror_axis",
        .position   = glm::dvec3 {0.0, 0.0, 0.0},
        .rotation   = RotationAroundAxis {
            .angle = 5.0_deg,
            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
        },
    },
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = ReflectBehavior { .substrate = materials::Si },
};

auto pgm_grating = SurfaceElement {
    .name = "pgm_grating",
    .transform = RelativeTransform {
        .relativeTo = "pgm",
        .position   = glm::dvec3 {0.0, 0.0, 0.0},
        .rotation   = RotationAroundAxis {
            .angle = -5.0_deg,
            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
        },
    },
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = GratingBehavior {
        .lineDensity         = 1200.0,  // lines per mm
        .orderOfDiffraction  = 1,
    },
};

auto detector = SurfaceElement {
    .name = "detector",
    .transform = AlongBeamTransform {
        .relativeTo         = "pgm",
        .distance           = 1000.0,
        .angleAroundBeam    = 90.0_deg,
        // NOTE: order of diffraction is inherited from behavior of previous element
    },
    .area = RectangularArea { .width = 100.0, .height = 100.0 },
    .behavior = AbsorbBehavior {},
};

using BeamlineNode = std::variant<EmptyNode, SourceNode, ElementNode>;

auto beamline = Beamline {
    .name = "beamline",
    .nodes = {
        pointSource,
        flatMirror,
        pgm,
        pgmMirrorAxis,
        pgmMirror,
        pgm_grating,
        detector,
    },
};

/*
 * Use trees but also allow placment of trees along beam
 */

auto pointSource = PointSource {
    .numRays = 100000,
    .rayAngle {
        .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
        .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
    },
    .rayEnergy = 300.0_eV,
    .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
};

auto flatMirror = SurfaceElement {
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = ReflectBehavior { .substrate = materials::Si },
};

auto pgmMirror = SurfaceElement {
    .transform = RelativeTransform {
        .relativeTo = "pgm_mirror_axis",
        .position   = glm::dvec3 {0.0, 0.0, 0.0},
        .rotation   = RotationAroundAxis {
            .angle = 5.0_deg,
            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
        },
    },
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = ReflectBehavior { .substrate = materials::Si },
};

auto pgmGrating = SurfaceElement {
    .transform = RelativeTransform {
        .relativeTo = "pgm",
        .position   = glm::dvec3 {0.0, 0.0, 0.0},
        .rotation   = RotationAroundAxis {
            .angle = -5.0_deg,
            .axis  = glm::dvec3 {1.0, 0.0, 0.0},
        },
    },
    .area = RectangularArea { .width = 50.0, .height = 50.0 },
    .behavior = GratingBehavior {
        .lineDensity         = 1200.0,  // lines per mm
        .orderOfDiffraction  = 1,
    },
};

auto detector = SurfaceElement {
    .transform = AlongBeamTransform {
        .relativeTo         = "pgm",
        .distance           = 1000.0,
        .angleAroundBeam    = 90.0_deg,
        // NOTE: order of diffraction is inherited from behavior of previous element
    },
    .area = RectangularArea { .width = 100.0, .height = 100.0 },
    .behavior = AbsorbBehavior {},
};

auto pgm = Tree {
    .properties = {
        Property<Angle> {
            .name  = "pimpale_angle",
            .value = 10_deg,
        },
    },
    .nodes = {
        TreeNode {
            .transform = Transform {
                .position = glm::dvec3 {0.0, 0.0, 1000.0},
                .rotation = RotationAroundAxis {
                    .angle = PropertyReference<Angle> { .name = "pimpale_angle" },
                    .axis  = glm::dvec3 {1.0, 0.0, 0.0},
                },
            },
            .nodes = {
                TreeNode {
                    .transform = Transform {
                        // transform of the mirror within the pgm
                    },
                    .nodes = {
                        pgmMirror,
                    },
                },
            },
        },
        TreeNode {
            .transform = Transform {
                .position = glm::dvec3 {0.0, 10.0, 0.0},
                .rotation = RotationAroundAxis {
                    .angle = 180.0_deg,
                    .axis  = glm::dvec3 {1.0, 0.0, 0.0},
                },
            },
            .nodes = {
                pgmGrating,
            },
        },
    },
};

auto beamline = Beamline {
    .name = "beamline",
    .objects = {
        ObjectNode {
            .transform = AbsoluteTransform {
                .position = glm::dvec3 {0.0, 0.0, 0.0},
                .rotation = glm::dmat3(1.0),
            },
            .name = "point_source",
            .object = pointSource,
        },
        ObjectNode {
            .transform = AlongBeamTransform {
                .relativeTo         = "point_source",
                .source             = "point_source",
                .distance           = 1000.0,
                .angleAroundBeam    = 0.0_deg,
            },
            .name = "flat_mirror",
            .object = flatMirror,
        },
        ObjectNode {
            .transform = AlongBeamTransform {
                .relativeTo         = "flat_mirror",
                .source             = "point_source",
                .distance           = 1000.0,
                .angleAroundBeam    = 0.0_deg,
            },
            .name = "pgm",
            .object = pgm,
        },
        pgm,
        detector,
    },
};
