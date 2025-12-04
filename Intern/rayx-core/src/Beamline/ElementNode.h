#pragma once

#include <memory>

#include "BeamlineNode.h"
#include "Design/SurfaceElement.h"

namespace rayx {

struct ElementNode : BeamlineNode {
    ~ElementNode() = default;
    inline bool isElement() const override { return true; }
    inline std::unique_ptr<BeamlineNode> clone() const override { return std::make_unique<ElementNode>(*this); }

    Element element;
};

}  // namespace rayx
