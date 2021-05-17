#include "Quadric.h"
#include <cassert>
#include <math.h>

namespace RAY
{
    Quadric::Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> parameters)
    {
        m_name = name;
        std::cout << inputPoints.size() << inputInMatrix.size() << inputOutMatrix.size() << misalignmentMatrix.size() << inverseMisalignmentMatrix.size() << parameters.size() << std::endl;
        assert(inputPoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16 && misalignmentMatrix.size() == 16 && inverseMisalignmentMatrix.size() == 16 && parameters.size() == 16); //parameter size ==6?
        m_anchorPoints = inputPoints;
        m_inMatrix = inputInMatrix;
        m_outMatrix = inputOutMatrix;
        m_temporaryMisalignmentMatrix = misalignmentMatrix;
        m_inverseTemporaryMisalignmentMatrix = inverseMisalignmentMatrix;
        m_parameters = parameters;
    }

    /**
     * standard constructor
     * this class calculates and stores transformation matrices (beam to element and element to beam system),
     * misalignment matrices and the parameters for the quadric equation!
     *
     * angles given in rad
     * define transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
     * @param: inputPoints      Matrix A for quadric surfaces with a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     *                      a_21,a_31,a_32,a_41,a_42,a_43 are never used for quadric surfaces because the matrix is symmetrial,
     *                      we use a_21,a_31 for x and z dimensions of the surface (xlength, zlength)
     * @param parameters                vector with 16 entries that contain further element specific parameters that are needed on the shader
     * @param alpha                     grazing incidence angle
     * @param chi                       azimuthal angle
     * @param beta                      grazing exit angle
     * @param dist                      distance to preceeding element
     * @param misalignmentParams        angles and distances for the object's misalignment
     * @param tempMisalignmentParams    parameters for temporary misalignment that can be removed midtracing.
    */
    Quadric::Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> parameters, double alpha, double chi, double beta, double dist, std::vector<double> misalignmentParams, std::vector<double> tempMisalignmentParams, Quadric* previous) 
    {   
        m_name = name;
        m_previous = previous;
        m_parameters = parameters;
        m_anchorPoints = inputPoints;
        m_misalignmentParams = misalignmentParams;
        calcTransformationMatrices(alpha, chi, beta, dist, misalignmentParams);
        setTemporaryMisalignment(tempMisalignmentParams);
        /*double cos_c = cos(chi);
        double sin_c = sin(chi);
        double cos_a = cos(alpha);
        double sin_a = sin(alpha);
        double sin_b = sin(beta);
        double cos_b = cos(beta);
        // transposes of the actual matrices since they seem to be transposed in the process of transferring to the shader
        m_inMatrix = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, dist*sin_a, -dist*cos_a, 1};
        m_outMatrix = {cos_c, sin_c, 0, 0,
                -sin_c*cos_b,cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
        m_misalignmentMatrix = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};*/
        /*inMatrix = {cos_c, sin_c, 0, 0,
            -sin_c*cos_a, cos_c*cos_a, -sin_a, distanceToPreceedingElement*sin_a,
            -sin_c*sin_a, sin_a*cos_c, cos_a, -distanceToPreceedingElement*cos_a,
            0,0,0,1};
        m_outMatrix = {cos_c, -sin_c*cos_b, sin_c*sin_b, 0,
            sin_c, cos_c*cos_b,-cos_c*sin_b, 0,
            0,sin_b, cos_b, 0,
            0, 0, 0, 1};*/
    }

    Quadric::Quadric(const char* name, Quadric* previous): m_name(name), m_previous(previous) {}

    Quadric::Quadric() {}

    Quadric::~Quadric()
    {
    }


    /**
     * calculates in and out transformation matrices from grazing incidence, exit angles, azimuthal angle and distance to preceeding element
     * angles are given in rad
     * @param alpha         grazing incidence angle
     * @param chi           azimuthal angle
     * @param beta          grazing exit angle
     * @param dist          distance to preceeding element
     * @param misalignment  misalignment x,y,z,psi,phi,chi
     * @return void
    */
    void Quadric::calcTransformationMatrices(double alpha, double chi, double beta, double dist, std::vector<double> misalignment) {
        double cos_c = cos(chi);
        double sin_c = sin(chi);
        double cos_a = cos(alpha);
        double sin_a = sin(alpha);
        double sin_b = sin(beta);
        double cos_b = cos(beta);

        // transposes of the actual matrices since they are transposed in the process of transferring to the shader
        /*std::vector<double> d_inRotation = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, 0, 0, 1};
        std::vector<double> d_inTranslation = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,-dist,1};*/
        d_b2e = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0, // M_b2e
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, dist*sin_a, -dist*cos_a, 1}; 
        
        d_inv_b2e = {cos_c, sin_c, 0, 0,            // (M_b2e)^-1
                -sin_c*cos_a, cos_c*cos_a, -sin_a, 0,
                -sin_c*sin_a, sin_a*cos_c, cos_a, 0,
                0, 0, dist, 1};

        d_e2b = {cos_c, sin_c, 0, 0, // M_e2b
                -sin_c*cos_b, cos_c*cos_b,  sin_b, 0,
                sin_c*sin_b, -cos_c*sin_b, cos_b, 0,
                0, 0, 0, 1};
        d_inv_e2b = {cos_c, -sin_c*cos_b, sin_c*sin_b, 0, // rotation -> orthogonal -> inverse = transpose
                        sin_c, cos_c*cos_b, -cos_c*sin_b, 0,
                        0, sin_b, cos_b, 0,
                        0, 0, 0, 1};
        

        double dchi = misalignment[5]; // rotation around z-axis
        double dphi = misalignment[4]; // rotation around y-axis
        double dpsi = -misalignment[3]; // rotation around x-axis (has to be negative)
        double dx = misalignment[0];
        double dy = misalignment[1];
        double dz = misalignment[2];

        m_misalignmentParams = misalignment;
        // transpose of original matrix, multiplication order: 1.chi(z) 2.phi(y) 3.psi(x), 4.dx, 5.dy, 6.dz
        // ray = tran * rot * ray
        d_misalignmentMatrix = { cos(dphi) * cos(dchi), -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), 0,
                                sin(dchi) * cos(dphi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), 0,
                                -sin(dphi), -sin(dpsi) * cos(dphi), cos(dpsi) * cos(dphi), 0,
                                -dx, -dy, -dz, 1 };
        // inverse of rotation part of misalignment matrix equals the transpose (orthogonal matrix)
        std::vector<double> inverseRotation = { cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
                            -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), -sin(dpsi) * cos(dphi), 0,
                            -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), cos(dpsi) * cos(dphi), 0,
                            0,0,0,1 };
        // inverse of translation part is negative of offsets
        std::vector<double> inverseTranslation = { 1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            dx, dy, dz, 1 };
        // inv(rot) * inv(tran) * ray
        d_inverseMisalignmentMatrix = getMatrixProductAsVector(inverseRotation, inverseTranslation);

        // add misalignment to beam<->element transformations
        d_b2e = getMatrixProductAsVector(d_misalignmentMatrix, d_b2e);
        d_inv_b2e = getMatrixProductAsVector(d_inv_b2e, d_inverseMisalignmentMatrix);  
        d_inv_e2b = getMatrixProductAsVector(d_misalignmentMatrix, d_inv_e2b);
        d_e2b = getMatrixProductAsVector(d_e2b, d_inverseMisalignmentMatrix);  
        
        
        // world coordinates = world coord of previous element * transformation from previous element to this one
        if(m_previous != NULL) { //Mi_g2e = Mi_b2e * M(i-1))_e2b * M_(i-1)_g2e
            d_g2e = getMatrixProductAsVector(m_previous->getE2B(), m_previous->getG2E());
            d_g2e = getMatrixProductAsVector(d_b2e, d_g2e);
            // Mi_e2g = M_(i-1)_e2g * M(i-1)_e2b^-1 * Mi_b2e^-1
            d_e2g = getMatrixProductAsVector(m_previous->getInvE2B(), d_inv_b2e);
            d_e2g = getMatrixProductAsVector(m_previous->getE2G(), d_e2g);
        }else{
            d_g2e = d_b2e;
            d_e2g = d_inv_b2e;
            std::cout << "first element" << std::endl;
        }
        //d_g2e = getMatrixProductAsVector(d_misalignmentMatrix, d_g2e);
        //d_e2g = getMatrixProductAsVector(d_e2g, d_inverseMisalignmentMatrix);
        
        // combine in and out transformation (global <-> element coordinates) with misalignment
        m_inMatrix = d_g2e;
        m_outMatrix = d_e2g;
        //m_inMatrix = getMatrixProductAsVector(d_misalignmentMatrix, d_g2e);
        //m_outMatrix = getMatrixProductAsVector(d_e2g, d_inverseMisalignmentMatrix);
        std::cout << "inMatrix: " << m_inMatrix.size() << std::endl;
        for(int i = 0; i<16; i++) {
            std::cout << m_inMatrix[i] << ", " ;
            if(i%4 == 3) std::cout << std::endl;
        }
        std::cout << std::endl;

    }

    /**
     * set some additional misalignment that needs to be removed in the process of tracing the ray (eg RZP and Ellipsoid),
     * therefore it is stored in a separate matrix and not in the InTrans and OutTrans that are 
     * used in the beginning and in the end of the tracing only (from Ray-coord to object-coord. and back)
     * angles given in rad 
     * we can calculate the misalignment with a matrix multiplication in the shader
     * -> store the matrix derived from the 6 input values in m_misalignmentMatrix
     * we calculate the inverse misalignment matrix as well
     *  @param: vector with 6 values: dx, dy, dz, dphi, psi, dchi
     *  @returns void
     */
    void Quadric::setTemporaryMisalignment(std::vector<double> misalignment) {
        double dx = misalignment[0];
        double dy = misalignment[1];
        double dz = misalignment[2];
        double dpsi = -misalignment[3]; // rotation around x-axis
        double dphi = misalignment[4]; // rotation around y-axis
        double dchi = misalignment[5]; // rotation around z-axis (has to be negative)

        m_temporaryMisalignmentParams = misalignment;
        // transpose of original matrix, multiplication order: chi phi psi, dx, dy, dz
        m_temporaryMisalignmentMatrix = { cos(dphi) * cos(dchi), -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), 0,
                                sin(dchi) * cos(dphi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), 0,
                                -sin(dphi), -sin(dpsi) * cos(dphi), cos(dpsi) * cos(dphi), 0,
                                -dx, -dy, -dz, 1 };
        // inverse of rotation part of misalignment matrix equals the transpose (orthogonal matrix)
        std::vector<double> inverseRotation = { cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
                            -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), -sin(dpsi) * cos(dphi), 0,
                            -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), cos(dpsi) * cos(dphi), 0,
                            0,0,0,1 };
        // inverse of translation part is negative of offsets
        std::vector<double> inverseTranslation = { 1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            dx, dy, dz, 1 };
        // inverseTranslation * inverseRotation = inverseMisalignmentMatrix 
        m_inverseTemporaryMisalignmentMatrix = getMatrixProductAsVector(inverseRotation, inverseTranslation);
    }

    std::vector<double> Quadric::getQuadric()
    {
        return m_anchorPoints;
    }

    std::vector<double> Quadric::getParams()
    {
        return m_parameters;
    }

    void Quadric::setParameters(std::vector<double> params) {
        assert(params.size() == 16);
        m_parameters = params;
    }

    /**
     * set a new set of paramters a_11 to a_44 for the quadric function
     * order: a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     * @param inputPoints   16 entry vector a_11 to a_44
     * @return void
    */
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

    std::vector<double> Quadric::getMisalignmentParams() {
        return m_misalignmentParams;
    }

    std::vector<double> Quadric::getMisalignmentMatrix()
    {
        return d_misalignmentMatrix;
    }

    std::vector<double> Quadric::getInverseMisalignmentMatrix()
    {
        return d_inverseMisalignmentMatrix;
    }

    std::vector<double> Quadric::getB2E() 
    {
        return d_b2e;
    }

    std::vector<double> Quadric::getE2B() 
    {
        return d_e2b;
    }

    std::vector<double> Quadric::getInvB2E() {
        return d_inv_b2e;
    }

    std::vector<double> Quadric::getInvE2B() {
        return d_inv_e2b;
    }

    std::vector<double> Quadric::getE2G() 
    {
        return d_e2g;
    }

    std::vector<double> Quadric::getG2E() 
    {
        return d_g2e;
    }

    std::vector<double> Quadric::getTempMisalignmentParams() {
        return m_temporaryMisalignmentParams;
    }

    std::vector<double> Quadric::getTempMisalignmentMatrix()
    {
        return m_temporaryMisalignmentMatrix;
    }

    std::vector<double> Quadric::getInverseTempMisalignmentMatrix()
    {
        return m_inverseTemporaryMisalignmentMatrix;
    }

    std::vector<double> Quadric::getParameters() {
        return m_parameters;
    }

    const char* Quadric::getName() {
        return m_name;
    }
} // namespace RAY