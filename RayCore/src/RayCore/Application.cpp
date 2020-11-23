#include "Application.h"
#include "BeamLine/BeamLine.h"

#include <iostream>

namespace RAY {

    Application::Application() {
        // std::vector<Ray*> rayList;
        // m_tracerInterface = TracerInterface(rayList);
        std::cout << "Application created" << std::endl;
    }

    Application::~Application() {
        std::cout << "Application deleted" << std::endl;
    }
    

	void Application::Run()
	{
        std::cout << "Application running..." << std::endl;
        // create BeamLine and BeamLineObjects
        BeamLine();
        // TODO: Create a beamlineobject (plane) which can be used to test tracing.

        // create Rays and run tracer over TracerInterface
        // TODO: Does RayCore need a raylist and rays? How is the data transferred between Tracer and RayCore?
        std::vector<Ray *> m_rayList;
        m_tracerInterface.run(m_rayList);
	}

}