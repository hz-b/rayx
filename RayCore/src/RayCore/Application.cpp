#include "Application.h"
#include "BeamLine/BeamLine.h"
#include "Debug.h"
#include "Data/Exporter.h"

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

    void Application::run()
    {
        DEBUG(std::cout << "Application running..." << std::endl);
        m_tracerInterface.run();
    }

    void Application::run(const std::string& filename)
    {
        DEBUG(std::cout << "Application running on given file..." << std::endl);

        // TODO: read file here
        //Exporter exp;
        //exp.exportToXML();

        m_tracerInterface.run();
    }

} // namespace RAY