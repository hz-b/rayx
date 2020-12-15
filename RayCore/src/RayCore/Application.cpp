#include "Application.h"
#include "BeamLine/BeamLine.h"
#include "Debug.h"

#include <iostream>

namespace RAY
{

    Application::Application()
    {
        DEBUG(std::cout << "Creating Application..." << std::endl);
    }

    Application::~Application()
    {
        DEBUG(std::cout << "Deleting Application..." << std::endl);
    }

    void Application::Run()
    {
        DEBUG(std::cout << "Application running..." << std::endl);
        m_tracerInterface.run();
    }

} // namespace RAY