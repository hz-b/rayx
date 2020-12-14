#pragma once

#include "Core.h"
#include "glm.hpp"
#include "BeamLineObject.h"

#include <vector>

namespace RAY {

    class RAY_API Beamline {

    public:
        Beamline();
        ~Beamline();
        void addBeamlineObject(BeamLineObject newObject);
        void replaceNthObject(uint32_t index, BeamLineObject newObject);
        std::vector<BeamLineObject> getObjects();
    private:
        std::vector<BeamLineObject> m_objects;

    };


}