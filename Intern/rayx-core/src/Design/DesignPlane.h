#pragma once

namespace rayx {

enum class RAYX_API DesignPlane {
    XY,
    XZ,
    // YZ,
};

RAYX_API inline DesignPlane getDesignPlane(Source source) {
    return DesignPlane::XY;
}

RAYX_API inline DesignPlane getDesignPlane(Element element) {
    if (
            std::holds_alternative<DetectorBehavior>(element.behavior) ||
            std::holds_alternative<AbsorbBehavior>(element.behavior) ||
            std::holds_alternative<TransmitBehavior>(element.behavior)

            )
        return DesignPlane::XY;
    return DesignPlane::XZ;
}

}
