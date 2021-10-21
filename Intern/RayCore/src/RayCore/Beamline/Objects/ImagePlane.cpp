#include "ImagePlane.h"
#include <Data/xml.h>

namespace RAYX
{

    /**
     * initializes transformation matrices, and parameters for the quadric in super class (optical element)
     * @param   name        name of ImagePlane
     * @param   distance    distance to preceeding element
     * @param   previous    pointer to previous element in beamline, needed for calculating world coordinates
     * @param   global
     *
    */
    ImagePlane::ImagePlane(const char* name, const double distance, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, 0, 0, 0, distance, { 0,0,0,0,0, 0,0 }, previous) {
        m_distance = distance;
        setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0 }));
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        calcTransformationMatricesFromAngles({ 0,0,0, 0,0,0 }, global);
        setElementParameters({ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 });
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
    }

    /**
     * initializes transformation matrices, and parameters for the quadric in super class (optical Element)
     * @param   name        name of ImagePlane
     * @param   position    distance to preceeding element
     * @param   orientation    pointer to previous element in beamline, needed for calculating world coordinates
     * 
    */
    ImagePlane::ImagePlane(const char* name, glm::dvec4 position, glm::dmat4x4 orientation)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, 0, 0, position, orientation, { 0,0,0, 0,0,0 }, { 0,0,0,0,0, 0,0 }) {
        
        setSurface(std::make_unique<Quadric>(std::vector<double>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0 }));
        // std::vector<double> inputPoints = {0,0,0,0, 0,0,0,-1, 0,0,0,0, 0,0,0,0};
        
    }

    ImagePlane::~ImagePlane()
    {
    }

    std::shared_ptr<ImagePlane> ImagePlane::createFromXML(rapidxml::xml_node<>* node) {
        const char* name = node->first_attribute("name")->value();

        glm::dvec3 position3;
        if (!xml::paramDvec3(node, "worldPosition", &position3)) { return nullptr; }
        glm::dvec4 position(position3, 1); // TODO(rudi): is this '1' correct?

        glm::dvec3 worldXdirection, worldYdirection, worldZdirection;
        if (!xml::paramDvec3(node, "worldXdirection", &worldXdirection)) { return nullptr; }
        if (!xml::paramDvec3(node, "worldYdirection", &worldYdirection)) { return nullptr; }
        if (!xml::paramDvec3(node, "worldZdirection", &worldZdirection)) { return nullptr; }

        glm::dmat4x4 orientation(1.0f);

        orientation[0][0] = worldXdirection.x;
        orientation[1][0] = worldXdirection.y;
        orientation[2][0] = worldXdirection.z;

        orientation[0][1] = worldYdirection.x;
        orientation[1][1] = worldYdirection.y;
        orientation[2][1] = worldYdirection.z;

        orientation[0][2] = worldZdirection.x;
        orientation[1][2] = worldZdirection.y;
        orientation[2][2] = worldZdirection.z;

        // TODO(rudi): orientation needs to be possibly transposed, depending on what you count as row & column in a glm::dmat4x4
/*
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << orientation[i][j] << " ";
            }
            std::cout << std::endl;
        }
*/

        return std::make_shared<ImagePlane>(name, position, orientation);
    }

    double ImagePlane::getDistance() {
        return m_distance;
    }

}
