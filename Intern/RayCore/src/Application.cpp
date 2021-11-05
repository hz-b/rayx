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

        RAYX::GeometricUserParams g_params = RAYX::GeometricUserParams(10, 10000, 1000);
        int coordinatesystem = 1; // 1 = misalignment in mirror coordinate system
        double tangentAngle = g_params.calcTangentAngle(10, 10000, 1000, coordinatesystem);
        
        RAYX::WorldUserParams w_coord = RAYX::WorldUserParams(g_params.getAlpha(), g_params.getBeta(), 0, 10000, std::vector<double>{1,2,3, 0.004,0.005,0.006}, tangentAngle);
        glm::dvec4 pos = w_coord.calcPosition();
        glm::dmat4x4 or1 = w_coord.calcOrientation();

        std::shared_ptr<RAYX::Ellipsoid> eb = std::make_shared<RAYX::Ellipsoid>("ellipsoid_ip_200mirrormis", 0, 50, 200, pos, or1, 10, 10000, 1000, 0, 1, std::vector<double> {0, 0, 0, 0, 0, 0, 0});
        eb->setOutMatrix(glmToVector16(glm::transpose(w_coord.calcE2B())));
        std::cout << "\n [App]: IMAGE PLANE \n" << std::endl;

        WorldUserParams im_param = WorldUserParams(0, 0, 0, 1000, std::vector<double>{0, 0, 0, 0, 0, 0});
        glm::dvec4 pos_imageplane = im_param.calcPosition(w_coord, pos, or1);
        glm::dmat4x4 or_imageplane = im_param.calcOrientation(w_coord, pos, or1);
        std::shared_ptr<ImagePlane> i = std::make_shared<ImagePlane>("Image plane", pos_imageplane, or_imageplane);

        m_Beamline->addOpticalElement(eb);
        //m_Beamline->addOpticalElement(i);

        m_Presenter = Presenter(m_Beamline);
        m_Presenter.addLightSource(matSourcePtr);
        RAYX_DEBUG(std::cout << "[App]: Creating dummy beamline took: " << float(clock() - allBeginTime) / CLOCKS_PER_SEC * 1000 << " ms" << std::endl);
    }

    void Application::run()
    {
        RAYX_DEBUG(std::cout << "[App]: Application running..." << std::endl);
    }

} // namespace RAYX