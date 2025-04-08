#ifndef STRINGS
#define STRINGS

#include <map>
#include <optional>
#include <string>

namespace RAYX {
enum class ElementType {
    ImagePlane,
    ConeMirror,
    CylinderMirror,
    EllipsoidMirror,
    ExpertsMirror,
    ParaboloidMirror,
    PlaneCrystal,
    PlaneGrating,
    PlaneMirror,
    ReflectionZoneplate,
    Slit,
    SphereGrating,
    Sphere,
    SphereMirror,
    ToroidMirror,
    ToroidGrating,
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
