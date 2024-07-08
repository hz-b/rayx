#include "Strings.h"

namespace RAYX {

std::map<ElementType, std::string> ElementStringMap = {{ElementType::CircleSource, "Circle Source"},
                                                       {ElementType::CylinderMirror, "Cylinder"},
                                                       {ElementType::ImagePlane, "ImagePlane"},
                                                       {ElementType::MatrixSource, "Matrix Source"},
                                                       {ElementType::ParaboloidMirror, "Paraboloid"},
                                                       {ElementType::PlaneGrating, "Plane Grating"},
                                                       {ElementType::PointSource, "Point Source"},
                                                       {ElementType::ReflectionZoneplate, "Reflection Zoneplate"},
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


ElementType findElementString(std::string name) {
    setMap();
    for (const auto& pair : ElementStringMap) {
        std::cout << pair.second << std::endl;
        if (pair.second == name) {
            return pair.first;
        }
    }
    RAYX_ERR << "Error reading the Element-type String";
    return ElementType::ImagePlane;
}
} //namespace