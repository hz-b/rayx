#include <iomanip> 
#include <iostream>

#include "Application.h"
#include "Debug.h"
#include "Data/Exporter.h"
#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldCoordinates.h"

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

        std::shared_ptr<MatrixSource> matSourcePtr = std::make_shared<MatrixSource>(0, "matrix source", 20000, 0, 0.065, 0.04, 0.0, 0.001, 0.001, 100, 0, 1, 0, 0, std::vector<double>{ 0, 0, 0, 0 });
        //std::shared_ptr<Slit> s = std::make_shared<Slit>("slit", 1, 2, 20, 2, 7.5, 10000, 20, 1, m->getPhotonEnergy(), std::vector<double>{2, 1, 0, 0, 0, 0 }, nullptr, GLOBAL);

        std::vector<double> sE = { 0,0,0,0,0, 0,0 };
        RAYX::GeometricUserParams g_params = RAYX::GeometricUserParams(10);
        RAYX::WorldCoordinates w_coord = RAYX::WorldCoordinates(degToRad(10), degToRad(10), degToRad(7), 2000, std::vector<double>{5,7,10, 0.04,0.03,0.05});
        glm::dvec4 pos1 = w_coord.calcPosition();
        glm::dmat4x4 or1 =  w_coord.calcOrientation();
        std::shared_ptr<RAYX::PlaneMirror> p1 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror1", 50, 200, 10, 7, 2000, std::vector<double>{5,7,10, 0.04,0.03,0.05}, sE, nullptr, true); // {1,2,3,0.01,0.02,0.03}
        std::shared_ptr<RAYX::PlaneMirror> p1a = std::make_shared<RAYX::PlaneMirror>("PlaneMirror1", 50, 200, pos1, or1, sE); // {1,2,3,0.01,0.02,0.03}
        
        RAYX::GeometricUserParams g_params2 = RAYX::GeometricUserParams(15);
        RAYX::WorldCoordinates w_coord2 = RAYX::WorldCoordinates(degToRad(15), degToRad(15), degToRad(4), 7000, std::vector<double>{1,2,3,0.04,0.01,0.06});//std::vector<double>{2,4,6, 0.004,0.001,0.006});
        glm::dvec4 pos2 = w_coord2.calcPosition(w_coord, pos1, or1);
        glm::dmat4x4 or2 =  w_coord2.calcOrientation(w_coord, pos1, or1);
        std::shared_ptr<RAYX::PlaneMirror> p2 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror2", 50, 200, 15, 4, 7000, std::vector<double>{1,2,3,0.04,0.01,0.06}, sE, p1, true); // {1,2,3,0.01,0.02,0.03}
        std::shared_ptr<RAYX::PlaneMirror> p2b = std::make_shared<RAYX::PlaneMirror>("PlaneMirror2", 50, 200, pos2, or2, sE); // {1,2,3,0.01,0.02,0.03}
        
        m_Beamline->addOpticalElement(p1);
        m_Beamline->addOpticalElement(p2);

        m_Presenter = Presenter(m_Beamline);
        m_Presenter.addLightSource(matSourcePtr);
        RAYX_DEBUG(std::cout << "Creating dummy beamline took: " << float(clock() - all_begin_time) << " ms" << std::endl);
    }

    void Application::run()
    {
        RAYX_DEBUG(std::cout << "Application running..." << std::endl);
    }

} // namespace RAYX