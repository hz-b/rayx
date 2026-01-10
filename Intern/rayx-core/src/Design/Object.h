#pragma once

#include <variant>
#include <memory>

#include "Source.h"
#include "SurfaceElement.h"

using ObjectPtr = std::variant<
    std::shared_ptr<PointSource>,
    std::shared_ptr<MatrixSource>,
    std::shared_ptr<CircleSource>,
    std::shared_ptr<SimpleUndulatorSource>,
    std::shared_ptr<PixelSource>,
    std::shared_ptr<DipoleSource>,
    std::shared_ptr<InputSource>,
    std::shared_ptr<SurfaceElement>,
>;
