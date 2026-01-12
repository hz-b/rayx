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
        return m_scene->copyNodesAndObjects();
    }

private:
    std::shared_ptr<Scene> m_scene;
    SourcePtr m_beamSource;
    PreceedingNodePtr m_head;
};


struct SourceNode;
struct ElementNode;
struct TranslateNode;
struct RotateNode;

using NodePtr = std::variant<
    std::shared_ptr<SourceNode>,
    std::shared_ptr<ElementNode>,
    std::shared_ptr<TranslateNode>,
    std::shared_ptr<RotateNode>
>;

struct NodeBase {
    std::string name;
    std::vector<NodePtr> children;

    std::shared_ptr<SourceNode> append(std::shared_ptr<SourceNode> node);
    std::shared_ptr<ElementNode> append(std::shared_ptr<ElementNode> node);
    std::shared_ptr<TranslateNode> append(std::shared_ptr<TranslateNode> node);
    std::shared_ptr<RotateNode> append(std::shared_ptr<RotateNode> node);
    NodePtr append(NodePtr node);

    std::shared_ptr<SourceNode> append(std::string_view name, Source source);
    std::shared_ptr<SourceNode> append(std::string_view name, SourcePtr source);

    std::shared_ptr<ElementNode> append(std::string_view name, Element element);
    std::shared_ptr<ElementNode> append(std::string_view name, ElementPtr element);

    std::shared_ptr<TranslateNode> append(Translation translation);
    std::shared_ptr<TranslateNode> append(std::string_view name, Translation translation);

    std::shared_ptr<RotateNode> append(Rotation rotation);
    std::shared_ptr<RotateNode> append(std::string_view name, Rotation rotation);

    NodePtr getNode(std::string_view name) const;
    std::shared_ptr<SourceNode> getNodeSource(std::string_view name) const;
    std::shared_ptr<ElementNode> getNodeElement(std::string_view name) const;
    std::shared_ptr<TranslateNode> getNodeTranslate(std::string_view name) const;
    std::shared_ptr<RotateNode> getNodeRotate(std::string_view name) const;

    void removeNode(NodePtr node);
};

struct SourceNode : NodeBase {
    SourcePtr source;

    // TODO: distance to preceeding source or element based on the beam coords.
    // NOTE: beam coords = direction and rotation around direction
    //
    // this can be done by tracing a single ray and figuring the beam coords after the last source / element.
    // effectively the successive node will be in beam coords.
    // -> we have to respect the rotation around the beam, that can change on every hit or when the source is rotateed
    // does this only make sense if the last node was a source or element?
    // -> a translation or rotation could make sense, when applyed to the beam coords. might be confusing though
    //
    // doing this requires a source that emits the ray from its coordinates. options to pick the source
    // -> when going up the tree, use first encountered source
    // -> when going up the tree, use last encountered source
    // -> use a specific source (most versatile approach). to simplifiy the api, the beamline building process could assume/enforce a source as root node and then use that unless specified explicitly
    //
    // there are two potentially useful implementations. i think both have their use cases, so maybe we want to implement both
    /// convert local beam coordinates to relative rotation -> append rotate node
    /// this way, future mutations that would affect the beam coords, DO NOT affect the relative rotation
    /// error detection can occur immediately
    // RotateNode& appendRotateToBeamCoordsImmediate(std::string_view name = {});
    /// evaluate local beam coordinates just before tracing, and convert to relative rotation that is only used in the tracer
    /// this way, future mutations that would affect the beam coords, DO affect the relative rotation
    /// error detection can occur when trying to trace
    // RotateToBeamCoordsNode& appendRotateToBeamCoordsDeferred(std::string_view name = {);
};

struct ElementNode : NodeBase {
    ElementPtr element;
};

struct TranslateNode : NodeBase {
    Translation translation;
};

struct RotateNode : NodeBase {
    Rotation rotation;
};

struct Beamline : NodeBase {
    Beamline() : NodeBase({.name = "root"}) {}

    glm::dvec3 getNodeAbsolutePosition(std::string_view name) const;
    RotationAroundAxis getNodeAbsoluteRotation(std::string_view name) const;
    std::vector<std::tuple<int, std::string>> getObjectIds() const;
    int getNodeSourceId(std::string_view name) const;
    int getNodeElementId(std::string_view name) const;
    int getNodeObjectId(std::string_view name) const;
};
