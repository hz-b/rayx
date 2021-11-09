#include <iomanip> 
#include <iostream>

#include "Application.h"
#include "Debug.h"

#include "Model/Beamline/Objects/MatrixSource.h"
#include "Model/Beamline/Objects/PointSource.h"
#include "Model/Beamline/Objects/PlaneMirror.h"
#include "Model/Beamline/Objects/ReflectionZonePlate.h"
#include "Model/Beamline/Objects/ToroidMirror.h"
#include "Model/Beamline/Objects/ImagePlane.h"
#include "Model/Beamline/Objects/Ellipsoid.h"

#include "Data/Exporter.h"

#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"

namespace RAYX
{

    Application::Application() :
        m_Beamline(std::make_shared<Beamline>())
    {
        RAYX_DEBUG(std::cout << "[App]: Creating Application..." << std::endl);
    }

    Application::~Application()
    {
        RAYX_DEBUG(std::cout << "[App]: Deleting Application..." << std::endl);
    }

    void Application::loadDummyBeamline()
    {
        // ! objects are created here temporarily until reading in file works
        const clock_t allBeginTime = clock();

        RAYX::SimulationEnv::get().m_numOfRays = 200;

        EnergyDistribution dist = EnergyDistribution(EnergyRange(100, 0), true);
        std::shared_ptr<MatrixSource> matSourcePtr = std::make_shared<MatrixSource>("matrix source", dist, 0.065, 0.04, 0.0, 0.001, 0.001, 1, 0, 0, std::vector<double>{ 0, 0, 0, 0 });
        //std::shared_ptr<Slit> s = std::make_shared<Slit>("slit", 1, 2, 20, 2, 7.5, 10000, 20, 1, m->getPhotonEnergy(), std::vector<double>{2, 1, 0, 0, 0, 0 }, nullptr, GLOBAL);

        // plane mirror with misalignment
        RAYX::GeometricUserParams pm_params = RAYX::GeometricUserParams(7);
        RAYX::WorldUserParams w_coord = RAYX::WorldUserParams(pm_params.getAlpha(), pm_params.getBeta(), 0, 10000, std::vector<double>{1, 2, 3, 0.004, 0.005, 0.006});
        glm::dvec4 pos1 = w_coord.calcPosition();
        glm::dmat4x4 or1 = w_coord.calcOrientation();
        std::shared_ptr<RAYX::PlaneMirror> pm = std::make_shared<RAYX::PlaneMirror>("pm_ell_ip_200mirrormis", Geometry::GeometricalShape::RECTANGLE, 50, 200, pos1, or1, std::vector<double>{0, 0, 0, 0, 0, 0, 0});

        // ellipsoid with mirror misalignment
        RAYX::GeometricUserParams ell_params = RAYX::GeometricUserParams(10, 10000, 1000);
        int coordinatesystem = 1; // misalignment in mirror coordinate system
        double tangentAngle = ell_params.calcTangentAngle(10, 10000, 1000, coordinatesystem);
        RAYX::WorldUserParams ell_w_coord = RAYX::WorldUserParams(ell_params.getAlpha(), ell_params.getBeta(), 0, 100, std::vector<double>{1, 2, 3, 0.004, 0.005, 0.006}, tangentAngle);
        glm::dvec4 pos2 = ell_w_coord.calcPosition(w_coord, pos1, or1);
        glm::dmat4x4 or2 = ell_w_coord.calcOrientation(w_coord, or1);
        std::shared_ptr<RAYX::Ellipsoid> eb = std::make_shared<RAYX::Ellipsoid>("ellipsoid_ip_200mirrormis", Geometry::GeometricalShape::RECTANGLE, 50, 200, pos2, or2, 10, 10000, 1000, 0, 1, std::vector<double>{0, 0, 0, 0, 0, 0, 0});

        // image plane
        RAYX::WorldUserParams ip_w_coord = RAYX::WorldUserParams(0, 0, 0, 1000, std::vector<double>{0, 0, 0, 0, 0, 0});
        glm::dvec4 pos3 = ip_w_coord.calcPosition(ell_w_coord, pos2, or2);
        glm::dmat4x4 or3 = ip_w_coord.calcOrientation(ell_w_coord, or2);
        std::shared_ptr<RAYX::ImagePlane> i = std::make_shared<RAYX::ImagePlane>("ImagePlane", pos3, or3);

        m_Beamline->addOpticalElement(pm);
        m_Beamline->addOpticalElement(eb);
        m_Beamline->addOpticalElement(i);

        m_Presenter = Presenter(m_Beamline);
        m_Presenter.addLightSource(matSourcePtr);
        RAYX_DEBUG(std::cout << "[App]: Creating dummy beamline took: " << float(clock() - allBeginTime) / CLOCKS_PER_SEC * 1000 << " ms" << std::endl);
    }

    void Application::run()
    {
        RAYX_DEBUG(std::cout << "[App]: Application running..." << std::endl);
    }

} // namespace RAYX