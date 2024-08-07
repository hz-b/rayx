#pragma once

#include "Design/DesignSource.h"
#include "Shader/LightSources.h"

namespace RAYX {

template <typename T>
std::optional<T> tryCompileDesignSource(const DesignSource& designSource) {
    switch (designSource.getType()) {
        case ElementType::PointSource:
            if constexpr (std::is_same_v<T, PointSource>) return T(designSource);
            return std::nullopt;
        case ElementType::MatrixSource:
            if constexpr (std::is_same_v<T, MatrixSource>) return T(designSource);
            return std::nullopt;
        case ElementType::DipoleSource:
            if constexpr (std::is_same_v<T, DipoleSource>) return T(designSource);
            return std::nullopt;
        case ElementType::PixelSource:
            if constexpr (std::is_same_v<T, PixelSource>) return T(designSource);
            return std::nullopt;
        case ElementType::CircleSource:
            if constexpr (std::is_same_v<T, CircleSource>) return T(designSource);
            return std::nullopt;
        case ElementType::SimpleUndulatorSource:
            if constexpr (std::is_same_v<T, SimpleUndulatorSource>) return T(designSource);
            return std::nullopt;
        default:
            RAYX_WARN << "DesignSource about to compile does not appear to be a light source. Aborting.";
            return std::nullopt;
    }
}

LightSourceVariant compileDesignSource(const DesignSource& designSource) {
    switch (designSource.getType()) {
        case ElementType::PointSource:
            return PointSource(designSource);
        case ElementType::MatrixSource:
            return MatrixSource(designSource);
        case ElementType::DipoleSource:
            return DipoleSource(designSource);
        case ElementType::PixelSource:
            return PixelSource(designSource);
        case ElementType::CircleSource:
            return CircleSource(designSource);
        case ElementType::SimpleUndulatorSource:
            return SimpleUndulatorSource(designSource);
        default:
            RAYX_WARN << "DesignSource about to compile does not appear to be a light source. Aborting.";
            return std::monostate{};
    }
}

}  // namespace RAYX
