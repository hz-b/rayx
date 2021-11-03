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
    OpticalElement::OpticalElement(const char* name, const std::vector<double> surfaceParams, const std::vector<double> inputInMatrix, const std::vector<double> inputOutMatrix, const std::vector<double> OParameters, const std::vector<double> EParameters)
        : BeamlineObject(name)
    {
        std::cout << surfaceParams.size() << inputInMatrix.size() << inputOutMatrix.size() << EParameters.size() << OParameters.size() << std::endl;
        // surface.getParams() to shader/buffer
        assert(surfaceParams.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16 && EParameters.size() == 16 && OParameters.size() == 16);
        m_surfaceParams = surfaceParams;
        m_geometry = std::make_unique<Geometry>();
        m_geometry->setInMatrix(inputInMatrix);
        m_geometry->setOutMatrix(inputOutMatrix);
        m_objectParameters = OParameters;
        m_elementParameters = EParameters;
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
    OpticalElement::OpticalElement(const char* name, const std::vector<double> EParameters, const int geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_elementParameters(EParameters),
        m_slopeError(slopeError)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, position, orientation);
        assert(EParameters.size() == 16 && slopeError.size() == 7 && tempMisalignmentParams.size() == 6);
        updateObjectParams();
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
    }


    OpticalElement::OpticalElement()
    {

    }

    OpticalElement::~OpticalElement()
    {
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

    double OpticalElement::getWidth() {
        double width = m_geometry->getWidth();
        return width;
    }

    double OpticalElement::getHeight() {
        return m_geometry->getHeight();
    }

    std::vector<double> OpticalElement::getInMatrix() const
    {
        return m_geometry->getInMatrix();
    }
    std::vector<double> OpticalElement::getOutMatrix() const
    {
        return m_geometry->getOutMatrix();
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
        std::cout << "[Optic-Element]: return anchor points" << std::endl;
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