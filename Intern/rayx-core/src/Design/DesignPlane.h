#pragma once

namespace rayx {

enum class RAYX_API DesignPlane {
    XY,
    XZ,
};

namespace detail {

/**
 * @brief Metafunction to determine the design plane of a given type.
 * This is specialized for different source and behavior types.
 * Unspecialized types will result in a compilation error.
 */
template <typename T>
struct DesignPlaneOf;

// clang-format off
template <> struct DesignPlaneOf<ArtificialSource>      { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<ProjectorSource>       { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<SimpleUndulatorSource> { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<PixelSource>           { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<DipoleSource>          { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<InputSource>           { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<DetectorBehavior>      { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<AbsorbBehaviour>       { static constexpr DesignPlane value = DesignPlane::XY; };
template <> struct DesignPlaneOf<TransmitBehaviour>     { static constexpr DesignPlane value = DesignPlane::XY; };

template <> struct DesignPlaneOf<ReflectBehaviour>      { static constexpr DesignPlane value = DesignPlane::XZ; };
template <> struct DesignPlaneOf<RzpBehavior>           { static constexpr DesignPlane value = DesignPlane::XZ; };
template <> struct DesignPlaneOf<GratingBehavior>       { static constexpr DesignPlane value = DesignPlane::XZ; };
template <> struct DesignPlaneOf<CrystalBehavior>       { static constexpr DesignPlane value = DesignPlane::XZ; };
// clang-format on

template <typename T>
constexpr DesignPlane designPlane_v = DesignPlaneOf<T>::value;

}  // namespace detail

RAYX_API DesignPlane getDesignPlane(const Source& source) {
    return std::visit([]<typename T>(const T&) { return detail::designPlane_v<T>; }, source);
}

RAYX_API DesignPlane getDesignPlane(const SurfaceElement& element) {
    return std::visit([]<typename T>(const T&) { return detail::designPlane_v<T>; }, element.behavior);
}

}  // namespace rayx
