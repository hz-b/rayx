#pragma once

#include <array>
#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"
#include "Data/xml.h"
#include "Debug.h"
#include "Model/Surface/Surface.h"
#include "utils.h"

namespace RAYX {

enum class GratingMount { Deviation, Incidence };
enum class ImageType { Point2Point, Astigmatic2Astigmatic };
enum class GeometricalShape { RECTANGLE = 0, ELLIPTICAL, TRAPEZOID };  ///< influences wastebox function in shader

class RAYX_API OpticalElement {
  public:
    struct Geometry {
        double m_widthA = 0.0;
        double m_widthB = 0.0;  //< this width is only used for trapezoid
        double m_height = 0.0;
        double m_azimuthalAngle = 0.0;                // rotation of element through xy-plane
                                                      // (needed for stokes vector)
        glm::dmat4x4 m_orientation = glm::dmat4x4();  //< Orientation matrix of element (is basis)
        glm::dvec4 m_position = glm::dvec4();         //< Position of element in world coordinates
        GeometricalShape m_geometricalShape = GeometricalShape::RECTANGLE;

        Geometry();
        Geometry(const Geometry& other);
        void setHeightWidth(double height, double widthA, double widthB = 0.0);
    };

    OpticalElement(const DesignObject&);
    // needed to add optical elements to tracer
    OpticalElement(const char* name, const std::array<double, 7> slopeError,
                   const Geometry& geometry = Geometry());  // TODO(Jannis): add surface

    virtual ~OpticalElement() = default;

    void setSurface(std::unique_ptr<Surface> surface);

    void calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, glm::dmat4& output, bool calcInMatrix = true) const;

    double getWidth();
    double getHeight();

    glm::dmat4 getInMatrix() const;
    glm::dmat4 getOutMatrix() const;
    glm::dmat4x4 getOrientation() const;
    glm::dvec4 getPosition() const;

    glm::dmat4x4 getObjectParameters() const;
    glm::dmat4x4 getSurfaceParams() const;
    virtual glm::dmat4x4 getElementParameters() const;
    std::array<double, 7> getSlopeError() const;

    [[maybe_unused]] const char* m_name;

  protected:
    std::unique_ptr<Geometry> m_Geometry;   ///< Geometry of the element
    std::unique_ptr<Surface> m_surfacePtr;  ///< Surface of the element

    std::array<double, 7> m_slopeError;  // TODO(Jannis): move to geometry
};

}  // namespace RAYX
