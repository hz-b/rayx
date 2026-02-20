#pragma once

#include <memory>
#include <variant>

#include "Element.h"
#include "Source.h"

namespace rayx {

using Source = std::variant<PointSource, MatrixSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource>;

using Element = std::variant<SurfaceElement>;

using Object = std::variant<PointSource, MatrixSource, CircleSource, SimpleUndulatorSource, PixelSource, DipoleSource, InputSource, SurfaceElement>;

using ObjectPtr =
    std::variant<std::shared_ptr<PointSource>, std::shared_ptr<MatrixSource>, std::shared_ptr<CircleSource>, std::shared_ptr<SimpleUndulatorSource>,
                 std::shared_ptr<PixelSource>, std::shared_ptr<DipoleSource>, std::shared_ptr<InputSource>, std::shared_ptr<SurfaceElement> >;

namespace detail {
// TODO: this is odd. it needs to be updated everytime a new element is added
constexpr bool isElement(const ObjectPtr& object) {
    return std::visit([]<typename T>(const std::shared_ptr<T>&) { return std::is_same_v<T, SurfaceElement>; }, object);
}
}  // namespace detail

}  // namespace rayx
