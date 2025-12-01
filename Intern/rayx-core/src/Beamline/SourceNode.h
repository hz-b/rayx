#pragma once

#include <memory>

#include "BeamlineNode.h"
#include "Design/Source.h"

namespace rayx {

struct SourceNode : BeamlineNode {
    ~SourceNode() = default;
    inline bool isSource() const override { return true; }
    inline std::unique_ptr<BeamlineNode> clone() const override { return std::make_unique<SourceNode>(*this); }

    Source source;
};

}  // namespace rayx
