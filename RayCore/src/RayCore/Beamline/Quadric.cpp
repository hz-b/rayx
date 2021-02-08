#include "Quadric.h"
#include <cassert>
#include <math.h>

namespace RAY
{

    Quadric::Quadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix)
    {
        assert(inputPoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16);
        m_anchorPoints = inputPoints;
        m_inMatrix = inputInMatrix;
        m_outMatrix = inputOutMatrix;
        m_misalignmentMatrix = misalignmentMatrix;
        m_inverseMisalignmentMatrix = inverseMisalignmentMatrix;
    }

    /**
     * angles given in rad
     * define transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
     * @params: 
     *          inputPoints: Matrix A for quadric surfaces with a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     *                      a_21,a_31,a_32,a_41,a_42,a_43 are never used for quadric surfaces because the matrix is symmetrial, 
     *                      we use a_21,a_31 for x and z dimensions of the surface (xlength, zlength)
    */
    Quadric::Quadric(std::vector<double> inputPoints, std::vector<double> parameters, double alpha, double chi, double beta, double distanceToPreceedingElement) 
    {   
        m_parameters = parameters;
        m_anchorPoints = inputPoints;
        double cos_c = cos(chi);
        double sin_c = sin(chi);
        double cos_a = cos(alpha);
        double sin_a = sin(alpha);
        double sin_b = sin(beta);
        double cos_b = cos(beta);
        // transposes of the actual matrices since they seem to be transposed in the process of transferring to the shader
        m_inMatrix = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, distanceToPreceedingElement*sin_a, -distanceToPreceedingElement*cos_a, 1};
        m_outMatrix = {cos_c, sin_c, 0, 0,
                -sin_c*cos_b,cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
        m_misalignmentMatrix = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        /*inMatrix = {cos_c, sin_c, 0, 0,
            -sin_c*cos_a, cos_c*cos_a, -sin_a, distanceToPreceedingElement*sin_a,
            -sin_c*sin_a, sin_a*cos_c, cos_a, -distanceToPreceedingElement*cos_a,
            0,0,0,1};
        m_outMatrix = {cos_c, -sin_c*cos_b, sin_c*sin_b, 0,
            sin_c, cos_c*cos_b,-cos_c*sin_b, 0,
            0,sin_b, cos_b, 0, 
            0, 0, 0, 1};*/
    }

    /**
     * calculates in and out transformation matrices from grazing incidence, exit angles, azimuthal angle and distance to preceeding element
     * angles are given in rad
    */
    void Quadric::calcTransformationMatrices(double alpha, double chi, double beta, double distanceToPreceedingElement) {
        double cos_c = cos(chi);
        double sin_c = sin(chi);
        double cos_a = cos(alpha);
        double sin_a = sin(alpha);
        double sin_b = sin(beta);
        double cos_b = cos(beta);
        // transposes of the actual matrices since they seem to be transposed in the process of transferring to the shader
        m_inMatrix = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, distanceToPreceedingElement*sin_a, -distanceToPreceedingElement*cos_a, 1};
        m_outMatrix = {cos_c, sin_c, 0, 0,
                -sin_c*cos_b,cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
    }

    /**
     * set misalignment of optical element: dx, dy, dz, dphi, psi, dchi
     * angles given in rad 
     *  @params: vector with 6 values: dx, dy, dz, dphi, psi, dchi
     * 
     * we can calculate the misalignment with a matrix multiplication in the shader
     * -> store the matrix derived from the 6 input values in m_misalignmentMatrix
     * we calculate the inverse misalignment matrix as well
     */
    void Quadric::setMisalignment(std::vector<double> misalignment) {
        double dx = misalignment[0];
        double dy = misalignment[1];
        double dz = misalignment[2];
        double dchi = misalignment[3]; // rotation around z-axis
        double dphi = misalignment[4]; // rotation around y-axis
        double dpsi = -misalignment[5]; // rotation around x-axis (has to be negative)
        
        m_misalignmentParams = misalignment;
        // transpose of original matrix
        m_misalignmentMatrix = {cos(dphi)*cos(dchi), -cos(dpsi)*sin(dchi)-sin(dpsi)*sin(dphi)*cos(dchi), -sin(dpsi)*sin(dchi)+cos(dpsi)*sin(dphi)*cos(dchi), 0,
                                sin(dchi)*cos(dphi), cos(dpsi)*cos(dchi)-sin(dpsi)*sin(dphi)*sin(dchi), sin(dpsi)*cos(dchi)+cos(dpsi)*sin(dphi)*sin(dchi), 0,
                                -sin(dphi), -sin(dpsi)*cos(dphi), cos(dpsi)*cos(dphi), 0,
                                -dx, -dy, -dz, 1};
        // inverse of rotation part of misalignment matrix equals the transpose (orthogonal matrix)
        Matrix inverseRotation = {{cos(dphi)*cos(dchi), sin(dchi)*cos(dphi), -sin(dphi), 0},
                            {-cos(dpsi)*sin(dchi)-sin(dpsi)*sin(dphi)*cos(dchi), cos(dpsi)*cos(dchi)-sin(dpsi)*sin(dphi)*sin(dchi), -sin(dpsi)*cos(dphi), 0},
                            {-sin(dpsi)*sin(dchi)+cos(dpsi)*sin(dphi)*cos(dchi), sin(dpsi)*cos(dchi)+cos(dpsi)*sin(dphi)*sin(dchi), cos(dpsi)*cos(dphi), 0},
                            {0,0,0,1}};
        // inverse of translation part is negative of offsets
        Matrix inverseTranslation = {{1,0,0,0}, 
                            {0,1,0,0}, 
                            {0,0,1,0}, 
                            {dx, dy, dz, 1}};
        // inverseTranslation * inverseRotation = inverseMisalignmentMatrix 
        m_inverseMisalignmentMatrix = getMatrixProduct(inverseTranslation, inverseRotation);
    }

    std::vector<double> Quadric::getMatrixProduct(Matrix A, Matrix B) {
        std::vector<double> result (16);
        std::cout << result.size() << ", "<< result[0] << std::endl;
        for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
                for(int k=0; k<4; k++) {
                    result[i*4+j] += A[i][k] * B[k][j];
                    // A[mod]*B[div*4] + A[mod+1*4]*B[div*4+1] + A[mod+2*4]*B[div*4+2] + A[mod+3*4]*B[div*4+3];
                }
                //std::cout << div << ", " <<mod << ", " << result[i] << std::endl;
            }
        }
        return result;
    }

    Quadric::Quadric() {}

    Quadric::~Quadric()
    {
    }
    std::vector<double> Quadric::getQuadric()
    {
        return m_anchorPoints;
    }
    void Quadric::setParameters(std::vector<double> params) {
        assert(params.size() == 16);
        m_parameters = params;
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

    std::vector<double> Quadric::getMisalignmentMatrix()
    {
        return m_misalignmentMatrix;
    }

    std::vector<double> Quadric::getInverseMisalignmentMatrix()
    {
        return m_inverseMisalignmentMatrix;
    }
} // namespace RAY