#include "Application.h"
#include "Beamline/Beamline.h"
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
        if (m_argc >= 4) {
            m_tracerInterface.run(std::stod(std::string(m_argv[1])),
                std::stod(std::string(m_argv[2])), std::stod(std::string(m_argv[3])));
        }
        else {
            m_tracerInterface.run(0.0, 0.0, 0.0);
        }
    }

    // void Application::run(const std::string& filename)
    // {
    //     DEBUG(std::cout << "Application running on given file..." << std::endl);

    //     // TODO: read file here
    //     //Exporter exp;
    //     //exp.exportToXML();

    //     m_tracerInterface.run();
    // }

} // namespace RAY