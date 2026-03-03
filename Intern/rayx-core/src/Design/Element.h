#pragma once

#include <optional>

#include "Aperture.h"
#include "Area.h"
#include "Behavior.h"
#include "Core.h"
#include "Curvature.h"
#include "SlopeError.h"

namespace rayx {

enum ElementType {
    SurfaceElement,
    PointSource,
    MatrixSource,
    CircleSource,
    SimpleUndulatorSource,
    PixelSource,
    DipoleSource,
    InputSource,
};


struct SurfaceElement {
    constexpr SurfaceElement(Area area, Behavior behavior) : m_area(area), m_behavior(behavior) {}

    RAYX_NESTED_PROPERTY(SurfaceElement, Area, area);
    RAYX_NESTED_PROPERTY(SurfaceElement, Behavior, behavior);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<Curvature>, curvature);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<Aperture>, aperture);
    RAYX_NESTED_PROPERTY(SurfaceElement, std::optional<SlopeError>, slopeError);
    RAYX_PROPERTY(SurfaceElement, BackFaceBehavior, backFaceBehavior) = BackFaceBehavior::Absorb;
};

}  // namespace rayx
