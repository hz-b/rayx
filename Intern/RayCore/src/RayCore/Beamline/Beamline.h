#pragma once

#include "Core.h"
#include "glm.hpp"

// TODO:
#include "Objects/PlaneGrating.h"
#include "Objects/PlaneMirror.h"
#include "Objects/SphereGrating.h"
#include "Objects/ImagePlane.h"
#include "Objects/MatrixSource.h"
#include "Objects/Ellipsoid.h"
#include "Objects/PointSource.h"
#include "Objects/SphereMirror.h"
#include "Objects/ReflectionZonePlate.h"
#include "Objects/RandomRays.h"
#include "Objects/Slit.h"

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