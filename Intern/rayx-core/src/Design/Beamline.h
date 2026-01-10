#pragma once

namespace rayx::design {

////////////////////////////////////////////////////////////
// nodes
////////////////////////////////////////////////////////////

struct AbsoluteNode;
struct RelativeNode;
struct AlongBeamNode;
struct ObjectNode;

using PreceedingNodePtr = std::variant<
    std::shared_ptr<AbsoluteNode>,
    std::shared_ptr<RelativeNode>,
    std::shared_ptr<AlongBeamNode>
>;

using NodePtr = std::variant<
    std::shared_ptr<AbsoluteNode>,
    std::shared_ptr<RelativeNode>,
    std::shared_ptr<AlongBeamNode>,
    std::shared_ptr<ObjectNode>
>;

struct AbsoluteNode {
    std::optional<std::string> name;
    std::optional<Transform> transform;
};

struct RelativeNode {
    std::optional<std::string> name;
    std::optional<Transform> transform;

    // relative position
    preceedingNodePtr preceedingNode;
};

struct AlongBeamNode {
    std::optional<std::string> name;
    std::optional<Transform> transform;

    // relative position
    SourcePtr beamSource;
    PreceedingNodePtr preceedingNode;
    double distanceToPreceedingNode = 0.0;
};

struct ObjectNode {
    std::string name;
    ObjectPtr object;
    PreceedingNodePtr preceedingNode;
};

std::optional<std::string> getName(const Node& node) {
    std::visit([] (const auto& alt) { return alt.name; }, node);
}
std::optional<Transform> getTransform(const Node& node) {
    std::visit([] (const auto& alt) { return alt.transform; }, node);
}
std::shared_ptr<Object> getObject(const Node& node) {
    std::visit([] (const auto& alt) { return alt.object; }, node);
}

////////////////////////////////////////////////////////////
// scene
////////////////////////////////////////////////////////////

class Scene {
public:
    Scene() = default;
    Scene(const Scene&) = default;
    Scene(Scene&&) = default;
    Scene& operator=(const Scene&) = default;
    Scene& operator=(Scene&&) = default;

    // general accessors
    bool hasCircularDependencies() const;
    bool hasObjectNames() const;
    bool isValid() const;

    // node accessors
    NodePtr getNode(std::string_view nodeName) const;
    Transform getNodeAbsoluteTransform(const std::string_view nodeName) const;
    Transform getNodeAbsoluteTransform(const NodePtr& node) const;

    // object node accessors
    std::shared_ptr<ObjectNode> getObjectNode(std::string_view objectNodeName) const;
    int getObjectNodeId(const std::string_view objectNodeName) const;
    std::map<std::string, int> getObjectNodeIds() const;

    // object accessors
    ObjectPtr getObject(std::string_view objectName) const;

    // node modifiers
    void addNode(NodePtr node); // add node and all preceeding nodes if not already present
    void removeNode(std::string_view nodeName); // remove node and all its descendant nodes
    void removeNode(NodePtr node);

    // copy
    Scene copyNodes() const;
    Scene copyNodesAndObjects() const;
    Scene copyNodesAndObjectsAndResources() const;

protected:
    std::vector<NodePtr> m_nodes;
    // std::vector<std::shared_ptr<ObjectNode>> m_objects;
};

////////////////////////////////////////////////////////////
// beamline
////////////////////////////////////////////////////////////

class BeamlineBuilder {
public:
    BeamlineBuilder(
        std::string_view beamSourceName,
        SourcePtr beamSource, // not null
        std::optional<Transform> transform = std::nullopt
    ) : m_scene(std::make_shared<Scene>()), m_beamSource(beamSource) {
        if (variantPtrIsNull(beamSource))
            throw std::runtime_exception("beamSource cannot be null!");

        auto root = std::make_shared<AbsoluteNode>(AbsoluteNode{
            .name = "root",
            .transform = transform,
        });

        auto source = std::make_shared<ObjectNode>(ObjectNode{
            .name = name,
            .object = beamSource,
        });

        m_scene.addNode(root);
        m_scene.addNode(source);

        m_head = root;
    }

    BeamlineBuilder(const BeamlineBuilder&) = default;
    BeamlineBuilder(BeamlineBuilder&&) = default;
    BeamlineBuilder& operator(const BeamlineBuilder&) = default;
    BeamlineBuilder& operator(BeamlineBuilder&&) = default;

    struct Func {
        std::optional<std::string> name;
        std::function<Transform(Transform)> func;
        std::optional<BeamlineBuilder> inputTransform;
    };

    struct AbsolutePosition {
        std::optional<std::string> name;
        Transform transform;
    };

    struct AbsoluteRotation {
        std::optional<std::string> name;
        Transform transform;
    };

    struct Translate {
        std::optional<std::string> name;
        Translation translation;
    };

    struct Rotate {
        std::optional<std::string> name;
        Rotation rotation;
    };

    struct AlongBeam {
        std::optional<std::string> name;
        double distanceToPreceeding = 1;
    };

    BeamlineBuilder next(const Absolute conf) {
        auto node = std::make_shared<AbsoluteNode>(AbsoluteNode{
            .name = conf.name,
            .transform = conf.transform,
        });
        m_head = m_nodes.back();
        m_scene->addNode(node);
        return *this;
    }

    BeamlineBuilder next(const Relative conf) {
        auto node = std::make_shared<RelativeNode>(RelativeNode{
            .name = conf.name,
            .transform = conf.transform,
            .preceeding = m_head,
        });
        m_head = m_nodes.back();
        m_scene-addNode(node);
        return *this;
    }

    BeamlineBuilder next(const AlongBeam conf) {
        auto node = std::make_shared<AlongBeamNode>(AlongBeamNode{
            .name = conf.name,
            .transform = conf.transform,
            .preceeding = m_head,
            .distanceToPreceeding = conf.distanceToPreceeding,
            .beamSource = m_beamSource,
        });
        m_head = node;
        m_scene->addNode(node);
        return *this;
    }

    BeamlineBuilder attachObject(std::string name, ObjectPtr object) {
        if (variantPtrIsNull(object)) throw std::runtime_exception("object cannot be null!");

        auto node = std::make_shared<ObjectNode>(ObjectNode{
            .name = name,
            .object = object,
            .parent = m_head,
        });
        m_scene.addNode(node);
        return *this;
    }

    Scene createScene() const {
        return *m_scene;
    }

private:
    std::shared_ptr<Scene> m_scene;
    SourcePtr m_beamSource;
    NodePtr m_head;
};

auto beamline = BeamlineBuilder(
    "mainSource", std::make_shared<PointSource>{
        .numRays = 1<<20,
    }
)
    .next(BeamlineBuilder::Relative{
        .transform = {
            .translation = glm::dvec3(0, 0, 100),
        },
    })
    // TODO: a Relative node cannot be the ancestor of an AlongBeam node
    .next(BeamlineBuilder::AlongBeam{
        .name = "pgm"
    })

;


auto beamline = BeamlineBuilder({
    .name = "beamline",
})
    .nextAbsolute({
        .name = "point_source",
        .source = PointSource {
            .numRays = 100000,
            .rayAngle {
                .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
                .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
            },
            .rayEnergy = 300.0_eV,
            .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
        },
    })
    .nextRelative({
        .name = "flat_mirror",
        .transform = Transform {
            .position = glm::dvec3 {0.0, 0.0, 1000.0},
            .rotation = RotationAroundAxis {
                .angle = 10.0_deg,
                .axis  = glm::dvec3 {1.0, 0.0, 0.0},
            },
        },
        .object = SurfaceElement {
            .area = RectangularArea { .width = 50.0, .height = 50.0 },
            .behavior = ReflectBehavior { .substrate = materials::Si },
        },
    })
    .nextAlongBeam({
        .name = "detector",
        .transform = Transform {
            .position = glm::dvec3 {0.0, 0.0, 2000.0},
        },
        .object = SurfaceElement {
            .area = RectangularArea { .width = 100.0, .height = 100.0 },
            .behavior = AbsorbBehavior {},
        },
        .source = std::dynamic_pointer_cast<Source>(beamline->getObjectByName("point_source")),
        .distance = 1000.0,
    })
    .beamline;

} // namespace rayx::design

/*
 * beamline mockup
 */

// auto beamline = Node {
//     .name = "beamline",
//     .children = {
//         SourceNode {
//             .name = "point_source",
//             .source = PointSource {
//                 .numRays = 100000,
//                 .rayAngle {
//                     .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//                     .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//                 },
//                 .rayEnergy = 300.0_eV,
//                 .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
//             }
//             .children = {
//                 ElementNode {
//                     .name = "flat_mirror",
//                     .transform = Transform {
//                         .position = glm::dvec3 {0.0, 0.0, 1000.0},
//                         .rotation = RotationAroundAxis {
//                             .angle = 10.0_deg,
//                             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//                         },
//                     },
//                     .element = SurfaceElement {
//                         .area = RectangularArea { .width = 50.0, .height = 50.0 },
//                         .behavior = ReflectBehavior { .substrate = materials::Si },
//                     },
//                     .children = {
//                         ElementNode {
//                             .name = "detector",
//                             .transform = Transform {
//                                 .position = glm::dvec3 {0.0, 0.0, 2000.0},
//                             },
//                             .element = SurfaceElement {
//                                 .area = RectangularArea { .width = 100.0, .height = 100.0 },
//                                 .behavior = AbsorbBehavior {},
//                             },
//                         }
//                     },
//                 },
//             },
//         },
//     },
// };
//
// /*
//  * Declare Beamline tree step by step. NOTE: this allows cyclic references! Not recommended.
//  */
//
// auto pointSource = PointSource {
//     .name = "point_source",
//     .numRays = 100000,
//     .rayAngle {
//         .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//         .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//     },
//     .rayEnergy = 300.0_eV,
//     .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
// };
//
// auto flatMirror = SurfaceElement {
//     .name = "flat_mirror",
//     .transform = RelativeTransform {
//         .relativeTo = "point_source",
//         .position   = glm::dvec3 {0.0, 0.0, 1000.0},
//         .rotation   = RotationAroundAxis {
//             .angle = 10.0_deg,
//             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//         },
//     },
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = ReflectBehavior { .substrate = materials::Si },
// };
//
// auto pgm = EmptyNode {
//     .name = "pgm",
//     .transform = AlongBeamTransform {
//         .relativeTo         = "flat_mirror",
//         .distance           = 1000.0,
//         .angleAroundBeam    = 0.0_deg,
//     },
// };
//
// auto pgmMirrorAxis = EmptyNode {
//     .name = "pgm_mirror_axis",
//     .transform = RotationAroundBeamTransform {
//         .relativeTo      = "flat_mirror",
//         .angleAroundBeam = 0.0_deg,
//     },
// };
//
// auto pgmMirror = SurfaceElement {
//     .name = "pgm_mirror",
//     .transform = RelativeTransform {
//         .relativeTo = "pgm_mirror_axis",
//         .position   = glm::dvec3 {0.0, 0.0, 0.0},
//         .rotation   = RotationAroundAxis {
//             .angle = 5.0_deg,
//             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//         },
//     },
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = ReflectBehavior { .substrate = materials::Si },
// };
//
// auto pgm_grating = SurfaceElement {
//     .name = "pgm_grating",
//     .transform = RelativeTransform {
//         .relativeTo = "pgm",
//         .position   = glm::dvec3 {0.0, 0.0, 0.0},
//         .rotation   = RotationAroundAxis {
//             .angle = -5.0_deg,
//             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//         },
//     },
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = GratingBehavior {
//         .lineDensity         = 1200.0,  // lines per mm
//         .orderOfDiffraction  = 1,
//     },
// };
//
// auto detector = SurfaceElement {
//     .name = "detector",
//     .transform = AlongBeamTransform {
//         .relativeTo         = "pgm",
//         .distance           = 1000.0,
//         .angleAroundBeam    = 90.0_deg,
//         // NOTE: order of diffraction is inherited from behavior of previous element
//     },
//     .area = RectangularArea { .width = 100.0, .height = 100.0 },
//     .behavior = AbsorbBehavior {},
// };
//
// using BeamlineNode = std::variant<EmptyNode, SourceNode, ElementNode>;
//
// auto beamline = Beamline {
//     .name = "beamline",
//     .nodes = {
//         pointSource,
//         flatMirror,
//         pgm,
//         pgmMirrorAxis,
//         pgmMirror,
//         pgm_grating,
//         detector,
//     },
// };
//
// /*
//  * Use trees but also allow placment of trees along beam
//  */
//
// auto pointSource = PointSource {
//     .numRays = 100000,
//     .rayAngle {
//         .x = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//         .y = WhiteNoiseDistribution<Angle> { .center = 0.0_deg, .range = 0.1_deg },
//     },
//     .rayEnergy = 300.0_eV,
//     .rayPolarization = Stokes {1.0, 1.0, 0.0, 0.0},
// };
//
// auto flatMirror = SurfaceElement {
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = ReflectBehavior { .substrate = materials::Si },
// };
//
// auto pgmMirror = SurfaceElement {
//     .transform = RelativeTransform {
//         .relativeTo = "pgm_mirror_axis",
//         .position   = glm::dvec3 {0.0, 0.0, 0.0},
//         .rotation   = RotationAroundAxis {
//             .angle = 5.0_deg,
//             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//         },
//     },
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = ReflectBehavior { .substrate = materials::Si },
// };
//
// auto pgmGrating = SurfaceElement {
//     .transform = RelativeTransform {
//         .relativeTo = "pgm",
//         .position   = glm::dvec3 {0.0, 0.0, 0.0},
//         .rotation   = RotationAroundAxis {
//             .angle = -5.0_deg,
//             .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//         },
//     },
//     .area = RectangularArea { .width = 50.0, .height = 50.0 },
//     .behavior = GratingBehavior {
//         .lineDensity         = 1200.0,  // lines per mm
//         .orderOfDiffraction  = 1,
//     },
// };
//
// auto detector = SurfaceElement {
//     .transform = AlongBeamTransform {
//         .relativeTo         = "pgm",
//         .distance           = 1000.0,
//         .angleAroundBeam    = 90.0_deg,
//         // NOTE: order of diffraction is inherited from behavior of previous element
//     },
//     .area = RectangularArea { .width = 100.0, .height = 100.0 },
//     .behavior = AbsorbBehavior {},
// };
//
// auto pgm = Tree {
//     .properties = {
//         Property<Angle> {
//             .name  = "pimpale_angle",
//             .value = 10_deg,
//         },
//     },
//     .nodes = {
//         TreeNode {
//             .transform = Transform {
//                 .position = glm::dvec3 {0.0, 0.0, 1000.0},
//                 .rotation = RotationAroundAxis {
//                     .angle = PropertyReference<Angle> { .name = "pimpale_angle" },
//                     .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//                 },
//             },
//             .nodes = {
//                 TreeNode {
//                     .transform = Transform {
//                         // transform of the mirror within the pgm
//                     },
//                     .nodes = {
//                         pgmMirror,
//                     },
//                 },
//             },
//         },
//         TreeNode {
//             .transform = Transform {
//                 .position = glm::dvec3 {0.0, 10.0, 0.0},
//                 .rotation = RotationAroundAxis {
//                     .angle = 180.0_deg,
//                     .axis  = glm::dvec3 {1.0, 0.0, 0.0},
//                 },
//             },
//             .nodes = {
//                 pgmGrating,
//             },
//         },
//     },
// };
//
// auto beamline = Beamline {
//     .name = "beamline",
//     .objects = {
//         ObjectNode {
//             .transform = AbsoluteTransform {
//                 .position = glm::dvec3 {0.0, 0.0, 0.0},
//                 .rotation = glm::dmat3(1.0),
//             },
//             .name = "point_source",
//             .object = pointSource,
//         },
//         ObjectNode {
//             .transform = AlongBeamTransform {
//                 .relativeTo         = "point_source",
//                 .source             = "point_source",
//                 .distance           = 1000.0,
//                 .angleAroundBeam    = 0.0_deg,
//             },
//             .name = "flat_mirror",
//             .object = flatMirror,
//         },
//         ObjectNode {
//             .transform = AlongBeamTransform {
//                 .relativeTo         = "flat_mirror",
//                 .source             = "point_source",
//                 .distance           = 1000.0,
//                 .angleAroundBeam    = 0.0_deg,
//             },
//             .name = "pgm",
//             .object = pgm,
//         },
//         pgm,
//         detector,
//     },
// };
