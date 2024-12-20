#pragma once

#include <functional>
#include <memory>
#include <variant>
#include <vector>

namespace RAYX {

class Group;
class DesignElement;
class DesignSource;

using BeamlineNode = std::variant<DesignElement, DesignSource, Group>;

enum class NodeType { OpticalElement, LightSource, Group };

// Utility function to determine node type
inline NodeType getNodeType(const BeamlineNode& node) {
    return std::visit(
        [](auto&& element) -> NodeType {
            using T = std::decay_t<decltype(element)>;
            if constexpr (std::is_same_v<T, DesignElement>) {
                return NodeType::OpticalElement;
            } else if constexpr (std::is_same_v<T, DesignSource>) {
                return NodeType::LightSource;
            } else if constexpr (std::is_same_v<T, Group>) {
                return NodeType::Group;
            }
        },
        node);
}

class Group {
  public:
    NodeType getNodeType() const;
    const BeamlineNode& getNode(size_t index) const;

    void traverse(const std::function<void(const BeamlineNode&)>& callback) const;

    void addChild(BeamlineNode&& child);
    void addChild(const BeamlineNode& child);

    // New methods for retrieving elements, sources, and groups
    std::vector<DesignElement> getAllElements() const;
    std::vector<DesignSource> getAllSources() const;
    std::vector<Group> getAllGroups() const;

  private:
    std::vector<BeamlineNode> children;  // Children of the node
};
}  // namespace RAYX
