#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#include "Element.h"
#include "Rotation.h"
#include "Source.h"
#include "Translation.h"

namespace rayx::design {

struct EmptyNode;
struct SourceNode;
struct ElementNode;
struct TranslateNode;
struct RotateNode;
struct Beamline;

using NodePtr = std::variant<std::shared_ptr<EmptyNode>, std::shared_ptr<SourceNode>, std::shared_ptr<ElementNode>, std::shared_ptr<TranslateNode>, std::shared_ptr<RotateNode>>;

enum class SubBeamlineBehavior {
    Reference,
    CopyNodes,
    CopyNodesSourcesElements,
};

// TODO: consider polymorphic approach instead of variant + visit
struct NodeAdapter {
    NodePtr node;

    ////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////

    NodeAdapter getNode(std::string_view name) const { return std::visit([&](auto&& arg) { return arg.getNode(name); }, node); }
    NodeAdapter operator[](std::string_view name) const;

    SourcePtr getSource(std::string_view name) const;
    ElementPtr getElement(std::string_view name) const;
    std::shared_ptr<Rotation> getRotation(std::string_view name) const;
    std::shared_ptr<Translation> getTranslation(std::string_view name) const;

    ////////////////////////////////////////////////////////////
    // modifiers
    ////////////////////////////////////////////////////////////

    std::shared_ptr<EmptyNode> append(std::string_view name, Beamline beamline, SubBeamlineBehavior behavior = SubBeamlineBehavior::Reference);

    std::shared_ptr<SourceNode> append(std::string_view name, Source source);
    std::shared_ptr<SourceNode> append(std::string_view name, SourcePtr source);

    std::shared_ptr<ElementNode> append(std::string_view name, Element element);
    std::shared_ptr<ElementNode> append(std::string_view name, ElementPtr element);

    std::shared_ptr<TranslateNode> append(Translation translation);
    std::shared_ptr<TranslateNode> append(std::shared_ptr<Translation> translation);
    std::shared_ptr<TranslateNode> append(std::string_view name, Translation translation);
    std::shared_ptr<TranslateNode> append(std::string_view name, std::shared_ptr<Translation> translation);

    std::shared_ptr<RotateNode> append(Rotation rotation);
    std::shared_ptr<RotateNode> append(std::shared_ptr<Rotation> rotation);
    std::shared_ptr<RotateNode> append(std::string_view name, Rotation rotation);
    std::shared_ptr<RotateNode> append(std::string_view name, std::shared_ptr<Rotation> rotation);

    void remove(std::string_view name);
    void remove(NodePtr node);
};

struct NodeBase {
    std::string name;
    std::vector<NodePtr> children = {};

    ////////////////////////////////////////////////////////////
    // accessors
    ////////////////////////////////////////////////////////////

    const NodeAdapter getNode(std::string_view name);
    NodeAdapter getNode(std::string_view name) const;
    NodeAdapter operator[](std::string_view name) const;

    SourcePtr getSource(std::string_view name) const;
    ElementPtr getElement(std::string_view name) const;
    std::shared_ptr<Rotation> getRotation(std::string_view name) const;
    std::shared_ptr<Translation> getTranslation(std::string_view name) const;

    NodeAdapter copy() const;
    NodeAdapter copy(std::string_view name) const;

    ////////////////////////////////////////////////////////////
    // modifiers
    ////////////////////////////////////////////////////////////

    std::shared_ptr<EmptyNode> append(std::string_view name, Beamline beamline);

    std::shared_ptr<SourceNode> append(std::string_view name, Source source);
    std::shared_ptr<SourceNode> append(std::string_view name, SourcePtr source);

    std::shared_ptr<ElementNode> append(std::string_view name, Element element);
    std::shared_ptr<ElementNode> append(std::string_view name, ElementPtr element);

    std::shared_ptr<TranslateNode> append(Translation translation);
    std::shared_ptr<TranslateNode> append(std::string_view name, Translation translation);

    std::shared_ptr<RotateNode> append(Rotation rotation);
    std::shared_ptr<RotateNode> append(std::string_view name, Rotation rotation);

    void remove(std::string_view name);
};

struct EmptyNode : NodeBase {};

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
    // -> use a specific source (most versatile approach). to simplifiy the api, the beamline building process could assume/enforce a source as root
    // node and then use that unless specified explicitly
    //
    // there are two potentially useful implementations. i think both have their use cases, so maybe we want to implement both
    /// convert local beam coordinates to relative rotation -> append rotate node
    /// this way, future mutations that would affect the beam coords, DO NOT affect the relative rotation
    /// error detection can occur immediately
    // RotateNode& appendRotateToBeAlongBeamCoordsImmediate(std::string_view name = {});
    /// evaluate local beam coordinates just before tracing, and convert to relative rotation that is only used in the tracer
    /// this way, future mutations that would affect the beam coords, DO affect the relative rotation
    /// error detection can occur when trying to trace
    // RotateToBeamCoordsNode& appendRotateToBeamCoordsDeferred(std::string_view name = {);
};

struct ElementNode : NodeBase {
    ElementPtr element;
};

struct TranslateNode : NodeBase {
    std::shared_ptr<Translation> translation;
    bool inverse = false;
};

struct RotateNode : NodeBase {
    std::shared_ptr<Rotation> rotation;
    bool inverse = false;
};

struct Beamline : NodeBase {
    Beamline(std::string_view name) : NodeBase({.name = name}) {}

    glm::dvec3 getNodeAbsolutePosition(std::string_view name) const;
    RotationAroundAxis getNodeAbsoluteRotation(std::string_view name) const;
    std::vector<std::tuple<int, std::string>> getObjectIds() const;
    int getNodeSourceId(std::string_view name) const;
    int getNodeElementId(std::string_view name) const;
    int getNodeObjectId(std::string_view name) const;

    Beamline copy(std::string_view name) const;
};

}  // namespace rayx::design
