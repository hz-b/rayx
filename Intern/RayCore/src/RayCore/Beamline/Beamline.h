#pragma once

#include "Core.h"
#include "glm.hpp"
#include "PlaneGrating.h"
#include "PlaneMirror.h"
#include "SphereGrating.h"
#include "ImagePlane.h"
#include "MatrixSource.h"
#include "Ellipsoid.h"
#include "PointSource.h"
#include "SphereMirror.h"
#include "ReflectionZonePlate.h"
#include "RandomRays.h"

#include <vector>

namespace RAY
{

    class RAY_API Beamline
    {

    public:
        static Beamline& get() {
            static Beamline m_Instance;
            return m_Instance;
        }

        ~Beamline();
        //Somehow results in wrong values. Should be fixed later
        //void addQuadric(Quadric newObject);
        
        void addQuadric(Quadric q);
        void addQuadric(const char* name, std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters);
        void replaceNthObject(uint32_t index, Quadric newObject);
        std::vector<Quadric> getObjects();

    private:
        Beamline();
        Beamline(const Beamline&) = delete;
        std::vector<Quadric> m_Objects;
    };

} // namespace RAY