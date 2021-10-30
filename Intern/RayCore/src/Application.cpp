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

#include "Data/Exporter.h"

#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"

namespace RAYX
{

    Application::Application() :
        m_Beamline(std::make_shared<Beamline>())
    {
        RAYX_DEBUG(std::cout << "Creating Application..." << std::endl);
    }

    Application::~Application()
    {
        RAYX_DEBUG(std::cout << "Deleting Application..." << std::endl);
    }

    void Application::loadDummyBeamline()
    {
        // ! objects are created here temporarily until reading in file works
        const clock_t all_begin_time = clock();

        EnergyDistribution dist = EnergyDistribution(EnergyRange(100, 0), true);
        std::shared_ptr<MatrixSource> matSourcePtr = std::make_shared<MatrixSource>("matrix source", dist, 0.065, 0.04, 0.0, 0.001, 0.001, 1, 0, 0, std::vector<double>{ 0, 0, 0, 0 });
        //std::shared_ptr<Slit> s = std::make_shared<Slit>("slit", 1, 2, 20, 2, 7.5, 10000, 20, 1, m->getPhotonEnergy(), std::vector<double>{2, 1, 0, 0, 0, 0 }, nullptr, GLOBAL);

        WorldUserParams param = WorldUserParams(degToRad(10), degToRad(10), 0, 10000, std::vector<double>{1, 2, 3, 0.001, 0.002, 0.003});
        glm::dvec4 pos_mirror = param.calcPosition();
        glm::dmat4x4 or_mirror = param.calcOrientation();
        std::shared_ptr<PlaneMirror> pm = std::make_shared<PlaneMirror>("PM", 0, 50, 200, pos_mirror, or_mirror, std::vector<double>{0, 0, 0, 0, 0, 0, 0});

        WorldUserParams tor_param = WorldUserParams(degToRad(10), degToRad(10), 0, 10000, std::vector<double>{0, 0, 0, 0, 0, 0});
        glm::dvec4 tor_position = tor_param.calcPosition();
        glm::dmat4x4 tor_orientation = tor_param.calcOrientation();
        std::shared_ptr<RAYX::ToroidMirror> t = std::make_shared<RAYX::ToroidMirror>("toroid", 0, 50, 200, tor_position, tor_orientation, degToRad(10), 10000, 1000, 10000, 1000, std::vector<double>{0, 0, 0, 0, 0, 0, 0});

        std::cout << "\n IMAGE PLANE \n" << std::endl;

        WorldUserParams im_param = WorldUserParams(0, 0, 0, 1000, std::vector<double>{0, 0, 0, 0, 0, 0});
        glm::dvec4 pos_imageplane = im_param.calcPosition(tor_param, tor_position, tor_orientation);
        glm::dmat4x4 or_imageplane = im_param.calcOrientation(tor_param, tor_position, tor_orientation);
        std::shared_ptr<ImagePlane> i = std::make_shared<ImagePlane>("Image plane", pos_imageplane, or_imageplane);

        m_Beamline->addOpticalElement(t);
        m_Beamline->addOpticalElement(i);

        m_Presenter = Presenter(m_Beamline);
        m_Presenter.addLightSource(matSourcePtr);
        RAYX_DEBUG(std::cout << "Creating dummy beamline took: " << float(clock() - all_begin_time) / CLOCKS_PER_SEC * 1000 << " ms" << std::endl);
    }

    void Application::run()
    {
        RAYX_DEBUG(std::cout << "Application running..." << std::endl);
    }

} // namespace RAYX