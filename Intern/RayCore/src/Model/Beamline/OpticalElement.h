#pragma once

#include <array>
#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"
#include "Model/Surface/Surface.h"
#include "utils.h"
#include "Debug.h"

namespace RAYX {

class RAYX_API OpticalElement {
  public:
    enum class GeometricalShape {
        RECTANGLE = 0,
        ELLIPTICAL,
        TRAPEZOID
    };  ///< influences wastebox function in shader

    struct Geometry {
        double m_widthA = 0.0;
        double m_widthB = 0.0;  //< this width is only used for trapezoid
        double m_height = 0.0;
        double m_azimuthalAngle = 0.0;  // rotation of element through xy-plane
                                        // (needed for stokes vector)
        glm::dmat4x4 m_orientation = glm::dmat4x4();
        glm::dvec4 m_position = glm::dvec4();
        glm::dmat4 m_inMatrix = glm::dmat4();
        glm::dmat4 m_outMatrix = glm::dmat4();
        GeometricalShape m_geometricalShape = GeometricalShape::RECTANGLE;

        /**
         * calculates element to world coordinates transformation matrix and its
         * inverse
         * @param   position     4 element vector which describes the position
         * of the element in world coordinates
         * @param   orientation  4x4 matrix that describes the orientation of
         * the surface with respect to the world coordinate system
         * @return void
         */
        void calcTransformationMatrices(glm::dvec4 position,
                                                  glm::dmat4x4 orientation) {
            RAYX_LOG << "Calculated orientation";
            for (int i = 0; i < 4; i++) {
                std::stringstream s;
                s.precision(17);
                s << '\t';
                for (int j = 0; j < 4; j++) {
                    s << orientation[i][j] << ", ";
                }
                RAYX_LOG << s.str();
            }
            std::stringstream s;
            s.precision(17);
            s << "Position: ";
            for (int i = 0; i < 4; i++) {
                s << position[i] << ", ";
            }
            RAYX_LOG << s.str();

            glm::dmat4x4 translation =
                glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0,
                             1, -position[2], 0, 0, 0, 1);  // o
            glm::dmat4x4 inv_translation =
                glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0,
                             1, position[2], 0, 0, 0, 1);  // o
            glm::dmat4x4 rotation = glm::dmat4x4(
                orientation[0][0], orientation[0][1], orientation[0][2], 0.0,
                orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                orientation[2][0], orientation[2][1], orientation[2][2], 0.0,
                0.0, 0.0, 0.0, 1.0);  // o
            glm::dmat4x4 inv_rotation = glm::transpose(rotation);

            // ray = tran * rot * ray
            glm::dmat4x4 g2e = translation * rotation;
            m_inMatrix = glm::transpose(g2e);

            // inverse of m_inMatrix
            glm::dmat4x4 e2g = inv_rotation * inv_translation;
            m_outMatrix = glm::transpose(e2g);

            /*RAYX_LOG << "from position and orientation";
            printDMatrix(m_inMatrix);
            printDMatrix(m_outMatrix);*/
        }
    };

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
                   GeometricalShape geometricalShape, const double width,
                   const double height, const double azimuthalAngle,
                   glm::dvec4 position, glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);
    OpticalElement(const char* name,
                   const std::array<double, 4 * 4> EParameters,
                   GeometricalShape geometricalShape, const double width,
                   const double widthB, const double height,
                   const double azimuthalAngle, glm::dvec4 position,
                   glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);
    OpticalElement(const char* name, GeometricalShape geometricalShape,
                   const double width, const double height,
                   const double azimuthalAngle, glm::dvec4 position,
                   glm::dmat4x4 orientation,
                   const std::array<double, 7> slopeError);
    OpticalElement(const char* name, GeometricalShape geometricalShape,
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
    virtual ~OpticalElement();

    const char* m_name;

  private:
    // GEOMETRY
    std::unique_ptr<Geometry> m_Geometry;  // will replace all of the following
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
