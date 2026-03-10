#pragma once

#include "Design/Beamline.h"
#include "Element.h"
#include "Model/Beamline.h"
#include "Source.h"
#include "ToModelTrait.h"

namespace rayx::detail {

inline model::BeamlineSource toModel(const ObjectInfo& objectInfo, model::Source source) {
    const auto transform = objectInfo.node->absoluteTransform();
    return model::BeamlineSource{
        .name     = objectInfo.node->name(),
        .objectId = objectInfo.objectId,
        .inMat    = glm::inverse(transform),
        .outMat   = transform,
        .source   = std::move(source),
    };
}

inline model::BeamlineElement toModel(const ObjectInfo& objectInfo, model::SurfaceElement element) {
    const auto transform = objectInfo.node->absoluteTransform();
    return model::BeamlineElement{
        .name     = objectInfo.node->name(),
        .objectId = objectInfo.objectId,
        .inMat    = glm::inverse(transform),
        .outMat   = transform,
        .element  = std::move(element),
    };
}

namespace {
inline void append(model::Beamline& beamline, model::BeamlineSource source) { beamline.sources.emplace_back(std::move(source)); }
inline void append(model::Beamline& beamline, model::BeamlineElement element) { beamline.elements.emplace_back(std::move(element)); }
}  // unnamed namespace

template <>
struct ToModel<Beamline> {
    static model::Beamline apply(const Beamline& beamline) {
        auto result = model::Beamline();
        result.name = beamline.name();

        for (const auto& objectInfo : beamline.allObjects()) {
            const auto& designObject = objectInfo.node->object();

            std::visit([&]<typename T>(const std::shared_ptr<T>& obj) { append(result, toModel(objectInfo, toModel(*obj))); }, designObject);
        }

        return result;
    }
};

}  // namespace rayx::detail
