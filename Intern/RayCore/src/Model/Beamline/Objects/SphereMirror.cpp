#include "SphereMirror.h"

namespace RAYX
{

    /**
     * Calculates transformation matrices, and sets parameters for the quadric surface.
     * Sets mirror-specific parameters in this class.
     * calculates radius from incidence angle, entrance and exit arm lengths
     *
     * @param name
     * @param width
     * @param height
     * @param grazingIncidenceAngle     angle in which the main ray should hit the element. given in degree
     * @param position                  position of element in world coordinates
     * @param orientation               orientation of element in world coordinates
     * @param entranceArmLength
     * @param exitArmLength
     * @param slopeError
     *
    */
    SphereMirror::SphereMirror(const char* name, const int geometricalShape, const double width, const double height, const double grazingIncidenceAngle, glm::dvec4 position, glm::dmat4x4 orientation, const double entranceArmLength, const double exitArmLength, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, geometricalShape, width, height, position, orientation, { 0,0,0,0,0,0 }, slopeError),
        m_entranceArmLength(entranceArmLength),
        m_exitArmLength(exitArmLength),
        m_grazingIncidenceAngle(degToRad(grazingIncidenceAngle))

    {
        calcRadius(); // calculate the radius
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, 1,1,0,-m_radius, 0,0,1,0, 0,0,0,0}));
    }

     /**
     * Calculates transformation matrices, and sets parameters for the quadric surface.
     * Sets mirror-specific parameters in this class.
     * Radius is not calculated but given as a parameter
     *
     * @param name
     * @param width
     * @param height
     * @param radius                    radius of sphere
     * @param position                  position of element in world coordinates
     * @param orientation               orientation of element in world coordinates
     * @param slopeError
    */
    SphereMirror::SphereMirror(const char* name, const int geometricalShape, const double width, const double height, double radius, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, geometricalShape, width, height, position, orientation, { 0,0,0,0,0,0 }, slopeError)
    {
        setSurface(std::make_unique<Quadric>(std::vector<double>{1,0,0,0, 1,1,0,-radius, 0,0,1,0, 0,0,0,0}));
    }


    SphereMirror::~SphereMirror()
    {
    }

    std::shared_ptr<SphereMirror> SphereMirror::createFromXML(rapidxml::xml_node<>* node) {
        const char* name = node->first_attribute("name")->value();

        int geometricalShape;
        if (!xml::paramInt(node, "geometricalShape", &geometricalShape)) { return nullptr; }

        double width;
        if (!xml::paramDouble(node, "totalWidth", &width)) { return nullptr; }

        double height;
        if (!xml::paramDouble(node, "totalLength", &height)) { return nullptr; }

        double grazingIncidenceAngle;
        if (!xml::paramDouble(node, "grazingIncAngle", &grazingIncidenceAngle)) { return nullptr; }

        glm::dvec4 position;
        if (!xml::paramPosition(node, &position)) { return nullptr; }

        glm::dmat4x4 orientation;
        if (!xml::paramOrientation(node, &orientation)) { return nullptr; }

        double entranceArmLength;
        if (!xml::paramDouble(node, "entranceArmLength", &entranceArmLength)) { return nullptr; }

        double exitArmLength;
        if (!xml::paramDouble(node, "exitArmLength", &exitArmLength)) { return nullptr; }

        std::vector<double> slopeError;
        if (!xml::paramSlopeError(node, &slopeError)) { return nullptr; }

        return std::make_shared<SphereMirror>(name, geometricalShape, width, height, grazingIncidenceAngle, position, orientation, entranceArmLength, exitArmLength, slopeError);
    }

    // TODO(Theresa): move this to user params and just give the radius as a parameter to the sphere class?
    void SphereMirror::calcRadius() {
        m_radius = 2.0 / sin(m_grazingIncidenceAngle) / (1.0 / m_entranceArmLength + 1.0 / m_exitArmLength);
    }


    double SphereMirror::getRadius() const {
        return m_radius;
    }


    double SphereMirror::getExitArmLength() const {
        return m_exitArmLength;
    }

    double SphereMirror::getEntranceArmLength() const {
        return m_entranceArmLength;
    }
}
