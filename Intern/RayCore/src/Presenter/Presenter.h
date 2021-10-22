#pragma once

#include <memory>

#include "Model/Beamline/LightSource.h"
#include "Model/Beamline/Beamline.h"
#include "Core.h"
#include "Tracer/TracerInterface.h"

namespace RAYX
{
    class RAYX_API Presenter {
    public:
        Presenter();
        Presenter(std::shared_ptr<Beamline> beamline);
        ~Presenter();

        bool run(double translationXerror, double translationYerror, double translationZerror);

        void addLightSource(std::shared_ptr<LightSource> newSource);
    private:
        std::shared_ptr<Beamline> m_Beamline;
        TracerInterface m_TracerInterface;
    };

} // namespace RAYX
