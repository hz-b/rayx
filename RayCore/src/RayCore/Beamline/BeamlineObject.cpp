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

    /**
     * defince transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
    */
    BeamLineObject::BeamLineObject(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement) 
    {
        anchorPoints = inputPoints;
        double pi = 2*std::acos(0.0); // not exact enough yet
        double cos_c = std::cos(chi * pi/180);
        double sin_c = std::sin(chi * pi/180);
        double cos_a = std::cos(alpha * pi/180);
        double sin_a = std::sin(alpha * pi/180);
        double sin_b = std::sin(beta * pi/180);
        double cos_b = std::cos(beta * pi/180);
        // transposes of the actual matrices since they seem to be transposed in the process of transferring to the shader
        inMatrix = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, distanceToPreceedingElement*sin_a, -distanceToPreceedingElement*cos_a, 1};
        outMatrix = {cos_c, sin_c, 0, 0,
                -sin_c*cos_b,cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
        //inMatrix = {cos_c, sin_c, 0, 0,
        //    -sin_c*cos_a, cos_c*cos_a, -sin_a, distanceToPreceedingElement*sin_a,
        //    -sin_c*sin_a, sin_a*cos_c, cos_a, -distanceToPreceedingElement*cos_a,
        //    0,0,0,1};
        //outMatrix = {cos_c, -sin_c*cos_b, sin_c*sin_b, 0,
        //    sin_c, cos_c*cos_b,-cos_c*sin_b, 0,
        //    0,sin_b, cos_b, 0, 
        //    0, 0, 0, 1};
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