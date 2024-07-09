#ifndef STRINGS
#define STRINGS

#include <map>
#include <optional>

namespace RAYX {
enum class ElementType {
    ImagePlane,
    ConeMirror,
    CylinderMirror,
    EllipsoidMirror,
    ExpertsMirror,
    ParaboloidMirror,
    PlaneGrating,
    PlaneMirror,
    ReflectionZoneplate,
    Slit,
    SphereGrating,
    Sphere,
    SphereMirror,
    ToroidMirror,
    PointSource,
    MatrixSource,
    DipoleSource,
    DipoleSrc,
    PixelSource,
    CircleSource,
    SimpleUndulatorSource
};

extern std::map<ElementType, std::string> ElementStringMap;

ElementType findElementString(const std::string& name);
}  // namespace RAYX
#endif