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


INLINE void setMap() {
    if(ElementStringMap.empty()) {
        ElementStringMap = {
            {ElementType::CircleSource, "Circle Source"},
            {ElementType::CylinderMirror, "Cylinder Mirror"},
            {ElementType::ImagePlane, "ImagePlane"},
            {ElementType::MatrixSource, "Matrix Source"},
            {ElementType::ParaboloidMirror, "Paraboloid"},
            {ElementType::PlaneGrating, "Plane Grating"},
            {ElementType::PointSource, "Point Source"},
            {ElementType::ReflectionZoneplante, "Reflection Zoneplante"},
            {ElementType::SimpleUndulatorSource, "Simple Undulator"},
            {ElementType::Slit, "Slit"},
            {ElementType::Sphere, "Sphere"},
            {ElementType::ConeMirror, "Cone"},
            {ElementType::ExpertsMirror, "Experts Optics"},
            {ElementType::PlaneMirror, "Plane Mirror"},
            {ElementType::SphereGrating, "Spherical Grating"},
            {ElementType::SphereMirror, "Sphere Mirror"},
            {ElementType::ToroidMirror, "Toroid"},
            {ElementType::DipoleSource, "Dipole Source"},
            {ElementType::DipoleSrc, "Dipole"},
            {ElementType::PixelSource, "Pixel Source"},
            {ElementType::EllipsoidMirror, "Ellipsoid"}
    };
}
}
ElementType findElementString(std::string name);
} //namespace
#endif