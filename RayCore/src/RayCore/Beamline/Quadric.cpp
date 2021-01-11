#include "Quadric.h"
#include <cassert>
#include <math.h>

namespace RAY
{

    Quadric::Quadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix)
    {
        assert(inputPoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16);
        anchorPoints = inputPoints;
        inMatrix = inputInMatrix;
        outMatrix = inputOutMatrix;
    }

    /**
     * defince transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
    */
    Quadric::Quadric(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement) 
    {
        anchorPoints = inputPoints;
        double pi = 3.14159265358979323; // DEFAULT wo?
        double cos_c = cos(chi * pi/180);
        double sin_c = sin(chi * pi/180);
        double cos_a = cos(alpha * pi/180);
        double sin_a = sin(alpha * pi/180);
        double sin_b = sin(beta * pi/180);
        double cos_b = cos(beta * pi/180);
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

    Quadric::~Quadric()
    {
    }
    std::vector<double> Quadric::getQuadric()
    {
        return anchorPoints;
    }
    void Quadric::editQuadric(std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16);
        anchorPoints = inputPoints;
    }
    std::vector<double> Quadric::getAnchorPoints()
    {
        return anchorPoints;
    }
    void Quadric::setInMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        inMatrix = inputMatrix;
    }
    void Quadric::setOutMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        outMatrix = inputMatrix;
    }
    std::vector<double> Quadric::getInMatrix()
    {
        return inMatrix;
    }
    std::vector<double> Quadric::getOutMatrix()
    {
        return outMatrix;
    }
} // namespace RAY