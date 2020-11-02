#pragma once

#include "Core.h"
#include <vector>

class BeamLineObject;

namespace RAY {
    /**
     *  TODO: A comprehensive description for the BeamLine class
     */
    class RAY_API BeamLine
    {
    public:
        static BeamLine* getInstance();


    private:
        BeamLine();
        virtual ~BeamLine();
        // controller instance (singleton)
        static BeamLine* m_Instance;

        std::vector<BeamLineObject*> m_Objects;
    };

}