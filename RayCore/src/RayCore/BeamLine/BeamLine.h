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
        BeamLine();
        ~BeamLine();

        void addObject();
        
        int numObjects() const { return m_Objects.size(); }
        int numLightSources() const;

        
        BeamLineObject* getObject(int i);
        int getObjectIndex(const BeamLineObject &obj);

        std::vector<BeamLineObject*> getObjects() { return m_Objects; } // necessary?

        int appendObject(BeamLineObject& object);

    private:
        // controller instance (singleton)
        static BeamLine* m_Instance;

        std::vector<BeamLineObject*> m_Objects;
    };

}