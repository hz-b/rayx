#ifndef STRINGS
#define STRINGS

#include <map>
#include <optional>

namespace RAYX {
enum class ElementType{
        ImagePlane,
        ConeMirror,
        CylinderMirror,
        EllipsoidMirror,
        ExpertsMirror,
        ParaboloidMirror,
        PlaneGrating,
        PlaneMirror,
        ReflectionZoneplante,
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

static std::map<ElementType, std::string> ElementStringMap;



ElementType findElementString(std::string name);
} //namespace
#endif