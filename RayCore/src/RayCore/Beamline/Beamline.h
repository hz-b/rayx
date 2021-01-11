#pragma once

#include "Core.h"
#include "glm.hpp"
#include "Quadric.h"
#include "MatrixSource.h"

#include <vector>

namespace RAY
{

    class RAY_API Beamline
    {

    public:
        Beamline();
        ~Beamline();

        //Somehow results in wrong values. Should be fixed later
        //void addQuadric(Quadric newObject);
        
        void addQuadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix);
        void replaceNthObject(uint32_t index, Quadric newObject);
        std::vector<Quadric> getObjects();

    private:
        std::vector<Quadric> m_Objects;
    };

} // namespace RAY