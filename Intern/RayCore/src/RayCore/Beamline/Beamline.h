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
#include "Slit.h"

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

        //Somehow results in wrong values. Should be fixed later
        //void addQuadric(Quadric newObject);

        void addOpticalElement(const std::shared_ptr<OpticalElement> q);
        void addOpticalElement(const char* name, const std::vector<double>& inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters);
        void addOpticalElement(const char* name, std::vector<double>&& inputPoints, std::vector<double>&& inputInMatrix, std::vector<double>&& inputOutMatrix, std::vector<double>&& misalignmentMatrix, std::vector<double>&& inverseMisalignmentMatrix, std::vector<double>&& OParameters, std::vector<double>&& EParameters);
        void replaceNthObject(uint32_t index, std::shared_ptr<OpticalElement> newObject);
        std::vector<std::shared_ptr<OpticalElement>> getObjects() const;

    private:
        Beamline() = default;
        std::vector<std::shared_ptr<OpticalElement>> m_Objects;

    public:
        Beamline(Beamline const&) = delete;
        void operator=(Beamline const&) = delete;
    };

} // namespace RAY