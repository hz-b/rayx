#pragma once

#include "Core.h"
#include "BeamLine/BeamLine.h"

namespace RAY {

    class RAY_API Controller
    {
    public:
        static Controller* getInstance();


    private:
        Controller();
        virtual ~Controller();

        // controller instance (singleton)
        static Controller* m_Instance;
        // private members of the ray components which the controller handles
        // static BeamLine* m_Beamline;
        // static TracerInterface* m_Tracer;
        // static Analyser* m_Analyser;
        
        // static Exporter* m_Exporter;
    };  

}