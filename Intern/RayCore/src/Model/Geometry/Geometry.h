#pragma once

#include <glm.hpp>
#include <vector>

#include "Core.h"

namespace RAYX {

class RAYX_API Geometry {
  public:
    enum class GeometricalShape {
        RECTANGLE = 0,
        ELLIPTICAL,
        TRAPEZOID
    };  ///< influences wastebox function in shader

    Geometry(GeometricalShape geometricShape, double width, double height,
             const double azimuthalAngle, glm::dvec4 position,
             glm::dmat4x4 orientation);
    Geometry(GeometricalShape geometricShape, double widthA, double widthB,
             double height, const double azimuthalAngle, glm::dvec4 position,
             glm::dmat4x4 orientation);
    Geometry();
    ~Geometry();

    void getWidth(double& widthA, double& widthB);
    double getHeight();
    double getAzimuthalAngle();
    std::vector<double> getInMatrix();
    std::vector<double> getOutMatrix();
    glm::dvec4 getPosition();
    glm::dmat4x4 getOrientation();
    void setInMatrix(std::vector<double> inputMatrix);
    void setOutMatrix(std::vector<double> inputMatrix);
    void calcTransformationMatrices(glm::dvec4 position,
                                    glm::dmat4x4 orientation);

  private:
    double m_widthA;
    double m_widthB;  //< this width is only used for trapezoid
    double m_height;
    double m_azimuthalAngle;  // rotation of element through xy-plane (needed
                              // for stokes vector)
    glm::dmat4x4 m_orientation;
    glm::dvec4 m_position;
    std::vector<double> m_inMatrix;
    std::vector<double> m_outMatrix;
    GeometricalShape m_geometricalShape;
};
}  // namespace RAYX