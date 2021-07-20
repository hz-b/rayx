#include "Application.h"
#include "Debug.h"
#include "Data/Exporter.h"

#include <iostream>

namespace RAYX
{

    Application::Application()
    {
        RAYX_DEBUG(std::cout << "Creating Application..." << std::endl);
    }

    Application::~Application()
    {
        RAYX_DEBUG(std::cout << "Deleting Application..." << std::endl);
    }

    void Application::run()
    {
        RAYX_DEBUG(std::cout << "Application running..." << std::endl);
    }

} // namespace RAYX