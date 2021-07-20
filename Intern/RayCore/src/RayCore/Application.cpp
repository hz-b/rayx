#include "Application.h"
#include "Beamline/Beamline.h"
#include "Debug.h"
#include "Data/Exporter.h"

#include <iostream>

namespace RAYX
{

    Application::Application()
    {
        RAY_DEBUG(std::cout << "Creating Application..." << std::endl);
    }

    Application::~Application()
    {
        RAY_DEBUG(std::cout << "Deleting Application..." << std::endl);
    }

    void Application::run()
    {
        RAY_DEBUG(std::cout << "Application running..." << std::endl);
    }

    // void Application::run(const std::string& filename)
    // {
    //     RAY_DEBUG(std::cout << "Application running on given file..." << std::endl);

    //     // TODO: read file here
    //     //Exporter exp;
    //     //exp.exportToXML();

    //     m_tracerInterface.run();
    // }

} // namespace RAYX