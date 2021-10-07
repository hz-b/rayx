#include "Presenter.h"
#include "Debug.h"

namespace RAYX
{

    Presenter::Presenter()
    {

    }

    Presenter::Presenter(std::shared_ptr<Beamline> beamline) :
        m_Beamline(beamline),
        m_TracerInterface(TracerInterface(beamline->size(), 200000))
    {
    }

    Presenter::~Presenter()
    {

    }


    // ! parameters are temporary and need to be removed again
    bool Presenter::run(double translationXerror, double translationYerror, double translationZerror)
    {   
        /*int beamlinesSimultaneously = 1;*/
        for (int j = 0; j < 1/*beamlinesSimultaneously*/; j++) {
            m_TracerInterface.generateRays(m_lightSources[0]);
        }

        m_TracerInterface.setBeamlineParameters();
        const std::vector<std::shared_ptr<OpticalElement>> Elements = m_Beamline->getObjects();
        for (int j = 0; j < 1/*beamlinesSimultaneously*/; j++) {
            for (int i = 0; i < int(Elements.size()); i++) {
                std::cout << "add " << Elements[i]->getName() << std::endl;
                m_TracerInterface.addOpticalElementToTracer(Elements[i]);
            }
        }
        m_TracerInterface.run(translationXerror, translationYerror, translationZerror);
    }



    /** Adds new light source to light sources.
     *
     *  @param newSource shared pointer to light source to be added
     */
    void Presenter::addLightSource(std::shared_ptr<LightSource> newSource)
    {
        m_lightSources.push_back(newSource);
    }
} // namespace RAYX