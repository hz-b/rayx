#include "BeamlineObject.h"
#include <cassert>

namespace RAY
{

    BeamLineObject::BeamLineObject(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix)
    {
        assert(inputPoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16);
        anchorPoints = inputPoints;
        inMatrix = inputInMatrix;
        outMatrix = inputOutMatrix;
    }

    BeamLineObject::BeamLineObject()
    {
    }

    BeamLineObject::~BeamLineObject()
    {
    }
    std::vector<double> BeamLineObject::getQuadric()
    {
        return anchorPoints;
    }
    void BeamLineObject::editQuadric(std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16);
        anchorPoints = inputPoints;
    }
    std::vector<double> BeamLineObject::getAnchorPoints()
    {
        return anchorPoints;
    }
    void BeamLineObject::setInMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        inMatrix = inputMatrix;
    }
    void BeamLineObject::setOutMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        outMatrix = inputMatrix;
    }
    std::vector<double> BeamLineObject::getInMatrix()
    {
        return inMatrix;
    }
    std::vector<double> BeamLineObject::getOutMatrix()
    {
        return outMatrix;
    }
} // namespace RAY