#include "ImagePlane.h"
#include <Data/xml.h>

namespace RAYX
{

    /**
     * initializes transformation matrices from position and orientation, and sets parameters for the quadric in super class (optical Element)
     * @param   name            name of ImagePlane
     * @param   position        distance to preceeding element
     * @param   orientation     pointer to previous element in beamline, needed for calculating world coordinates
     * 
    */
    ImagePlane::ImagePlane(const char* name, glm::dvec4 position, glm::dmat4x4 orientation)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, 0, 0, 0, position, orientation, { 0,0,0,0,0, 0,0 }) {        
        setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0 }));        
    }

    ImagePlane::~ImagePlane()
    {
    }

    std::shared_ptr<ImagePlane> ImagePlane::createFromXML(rapidxml::xml_node<>* node) {
        const char* name = node->first_attribute("name")->value();

        glm::dvec4 position;
        if (!xml::paramPosition(node, &position)) { return nullptr; }

        glm::dmat4x4 orientation;
        if (!xml::paramOrientation(node, &orientation)) { return nullptr; }

        return std::make_shared<ImagePlane>(name, position, orientation);
    }

}
