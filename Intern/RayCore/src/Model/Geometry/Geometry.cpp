#include "Geometry.h"
#include "Ray.h"
#include "utils.h"

#include <math.h>
#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace RAYX
{
    Geometry::Geometry(const int geometricShape, double width, double height, glm::dvec4 position, glm::dmat4x4 orientation)
    {
        m_geometricalShape = geometricShape == 0 ? GS_RECTANGLE : GS_ELLIPTICAL;
        if (m_geometricalShape == GS_ELLIPTICAL) {
            m_width = -width;
            m_height = -height);
        }
        else {
            m_width = width;
            m_height = height);
        }
        // calculate in and out matrices
    }


    Geometry::Geometry() {}
    Geometry::~Geometry() {}
}