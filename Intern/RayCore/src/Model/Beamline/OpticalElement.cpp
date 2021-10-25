#include "OpticalElement.h"
#include "Presenter/SimulationEnv.h"
#include <cassert>
#include <assert.h>
#include <math.h>

namespace RAYX
{
    /**
     * constructor for adding elements to tracer. The given vectors contain the values that will actually be moved to the shader
     * @param name                  name of the element
     * @param surfaceParams         parameters that define the surface of the element
     * @param inputInMatrix         16x16 world coordinate to element coordinate transformation matrix
     * @param inputOutMatrix        16x16 element coordinate to world coordinate transformation matrix
     * @param misalignmentMatrix    contains matrix for when the misalignment needs to be removed mid-tracing
     * @param invMisalignmentMatrix contains inverse matrix for when the misalignment needs to be removed mid-tracing
     * @param OParameters           Object parameters (width, height, slopeError..) something all elements have
     * @param EParameters           Element specific parameters, depend on which element it is
     */
    OpticalElement::OpticalElement(const char* name, const std::vector<double> surfaceParams, const std::vector<double> inputInMatrix, const std::vector<double> inputOutMatrix, const std::vector<double> misalignmentMatrix, const std::vector<double> invMisalignmentMatrix, const std::vector<double> OParameters, const std::vector<double> EParameters)
        : BeamlineObject(name)
    {
        std::cout << surfaceParams.size() << inputInMatrix.size() << inputOutMatrix.size() << misalignmentMatrix.size() << invMisalignmentMatrix.size() << EParameters.size() << OParameters.size() << std::endl;
        // surface.getParams() to shader/buffer
        assert(surfaceParams.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16 && misalignmentMatrix.size() == 16 && invMisalignmentMatrix.size() == 16 && EParameters.size() == 16 && OParameters.size() == 16);
        m_surfaceParams = surfaceParams;
        m_geometry = std::make_unique<Geometry>();
        m_geometry->setInMatrix(inputInMatrix);
        m_geometry->setOutMatrix(inputOutMatrix);
        m_temporaryMisalignmentMatrix = misalignmentMatrix;
        m_inverseTemporaryMisalignmentMatrix = invMisalignmentMatrix;
        m_objectParameters = OParameters;
        m_elementParameters = EParameters;
    }



    OpticalElement::OpticalElement(const char* name, const int geometricalShape, const double width, const double height, const double chi, const double dist, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous)
        : BeamlineObject(name),
        m_alpha(0),
        m_beta(0),
        m_chi(chi),
        m_distanceToPreceedingElement(dist),
        m_previous(previous),
        m_slopeError(slopeError)
    {
        glm::dvec4 pos = { 0,0,0,0 };
        glm::dmat4x4 orientation = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, pos, orientation);
        updateObjectParams();

    }

    /* NEW CONSTRUCTORS */

    /**
     * @param name                      name of the element
     * @param EParameters               Element specific parameters
     * @param geometricalShape          geometrical Shape of element (0 = rectangle, 1 = elliptical)
     * @param width                     x-dimension of element
     * @param height                    z-dimension of element
     * @param position                  position in world coordinates
     * @param orientation               orientation in world coordinate system
     * @param tempMisalignmentParams    remove?
     * @param slopeError                slope error parameters
     */
    OpticalElement::OpticalElement(const char* name, const std::vector<double> EParameters, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> tempMisalignmentParams, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_elementParameters(EParameters),
        m_slopeError(slopeError)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, position, orientation);
        assert(EParameters.size() == 16 && slopeError.size() == 7 && tempMisalignmentParams.size() == 6);
        updateObjectParams();
        setTemporaryMisalignment(tempMisalignmentParams);
    }

    /**
     * @param name                      name of the element
     * @param geometricalShape          geometrical Shape of element (0 = rectangle, 1 = elliptical)
     * @param width                     x-dimension of element
     * @param height                    z-dimension of element
     * @param position                  position in world coordinates
     * @param orientation               orientation in world coordinate system
     * @param slopeError                slope error parameters
     */
    OpticalElement::OpticalElement(const char* name, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_slopeError(slopeError)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, position, orientation);
        updateObjectParams();
        setTemporaryMisalignment({ 0,0,0, 0,0,0 });
    }


    OpticalElement::OpticalElement()
    {

    }

    OpticalElement::~OpticalElement()
    {
    }

    /**
     * calculates in and out transformation matrices from grazing incidence and exit angles, azimuthal angle and distance to preceeding element
     * @param   misalignment    misalignment x,y,z,psi,phi,chi
     * @param   global          flag for global coordinate calculations
     * @return void
    */
    void OpticalElement::calcTransformationMatricesFromAngles(const std::vector<double> misalignment, bool global) {
        std::cout << getName() << " alpha " << m_alpha << " beta " << m_beta << " chi " << m_chi << std::endl;
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
        m_b2e = glm::dmat4x4(cos_c, -sin_c * cos_a, -sin_c * sin_a, 0, // M_b2e
            sin_c, cos_c * cos_a, sin_a * cos_c, 0,
            0, -sin_a, cos_a, 0,
            0, m_distanceToPreceedingElement * sin_a, -m_distanceToPreceedingElement * cos_a, 1);

        m_inv_b2e = glm::dmat4x4(cos_c, sin_c, 0, 0,            // (M_b2e)^-1
            -sin_c * cos_a, cos_c * cos_a, -sin_a, 0,
            -sin_c * sin_a, sin_a * cos_c, cos_a, 0,
            0, 0, m_distanceToPreceedingElement, 1);

        m_e2b = glm::dmat4x4(cos_c, sin_c, 0, 0, // M_e2b
            -sin_c * cos_b, cos_c * cos_b, sin_b, 0,
            sin_c * sin_b, -cos_c * sin_b, cos_b, 0,
            0, 0, 0, 1);
        m_inv_e2b = glm::dmat4x4(cos_c, -sin_c * cos_b, sin_c * sin_b, 0, // rotation -> orthogonal -> inverse = transpose
            sin_c, cos_c * cos_b, -cos_c * sin_b, 0,
            0, sin_b, cos_b, 0,
            0, 0, 0, 1);


        double dchi = misalignment[5]; // rotation around z-axis
        double dphi = misalignment[4]; // rotation around y-axis
        double dpsi = -misalignment[3]; // rotation around x-axis (has to be negative)
        double dx = misalignment[0];
        double dy = misalignment[1];
        double dz = misalignment[2];

        m_misalignmentParams = misalignment;
        // transpose of original matrix, multiplication order: 1.chi(z) 2.phi(y) 3.psi(x), 4.dx, 5.dy, 6.dz
        // ray = tran * rot * ray
        m_misalignmentMatrix = glm::dmat4x4(cos(dphi) * cos(dchi), -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), 0,
            sin(dchi) * cos(dphi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), 0,
            -sin(dphi), -sin(dpsi) * cos(dphi), cos(dpsi) * cos(dphi), 0,
            -dx, -dy, -dz, 1);
        // inverse of rotation part of misalignment matrix equals the transpose (orthogonal matrix)
        glm::dmat4x4 inverseRotation = glm::dmat4x4(cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
            -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), -sin(dpsi) * cos(dphi), 0,
            -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), cos(dpsi) * cos(dphi), 0,
            0, 0, 0, 1);
        // inverse of translation part is negative of offsets
        glm::dmat4x4 inverseTranslation = glm::dmat4x4(1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            dx, dy, dz, 1);
        // inv(rot) * inv(tran) * ray
        m_inverseMisalignmentMatrix = inverseRotation * inverseTranslation;

        // add misalignment to beam<->element transformations
        m_b2e = m_misalignmentMatrix * m_b2e;
        m_inv_b2e = m_inv_b2e * m_inverseMisalignmentMatrix;
        m_inv_e2b = m_misalignmentMatrix * m_inv_e2b;
        m_e2b = m_e2b * m_inverseMisalignmentMatrix;


        // world coordinates = world coord of previous element * transformation from previous element to this one
        if (m_previous != NULL) { //Mi_g2e = M_i_b2e * M_(i-1))_e2b * M_(i-1)_g2e
            std::cout << "calc world coordinates" << std::endl;
            m_g2e = m_previous->getE2B() * m_previous->getG2E();
            printDMat4(m_b2e);
            m_g2e = m_b2e * m_g2e;
            // Mi_e2g = M_(i-1)_e2g * M_(i-1)_e2b^-1 * M_i_b2e^-1
            m_e2g = m_previous->getInvE2B() * m_inv_b2e;
            m_e2g = m_previous->getE2G() * m_e2g;
        }
        else {
            m_g2e = m_b2e;
            m_e2g = m_inv_b2e;
            std::cout << "first element" << std::endl;
        }

        if (global) {  // combine in and out transformation (global <-> element coordinates) with misalignment
            std::cout << "global" << std::endl;
            m_inMatrix = glmToVector16(m_g2e);
            m_outMatrix = glmToVector16(m_e2g);
        }
        else {  // to use usual ray coordinatesystem, also contains misalignment
            std::cout << "RAY-UI beam coordinates" << std::endl;
            m_inMatrix = glmToVector16(m_b2e);
            m_outMatrix = glmToVector16(m_e2b);
        }

        m_geometry->setInMatrix(m_inMatrix);
        m_geometry->setOutMatrix(m_outMatrix);
        std::cout.precision(17);
        printMatrix(m_inMatrix);
        printMatrix(m_outMatrix);

    }

    /**
     * Set some additional misalignment that needs to be removed in the process of tracing the ray (eg RZP and Ellipsoid).
     * It is stored in a separate matrix and not in the InTrans and OutTrans, which are used only at the beginning
     * and end of the tracing (from ray-coord to object-coord and back).
     * Angles given in radians.
     * We can calculate the misalignment with a matrix multiplication in the shader
     * -> store the matrix derived from the 6 input values in m_misalignmentMatrix
     * We calculate the inverse misalignment matrix as well.
     *
     *  @param  misalignment    vector with 6 values: dx, dy, dz, dphi, psi, dchi
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
        glm::dmat4x4 inverseRotation = { cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
                            -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), -sin(dpsi) * cos(dphi), 0,
                            -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), cos(dpsi) * cos(dphi), 0,
                            0,0,0,1 };
        // inverse of translation part is negative of offsets
        glm::dmat4x4 inverseTranslation = { 1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            dx, dy, dz, 1 };
        // inverseTranslation * inverseRotation = inverseMisalignmentMatrix 
        m_inverseTemporaryMisalignmentMatrix = glmToVector16(inverseRotation * inverseTranslation);
    }

    void OpticalElement::setElementParameters(std::vector<double> params) {
        assert(params.size() == 16);
        m_elementParameters = params;
    }

    void OpticalElement::setInMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_geometry->setInMatrix(inputMatrix);
    }
    void OpticalElement::setOutMatrix(std::vector<double> inputMatrix)
    {
        assert(inputMatrix.size() == 16);
        m_geometry->setOutMatrix(inputMatrix);
    }

    void OpticalElement::setSurface(std::unique_ptr<Surface> surface) {
        m_surfacePtr = std::move(surface);
        assert(surface == nullptr);
        assert(m_surfacePtr != nullptr);

    }

    void OpticalElement::updateObjectParams() {
        m_objectParameters = {
                m_geometry->getWidth(), m_geometry->getHeight(), m_slopeError[0], m_slopeError[1],
                m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5],
                m_slopeError[6], 0,0,0,
                0,0,0,0
        };
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

    double OpticalElement::getWidth() {
        double width = m_geometry->getWidth();
        return width;
    }

    double OpticalElement::getHeight() {
        return m_geometry->getHeight();
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
        return m_geometry->getInMatrix();
    }
    std::vector<double> OpticalElement::getOutMatrix() const
    {
        return m_geometry->getOutMatrix();
    }

    std::vector<double> OpticalElement::getMisalignmentParams() const
    {
        return m_misalignmentParams;
    }

    glm::dmat4x4 OpticalElement::getMisalignmentMatrix() const
    {
        return m_misalignmentMatrix;
    }

    glm::dmat4x4 OpticalElement::getInverseMisalignmentMatrix() const
    {
        return m_inverseMisalignmentMatrix;
    }

    glm::dmat4x4 OpticalElement::getB2E() const
    {
        return m_b2e;
    }

    glm::dmat4x4 OpticalElement::getE2B() const
    {
        return m_e2b;
    }

    glm::dmat4x4 OpticalElement::getInvB2E() const
    {
        return m_inv_b2e;
    }

    glm::dmat4x4 OpticalElement::getInvE2B() const
    {
        return m_inv_e2b;
    }

    glm::dmat4x4 OpticalElement::getE2G() const
    {
        return m_e2g;
    }

    glm::dmat4x4 OpticalElement::getG2E() const
    {
        return m_g2e;
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
        //assert(m_surfacePtr!=nullptr);
        if (m_surfacePtr != nullptr)
            return m_surfacePtr->getParams();
        else
            return m_surfaceParams;
    }

    std::vector<double> OpticalElement::getSlopeError() const
    {
        return m_slopeError;
    }

} // namespace RAYX