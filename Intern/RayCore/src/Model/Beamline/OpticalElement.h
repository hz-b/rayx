#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "BeamlineObject.h"
#include "Core.h"
#include "Model/Surface/Surface.h"
#include "utils.h"
#include "Model/Geometry/Geometry.h"
#include <glm.hpp>

namespace RAYX
{

    class RAYX_API OpticalElement : public BeamlineObject
    {
    public:

        // needed to add optical elements to tracer
        OpticalElement(const char* name, const std::vector<double> surfaceParams, const std::vector<double> inputInMatrix, const std::vector<double> inputOutMatrix, const std::vector<double> OParameters, const std::vector<double> EParameters);

        // new constructors
        OpticalElement(const char* name, const std::vector<double> EParameters, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);
        OpticalElement(const char* name, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);
        OpticalElement(const char* name, Geometry::GeometricalShape geometricalShape, const double widthA, const double widthB, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError);


        void setElementParameters(std::vector<double> params);
        void setInMatrix(std::vector<double> inputMatrix);
        void setOutMatrix(std::vector<double> inputMatrix);
        void setSurface(std::unique_ptr<Surface> surface);
        void updateObjectParams();
        void updateObjectParamsNoGeometry();

        double getWidth();
        double getHeight();

        std::vector<double> getInMatrix() const;
        std::vector<double> getOutMatrix() const;
        std::vector<double> getObjectParameters();
        std::vector<double> getElementParameters() const;
        std::vector<double> getSurfaceParams() const;
        std::vector<double> getSlopeError() const;

        OpticalElement();
        ~OpticalElement();

        // TODO(Jannis): move to geometry
        enum GRATING_MOUNT { GM_DEVIATION, GM_INCIDENCE }; ///< influences incidence and exit angle calculation (moved somewhere else)

    private:

        // GEOMETRY
        std::unique_ptr<Geometry> m_geometry; // will replace all of the following attributes (up until surface)
        // SURFACE (eg Quadric or if eg torus something else)
        std::unique_ptr<Surface> m_surfacePtr;
        std::vector<double> m_surfaceParams; // used to be anchor points

        // Geometric Parameter
        // 7 paramters that specify the slope error, are stored in objectParamters to give to shader
        std::vector<double> m_slopeError;

        std::vector<double> m_inMatrix;
        std::vector<double> m_outMatrix;

        // things every optical element has (e.g. slope error) (16 entries -> one dmat4x4 in shader)
        // also put to shader
        std::vector<double> m_objectParameters;
        // additional element-specific parameters that are used for tracing (16 entries -> one dmat4x4 in shader)
        std::vector<double> m_elementParameters;
    };


} // namespace RAYX

