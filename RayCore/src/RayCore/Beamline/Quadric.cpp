#include "Quadric.h"
#include <cassert>
#include <math.h>

namespace RAY
{

    Quadric::Quadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix)
    {
        assert(inputPoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16);
        m_anchorPoints = inputPoints;
        m_inMatrix = inputInMatrix;
        m_outMatrix = inputOutMatrix;
    }

    /**
     * angles given in degree
     * define transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
    */
    Quadric::Quadric(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement) 
    {
        m_anchorPoints = inputPoints;
        double pi = 3.14159265358979323; // DEFAULT wo?
        double cos_c = cos(chi * pi/180);
        double sin_c = sin(chi * pi/180);
        double cos_a = cos(alpha * pi/180);
        double sin_a = sin(alpha * pi/180);
        double sin_b = sin(beta * pi/180);
        double cos_b = cos(beta * pi/180);
        // transposes of the actual matrices since they seem to be transposed in the process of transferring to the shader
        m_inMatrix = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, distanceToPreceedingElement*sin_a, -distanceToPreceedingElement*cos_a, 1};
        m_outMatrix = {cos_c, sin_c, 0, 0,
                -sin_c*cos_b,cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
        /*m_inMatrix = {cos_c, sin_c, 0, 0,
            -sin_c*cos_a, cos_c*cos_a, -sin_a, distanceToPreceedingElement*sin_a,
            -sin_c*sin_a, sin_a*cos_c, cos_a, -distanceToPreceedingElement*cos_a,
            0,0,0,1};
        m_outMatrix = {cos_c, -sin_c*cos_b, sin_c*sin_b, 0,
            sin_c, cos_c*cos_b,-cos_c*sin_b, 0,
            0,sin_b, cos_b, 0, 
            0, 0, 0, 1};*/
    }

    /**
     * set misalignment of optical element: dx, dy, dz, dphi, psi, dchi
     * angles given in rad 
     *  @params: vector with 6 values
     * 
     * we can calculate the misalignment with a matrix multiplication in the shader
     * -> store the matrix derived from the 6 input values in m_misalignmentMatrix
     */
    void Quadric::setMisalignment(std::vector<double> misalignment) {
        double dx = misalignment[0];
        double dy = misalignment[1];
        double dz = misalignment[2];
        double dchi = misalignment[3]; // rotation around z-axis
        double dphi = misalignment[4]; // rotation around y-axis
        double dpsi = -misalignment[5]; // rotation around x-axis (has to be negative)
        
        m_misalignmentParams = misalignment;
        // transpose
        m_misalignmentMatrix = {cos(dphi)*cos(dchi), -cos(dpsi)*sin(dchi)-sin(dpsi)*sin(dphi)*cos(dchi), -sin(dpsi)*sin(dchi)+cos(dpsi)*sin(dphi)*sin(dchi), 0,
                                sin(dchi)*cos(dphi), cos(dpsi)*cos(dchi)-sin(dpsi)*sin(dphi)*sin(dchi), sin(dpsi)*cos(dchi)+cos(dpsi)*sin(dphi)*sin(dchi), 0,
                                -sin(dphi), -sin(dpsi)*cos(dphi), cos(dpsi)*cos(dphi), 0,
                                -dx, -dy, -dz, 1};
    }

    Quadric::~Quadric()
    {
    }
    std::vector<double> Quadric::getQuadric()
    {
        return m_anchorPoints;
    }
    void Quadric::editQuadric(std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16);
        m_anchorPoints = inputPoints;
    }
    std::vector<double> Quadric::getAnchorPoints()
    {
        return m_anchorPoints;
    }
    void Quadric::setInMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_inMatrix = inputMatrix;
    }
    void Quadric::setOutMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_outMatrix = inputMatrix;
    }
    std::vector<double> Quadric::getInMatrix()
    {
        return m_inMatrix;
    }
    std::vector<double> Quadric::getOutMatrix()
    {
        return m_outMatrix;
    }
} // namespace RAY