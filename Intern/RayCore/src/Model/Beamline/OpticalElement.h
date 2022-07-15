#pragma once

#include <array>
#include <glm.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core.h"
#include "Debug.h"
#include "Model/Surface/Surface.h"
#include "utils.h"

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
        GeometricalShape m_geometricalShape = GeometricalShape::RECTANGLE;

        // Default CTOR
        Geometry() {
            m_widthA = 0.0;          ///< x-dimension of element
            m_widthB = 0.0;          ///< this width is only used for trapezoid
            m_height = 0.0;          ///< z-dimension of element
            m_azimuthalAngle = 0.0;  // rotation of element through xy-plane
                                     // (needed for stokes vector)
            m_orientation =
                glm::dmat4x4();  ///< orientation in world coordinate system
            m_position = glm::dvec4();  ///< position in world coordinates
            m_geometricalShape = GeometricalShape::RECTANGLE;
        }
        // Copy CTOR
        Geometry(const Geometry& geometry) {
            m_widthA = geometry.m_widthA;
            m_widthB = geometry.m_widthB;
            m_height = geometry.m_height;
            m_azimuthalAngle = geometry.m_azimuthalAngle;
            m_orientation = geometry.m_orientation;
            m_position = geometry.m_position;
            m_geometricalShape = geometry.m_geometricalShape;
        }

        void setHeightWidth(double height, double widthA, double widthB = 0.0) {
            m_widthB = widthB;
            if (m_geometricalShape == GeometricalShape::ELLIPTICAL) {
                m_widthA = -widthA;
                m_height = -height;
            } else {
                m_widthA = widthA;
                m_height = height;
            }
        }
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
                                        glm::dmat4 orientation,
                                        glm::dmat4& output,
                                        bool calcInMatrix = true) {
#ifdef RAYX_DEBUG_MODE
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
#endif

            glm::dmat4x4 rotation = glm::dmat4x4(
                orientation[0][0], orientation[0][1], orientation[0][2], 0.0,
                orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                orientation[2][0], orientation[2][1], orientation[2][2], 0.0,
                0.0, 0.0, 0.0, 1.0);  // o
            glm::dmat4x4 inv_rotation = glm::transpose(rotation);

            if (calcInMatrix) {
                glm::dmat4x4 translation =
                    glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0,
                                1, -position[2], 0, 0, 0, 1);  // o
                // ray = tran * rot * ray
                glm::dmat4x4 g2e = translation * rotation;
                output = glm::transpose(g2e);
                return;
            } else {
                glm::dmat4x4 inv_translation =
                    glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0,
                                1, position[2], 0, 0, 0, 1);  // o
                // inverse of m_inMatrix
                glm::dmat4x4 e2g = inv_rotation * inv_translation;
                output = glm::transpose(e2g);
                return;
            }
        }
    };

    // needed to add optical elements to tracer
    OpticalElement(const char* name,
                   const std::array<double, 4 * 4> surfaceParams,
                   const std::array<double, 4 * 4> OParameters,
                   const std::array<double, 4 * 4> EParameters);

    OpticalElement(const char* name,
                   const std::array<double, 4 * 4> EParameters,
                   const std::array<double, 7> slopeError,
                   const Geometry& geometry = Geometry());

    OpticalElement(const char* name, const std::array<double, 7> slopeError,
                   const Geometry& geometry = Geometry());

    virtual ~OpticalElement() = default;

    void setElementParameters(std::array<double, 4 * 4> params);
    void setSurface(std::unique_ptr<Surface> surface);
    void updateObjectParams();
    void updateObjectParamsNoGeometry();

    double getWidth();
    double getHeight();

    glm::dmat4 getInMatrix() const;
    glm::dmat4 getOutMatrix() const;
    glm::dmat4x4 getOrientation() const;
    glm::dvec4 getPosition() const;
    std::array<double, 4 * 4> getObjectParameters();
    std::array<double, 4 * 4> getElementParameters() const;
    std::array<double, 4 * 4> getSurfaceParams() const;
    std::array<double, 7> getSlopeError() const;

    const char* m_name;

  protected:
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

    // things every optical element has (e.g. slope error) (16 entries -> one
    // dmat4x4 in shader) also put to shader
    std::array<double, 4 * 4> m_objectParameters;
    // additional element-specific parameters that are used for tracing (16
    // entries -> one dmat4x4 in shader)
    std::array<double, 4 * 4> m_elementParameters;
};

}  // namespace RAYX
