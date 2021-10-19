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
#include "Objects/ToroidMirror.h"

#include <vector>

namespace RAYX
{

    class RAYX_API Beamline
    {
    public:
        Beamline();
        ~Beamline();

        void addOpticalElement(const std::shared_ptr<OpticalElement> q);
        void addOpticalElement(const char* name, const std::vector<double>& inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters);
        void addOpticalElement(const char* name, std::vector<double>&& inputPoints, std::vector<double>&& inputInMatrix, std::vector<double>&& inputOutMatrix, std::vector<double>&& misalignmentMatrix, std::vector<double>&& inverseMisalignmentMatrix, std::vector<double>&& OParameters, std::vector<double>&& EParameters);
        std::vector<std::shared_ptr<OpticalElement>> getObjects() const; // TODO(rudi) return BeamlineObjects! see the .cpp file for more info
        int size() const;

        // TODO(rudi): make this private again!
        std::vector<std::shared_ptr<BeamlineObject>> m_Objects;

    };

} // namespace RAYX