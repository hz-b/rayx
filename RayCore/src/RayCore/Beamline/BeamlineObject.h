#pragma once

#include "Core.h"
#include <vector>
#include <iostream>
#include <stdexcept>

namespace RAY
{

    class RAY_API BeamLineObject
    {
    public:
        BeamLineObject(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix);
        BeamLineObject(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement);
        std::vector<double> getQuadric();
        void editQuadric(std::vector<double> inputPoints);
        std::vector<double> getAnchorPoints();
        void setInMatrix(std::vector<double> inputMatrix);
        void setOutMatrix(std::vector<double> inputMatrix);
        std::vector<double> getInMatrix();
        std::vector<double> getOutMatrix();

        BeamLineObject();
        ~BeamLineObject();

    private:
        std::vector<double> anchorPoints;
        std::vector<double> inMatrix;
        std::vector<double> outMatrix;
    };

} // namespace RAY