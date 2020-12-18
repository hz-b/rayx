#pragma once

#include "Core.h"
#include "glm.hpp"
#include "BeamlineObject.h"

#include <vector>

namespace RAY
{

    class RAY_API Beamline
    {

    public:
        Beamline();
        ~Beamline();

        //Somehow results in wrong values. Should be fixed later
        //void addBeamlineObject(BeamLineObject newObject);
        
        void addBeamlineObject(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix);
        void replaceNthObject(uint32_t index, BeamLineObject newObject);
        std::vector<BeamLineObject> getObjects();

    private:
        std::vector<BeamLineObject> m_Objects;
    };

} // namespace RAY