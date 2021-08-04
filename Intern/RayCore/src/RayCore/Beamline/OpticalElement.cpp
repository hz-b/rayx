#include "OpticalElement.h"
#include <cassert>
#include <assert.h>
#include <math.h>

namespace RAYX
{
    OpticalElement::OpticalElement(const char* name, const std::vector<double> surfacePoints, const std::vector<double> inputInMatrix, const std::vector<double> inputOutMatrix, const std::vector<double> misalignmentMatrix, const std::vector<double> inverseMisalignmentMatrix, const std::vector<double> OParameters, const std::vector<double> EParameters)
        : BeamlineObject(name)
    {
        std::cout << surfacePoints.size() << inputInMatrix.size() << inputOutMatrix.size() << misalignmentMatrix.size() << inverseMisalignmentMatrix.size() << EParameters.size() << OParameters.size() << std::endl;
        // surface.getParams() to shader/buffer
        assert(surfacePoints.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16 && misalignmentMatrix.size() == 16 && inverseMisalignmentMatrix.size() == 16 && EParameters.size() == 16 && OParameters.size() == 16);
        m_surfaceParams = surfacePoints;
        m_inMatrix = inputInMatrix;
        m_outMatrix = inputOutMatrix;
        m_temporaryMisalignmentMatrix = misalignmentMatrix;
        m_inverseTemporaryMisalignmentMatrix = inverseMisalignmentMatrix;
        m_objectParameters = OParameters;
        m_elementParameters = EParameters;
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
     * @param EParameters               vector with 16 entries that contain further element specific parameters that are needed on the shader
     * @param alpha                     grazing incidence angle
     * @param chi                       azimuthal angle
     * @param beta                      grazing exit angle
     * @param dist                      distance to preceeding element
     * @param misalignmentParams        angles and distances for the object's misalignment
     * @param tempMisalignmentParams    parameters for temporary misalignment that can be removed midtracing.
    */
    OpticalElement::OpticalElement(const char* name, const std::vector<double> EParameters, const double width, const double height, const double alpha, const double chi, const double beta, const double dist, const std::vector<double> misalignmentParams, const std::vector<double> tempMisalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : BeamlineObject(name),
        //m_surface(std::move(surface)), 
        m_width(width),
        m_height(height),
        m_alpha (alpha),
        m_beta(beta), // mirror -> exit angle = incidence angle
        m_chi (chi),
        m_distanceToPreceedingElement (dist),
        m_previous(previous),
        m_misalignmentParams(misalignmentParams),
        m_slopeError(slopeError),
        m_elementParameters(EParameters)
    {
        m_objectParameters = {
                m_width, m_height, m_slopeError[0], m_slopeError[1],
                m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5],
                m_slopeError[6],0,0,0,
                0,0,0,0
            };
        calcTransformationMatrices(misalignmentParams, global);
        setTemporaryMisalignment(tempMisalignmentParams);
    }

    OpticalElement::OpticalElement(const char* name, const double width, const double height, const double chi, const double dist, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous)
        : BeamlineObject(name),
        m_width(width),
        m_height(height),
        m_chi(chi),
        m_distanceToPreceedingElement(dist),
        m_previous(previous),
        m_slopeError(slopeError) 
    {
        m_objectParameters = {
                m_width, m_height, m_slopeError[0], m_slopeError[1],
                m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5],
                m_slopeError[6],0,0,0,
                0,0,0,0
            };
    }

    OpticalElement::OpticalElement(const char* name, const double chi, const double dist, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous)
        : BeamlineObject(name), m_chi(chi), m_distanceToPreceedingElement(dist), m_previous(previous), m_slopeError(slopeError) {}


    OpticalElement::OpticalElement() {}

    OpticalElement::~OpticalElement()
    {
    }


    /**
     * calculates in and out transformation matrices from grazing incidence and exit angles, azimuthal angle and distance to preceeding element
     * @param misalignment  misalignment x,y,z,psi,phi,chi
     * @return void
    */
    void OpticalElement::calcTransformationMatrices(const std::vector<double> misalignment, bool global) {
        
        double cos_c = cos(m_chi);
        double sin_c = sin(m_chi);
        double cos_a = cos(m_alpha);
        double sin_a = sin(m_alpha);
        double sin_b = sin(m_beta);
        double cos_b = cos(m_beta);

        // transposes of the actual matrices since they are transposed in the process of transferring to the shader
        /*std::vector<double> d_inRotation = {cos_c, -sin_c*cos_a, -sin_c*sin_a, 0,
                sin_c, cos_c*cos_a, sin_a*cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, 0, 0, 1};
        std::vector<double> d_inTranslation = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,-dist,1};*/
        d_b2e = { cos_c, -sin_c * cos_a, -sin_c * sin_a, 0, // M_b2e
                sin_c, cos_c * cos_a, sin_a * cos_c, 0,
                0, -sin_a, cos_a, 0,
                0, m_distanceToPreceedingElement * sin_a, -m_distanceToPreceedingElement * cos_a, 1 };

        d_inv_b2e = { cos_c, sin_c, 0, 0,            // (M_b2e)^-1
                -sin_c * cos_a, cos_c * cos_a, -sin_a, 0,
                -sin_c * sin_a, sin_a * cos_c, cos_a, 0,
                0, 0, m_distanceToPreceedingElement, 1 };

        d_e2b = { cos_c, sin_c, 0, 0, // M_e2b
                -sin_c * cos_b, cos_c * cos_b,  sin_b, 0,
                sin_c * sin_b, -cos_c * sin_b, cos_b, 0,
                0, 0, 0, 1 };
        d_inv_e2b = { cos_c, -sin_c * cos_b, sin_c * sin_b, 0, // rotation -> orthogonal -> inverse = transpose
                        sin_c, cos_c * cos_b, -cos_c * sin_b, 0,
                        0, sin_b, cos_b, 0,
                        0, 0, 0, 1 };


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
        if (m_previous != NULL) { //Mi_g2e = M_i_b2e * M_(i-1))_e2b * M_(i-1)_g2e
            std::cout << "calc world coordinates" << std::endl;
            d_g2e = getMatrixProductAsVector(m_previous->getE2B(), m_previous->getG2E());
            d_g2e = getMatrixProductAsVector(d_b2e, d_g2e);
            // Mi_e2g = M_(i-1)_e2g * M_(i-1)_e2b^-1 * M_i_b2e^-1
            d_e2g = getMatrixProductAsVector(m_previous->getInvE2B(), d_inv_b2e);
            d_e2g = getMatrixProductAsVector(m_previous->getE2G(), d_e2g);
        }
        else {
            d_g2e = d_b2e;
            d_e2g = d_inv_b2e;
            std::cout << "first element" << std::endl;
        }

        if(global) {  // combine in and out transformation (global <-> element coordinates) with misalignment
            std::cout << "global" << std::endl;
            m_inMatrix = d_g2e;
            m_outMatrix = d_e2g;
        }else{  // to use usual ray coordinatesystem, also contains misalignment
            std::cout << "RAY-UI beam coordinates" << std::endl;
            m_inMatrix = d_b2e;
            m_outMatrix = d_e2b;
        }

        std::cout << "inMatrix: " << m_inMatrix.size() << std::endl;
        for (int i = 0; i < 16; i++) {
            std::cout << m_inMatrix[i] << ", ";
            if (i % 4 == 3) std::cout << std::endl;
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
    void OpticalElement::setTemporaryMisalignment(std::vector<double> misalignment) {
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

    void OpticalElement::setElementParameters(std::vector<double> params) {
        assert(params.size() == 16);
        m_elementParameters = params;
    }

    void OpticalElement::setDimensions(double width, double height) {
        m_width = width;
        m_height = height;
        m_objectParameters = {
                m_width, m_height, m_slopeError[0], m_slopeError[1],
                m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5],
                m_slopeError[6],0,0,0,
                0,0,0,0
            };
    }

    void OpticalElement::setInMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_inMatrix = inputMatrix;
    }
    void OpticalElement::setOutMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_outMatrix = inputMatrix;
    }

    void OpticalElement::setSurface(std::unique_ptr<Surface> surface) {
        m_surface = std::move(surface);
        assert(surface == nullptr);
        assert(m_surface != nullptr);

    }

    void OpticalElement::setAlpha(double alpha) {
        m_alpha = alpha;
    }
    
    void OpticalElement::setBeta(double beta) {
        m_beta = beta;
    }

    double OpticalElement::getBeta() const {
        return m_beta;
    }

    double OpticalElement::getWidth() const {
        return abs(m_width);
    }

    double OpticalElement::getHeight() const {
        return abs(m_height);
    }

    double OpticalElement::getAlpha() const {
        return m_alpha;
    }

    double OpticalElement::getChi() const {
        return m_chi;
    }
    double OpticalElement::getDistanceToPreceedingElement() const {
        return m_distanceToPreceedingElement;
    }

    std::vector<double> OpticalElement::getInMatrix() const
    {
        return m_inMatrix;
    }
    std::vector<double> OpticalElement::getOutMatrix() const
    {
        return m_outMatrix;
    }

    std::vector<double> OpticalElement::getMisalignmentParams() const
    {
        return m_misalignmentParams;
    }

    std::vector<double> OpticalElement::getMisalignmentMatrix() const
    {
        return d_misalignmentMatrix;
    }

    std::vector<double> OpticalElement::getInverseMisalignmentMatrix() const
    {
        return d_inverseMisalignmentMatrix;
    }

    std::vector<double> OpticalElement::getB2E() const
    {
        return d_b2e;
    }

    std::vector<double> OpticalElement::getE2B() const
    {
        return d_e2b;
    }

    std::vector<double> OpticalElement::getInvB2E() const
    {
        return d_inv_b2e;
    }

    std::vector<double> OpticalElement::getInvE2B() const
    {
        return d_inv_e2b;
    }

    std::vector<double> OpticalElement::getE2G() const
    {
        return d_e2g;
    }

    std::vector<double> OpticalElement::getG2E() const
    {
        return d_g2e;
    }

    std::vector<double> OpticalElement::getTempMisalignmentParams() const
    {
        return m_temporaryMisalignmentParams;
    }

    std::vector<double> OpticalElement::getTempMisalignmentMatrix() const
    {
        return m_temporaryMisalignmentMatrix;
    }

    std::vector<double> OpticalElement::getInverseTempMisalignmentMatrix() const
    {
        return m_inverseTemporaryMisalignmentMatrix;
    }

    std::vector<double> OpticalElement::getObjectParameters()
    {
        return m_objectParameters;
    }

    std::vector<double> OpticalElement::getElementParameters() const
    {
        return m_elementParameters;
    }

    std::vector<double> OpticalElement::getSurfaceParams() const
    {
        std::cout << "return anchor points" << std::endl;
        //assert(m_surface!=nullptr);
        if (m_surface != nullptr)
            return m_surface->getParams();
        else
            return m_surfaceParams;
    }

    std::vector<double> OpticalElement::getSlopeError() const
    {
        return m_slopeError;
    }

} // namespace RAYX