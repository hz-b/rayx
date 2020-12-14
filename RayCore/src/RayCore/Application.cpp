#include "Application.h"
#include "BeamLine/BeamLine.h"

#include <iostream>

namespace RAY {

    Application::Application() {
        std::cout << "Application created" << std::endl;
    }

    Application::~Application() {
        std::cout << "Application deleted" << std::endl;
    }

	void Application::Run()
	{
        std::cout << "Application running..." << std::endl;
        m_tracerInterface.run();
	}

}