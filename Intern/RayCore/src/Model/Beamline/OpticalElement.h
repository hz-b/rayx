#pragma once

#include <array>
#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "BeamlineObject.h"
#include "Core.h"
#include "Model/Geometry/Geometry.h"
#include "Model/Surface/Surface.h"
#include "utils.h"

namespace RAYX {

class RAYX_API OpticalElement : public BeamlineObject {
  public:
    // needed to add optical elements to tracer
    OpticalElement(const char* name,
                   const std::array<double, 4 * 4> surfaceParams,
                   const std::array<double, 4 * 4> inputInMatrix,
                   const std::array<double, 4 * 4> inputOutMatrix,
                   const std::array<double, 4 * 4> OParameters,
                   const std::array<double, 4 * 4> EParameters);

    // new constructors
    OpticalElement(const char* name,
                   const std::array<double, 4 * 4> EParameters,
                   Geometry::GeometricalShape geometricalShape,
                   const double width, const double height,
                   const double azimuthalAngle, glm::dvec4 position,
                   glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);
    OpticalElement(const char* name,
                   Geometry::GeometricalShape geometricalShape,
                   const double width, const double height,
                   const double azimuthalAngle, glm::dvec4 position,
                   glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);
    OpticalElement(const char* name,
                   Geometry::GeometricalShape geometricalShape,
                   const double widthA, const double widthB,
                   const double height, const double azimuthalAngle,
                   glm::dvec4 position, glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);

    void setElementParameters(std::array<double, 4 * 4> params);
    void setInMatrix(std::array<double, 4 * 4> inputMatrix);
    void setOutMatrix(std::array<double, 4 * 4> inputMatrix);
    void setSurface(std::unique_ptr<Surface> surface);
    void updateObjectParams();
    void updateObjectParamsNoGeometry();

    double getWidth();
    double getHeight();

    std::array<double, 4 * 4> getInMatrix() const;
    std::array<double, 4 * 4> getOutMatrix() const;
    glm::dmat4x4 getOrientation() const;
    glm::dvec4 getPosition() const;
    std::array<double, 4 * 4> getObjectParameters();
    std::array<double, 4 * 4> getElementParameters() const;
    std::array<double, 4 * 4> getSurfaceParams() const;
    std::array<double, 7> getSlopeError() const;

    OpticalElement();
    ~OpticalElement();

  private:
    // GEOMETRY
    std::unique_ptr<Geometry> m_geometry;  // will replace all of the following
                                           // attributes (up until surface)
    // SURFACE (eg Quadric or if eg torus something else)
    std::unique_ptr<Surface> m_surfacePtr;
    std::array<double, 4 * 4> m_surfaceParams;  // used to be anchor points

    // Geometric Parameter
    // 7 paramters that specify the slope error, are stored in objectParamters
    // to give to shader
    std::array<double, 7> m_slopeError;

    std::array<double, 4 * 4> m_inMatrix;
    std::array<double, 4 * 4> m_outMatrix;

    // things every optical element has (e.g. slope error) (16 entries -> one
    // dmat4x4 in shader) also put to shader
    std::array<double, 4 * 4> m_objectParameters;
    // additional element-specific parameters that are used for tracing (16
    // entries -> one dmat4x4 in shader)
    std::array<double, 4 * 4> m_elementParameters;
};

}  // namespace RAYX
