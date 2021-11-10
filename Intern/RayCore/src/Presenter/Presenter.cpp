#include "Presenter.h"
#include "Debug.h"
#include "Model/Beamline/OpticalElement.h"

namespace RAYX
{

    Presenter::Presenter()
    {

    }

    Presenter::Presenter(std::shared_ptr<Beamline> beamline) :
        m_Beamline(beamline),
        m_TracerInterface(TracerInterface(beamline->m_OpticalElements.size(), SimulationEnv::get().m_numOfRays))
    {
    }

    Presenter::~Presenter()
    {

    }


    // ! parameters are temporary and need to be removed again
    bool Presenter::run()
    {
        /*int beamlinesSimultaneously = 1;*/
        for (int j = 0; j < 1/*beamlinesSimultaneously*/; j++) {
            if (m_Beamline->m_LightSources.empty()) {
                std::cerr << "(Presenter) There is no light source!\n";
                exit(1);
            }
            else {
                m_TracerInterface.generateRays(m_Beamline->m_LightSources[0]);
            }
        }

        m_TracerInterface.setBeamlineParameters();
        const std::vector<std::shared_ptr<OpticalElement>>& Elements = m_Beamline->m_OpticalElements;
        for (int j = 0; j < 1/*beamlinesSimultaneously*/; j++) {
            for (int i = 0; i < int(Elements.size()); i++) {
                std::cout << "add " << Elements[i]->getName() << std::endl;
                m_TracerInterface.addOpticalElementToTracer(Elements[i]);
            }
        }
        return m_TracerInterface.run();
    }



    /** Adds new light source to light sources.
     *
     *  @param newSource shared pointer to light source to be added
     */
    void Presenter::addLightSource(std::shared_ptr<LightSource> newSource)
    {
        m_Beamline->m_LightSources.push_back(newSource);
    }
} // namespace RAYX