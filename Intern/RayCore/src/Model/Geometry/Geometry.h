#pragma once

#include "Core.h"
#include "Model/Beamline/OpticalElement.h"
#include <glm.hpp>

namespace RAYX
{

    class RAYX_API Geometry {

    public:

        Geometry(const int geometricShape, double width, double height, glm::dvec4 position, glm::dmat4x4 orientation);
        Geometry();
        ~Geometry();


    private:
        double m_width;
        double m_height;
        glm::dmat4x4 inMatrix;
        glm::dmat4x4 outMatrix;
        OpticalElement::GEOMETRICAL_SHAPE m_geometricalShape;
    };
}