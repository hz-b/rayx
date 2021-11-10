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
     * @param slopeError                slope error parameters
     */
    OpticalElement::OpticalElement(const char* name, const std::vector<double> EParameters, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_slopeError(slopeError),
        m_elementParameters(EParameters)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, position, orientation);
        assert(EParameters.size() == 16 && slopeError.size() == 7);
        updateObjectParams();
    }

    // ! temporary constructor for trapezoid (10/11/2021)
    OpticalElement::OpticalElement(const char* name, Geometry::GeometricalShape geometricalShape, const double widthA, const double widthB, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_slopeError(slopeError)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, widthA, widthB, height, position, orientation);
        assert(slopeError.size() == 7);
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
    OpticalElement::OpticalElement(const char* name, Geometry::GeometricalShape geometricalShape, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : BeamlineObject(name),
        m_slopeError(slopeError)
    {
        m_geometry = std::make_unique<Geometry>(geometricalShape, width, height, position, orientation);
        assert(slopeError.size() == 7);
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

    // ! temporary adjustment for trapezoid (10/11/2021)
    void OpticalElement::updateObjectParams() {
        double widthA, widthB = 0.0;
        m_geometry->getWidth(widthA, widthB);

        m_objectParameters = {
                widthA, m_geometry->getHeight(), m_slopeError[0], m_slopeError[1],
                m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5],
                m_slopeError[6], widthB, 0, 0, 
                0, 0, 0, 0
        };
    }

    // ! temporary adjustment for trapezoid (10/11/2021)
    double OpticalElement::getWidth() {
        double width,tmp = 0.0;
        m_geometry->getWidth(width, tmp);
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