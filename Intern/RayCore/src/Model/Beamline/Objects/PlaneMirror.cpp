#include "PlaneMirror.h"
#include <Data/xml.h>

namespace RAYX
{

    /**
     * new constructor
     * 
     * initializes transformation matrices, and parameters for the quadric in super class (optical element)
     * sets mirror-specific parameters in this class
     * @param name                  name of the plane mirror
     * @param width                 width of mirror (x-dimension in element coord. sys.)
     * @param height                height of mirror (z-dimension in element coord. sys.)
     * @param position              position in world coordinates (in homogeneous coordinates)
     * @param orientation           orientation in world coordinates
     * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * 
     */
    PlaneMirror::PlaneMirror(const char* name, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, geometricalShape, width,
            height, position, orientation, { 0,0,0,0,0,0 }, slopeError)
    {
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0}));
    }

    PlaneMirror::~PlaneMirror()
    {
    }

    std::shared_ptr<PlaneMirror> PlaneMirror::createFromXML(rapidxml::xml_node<>* node) {
        const char* name = node->first_attribute("name")->value();

        int geometricalShape;
        if (!xml::paramInt(node, "geometricalShape", &geometricalShape)) { return nullptr; }

        double width;
        if (!xml::paramDouble(node, "totalWidth", &width)) { return nullptr; }

        double height;
        if (!xml::paramDouble(node, "totalLength", &height)) { return nullptr; }

        glm::dvec4 position;
        if (!xml::paramPosition(node, &position)) { return nullptr; }

        glm::dmat4x4 orientation;
        if (!xml::paramOrientation(node, &orientation)) { return nullptr; }

        std::vector<double> slopeError;
        if (!xml::paramSlopeError(node, &slopeError)) { return nullptr; }

        return std::make_shared<PlaneMirror>(name, geometricalShape, width, height, position, orientation, slopeError);
    }


}
