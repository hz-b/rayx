#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#include "Source.h"
#include "SurfaceElement.h"
#include "Translation.h"
#include "Rotation.h"

namespace rayx::design {

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

}  // namespace rayx::design
