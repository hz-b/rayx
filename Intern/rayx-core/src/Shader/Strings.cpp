#include "Strings.h"

namespace RAYX {

std::map<ElementType, std::string> ElementStringMap = {{ElementType::CircleSource, "Circle Source"},
                                                       {ElementType::CylinderMirror, "Cylinder"},
                                                       {ElementType::ImagePlane, "ImagePlane"},
                                                       {ElementType::MatrixSource, "Matrix Source"},
                                                       {ElementType::ParaboloidMirror, "Paraboloid"},
                                                       {ElementType::PlaneGrating, "Plane Grating"},
                                                       {ElementType::PointSource, "Point Source"},
                                                       {ElementType::ReflectionZoneplante, "Reflection Zoneplate"},
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
                                                       {ElementType::EllipsoidMirror, "Ellipsoid"}};

ElementType findElementString(const std::string& name) {
    auto it = std::find_if(ElementStringMap.begin(), ElementStringMap.end(),
                           [&name](const std::pair<ElementType, std::string>& pair) { return pair.second == name; });
    if (it != ElementStringMap.end()) {
        return it->first;
    } else {
        RAYX_ERR << "Error in findElementString(const std::string&): Element not found";
        return ElementType::ImagePlane;  // or some other default/fallback value
    }
}
}  // namespace RAYX