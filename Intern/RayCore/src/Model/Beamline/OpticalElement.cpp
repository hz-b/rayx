#include "OpticalElement.h"

#include <glm.hpp>
#include <optional>

#include "Debug/Debug.h"

namespace RAYX {

OpticalElement::OpticalElement(const DesignObject& dobj) {
    // TODO(Rudi) replace try-catch stuff by std::optionals
    m_name = dobj.name();
    m_slopeError = dobj.parseSlopeError();
    m_Geometry = std::make_unique<Geometry>();
    m_material = dobj.parseMaterial();

    double widthB = 0.0;
    xml::paramDouble(dobj.node, "totalWidthB", &widthB);
    try {
        m_Geometry->m_geometricalShape = dobj.parseGeometricalShape();
    } catch (std::runtime_error& e) {
    }
    std::optional<double> lengthOrHeight;
    try {
        lengthOrHeight = dobj.parseTotalLength();
    } catch (std::runtime_error& e) {
        try {
            lengthOrHeight = dobj.parseTotalHeight();
        } catch (std::runtime_error& e) {
        }
    }
    if (lengthOrHeight) {
        m_Geometry->setHeightWidth(lengthOrHeight.value(), dobj.parseTotalWidth(), widthB);
    }
    try {
        m_Geometry->m_azimuthalAngle = dobj.parseAzimuthalAngle();
    } catch (std::runtime_error& e) {
    }

    m_Geometry->m_position = dobj.parsePosition();
    m_Geometry->m_orientation = dobj.parseOrientation();
}

Element OpticalElement::intoElement() const {
    return Element{
        .m_inTrans = getInMatrix(),
        .m_outTrans = getOutMatrix(),
        .m_elementParams = getElementParams(),
        .m_surfaceParams = getSurfaceParams(),
        .m_type = (double)getElementType(),
        .m_surfaceType = (double)m_surfacePtr->getSurfaceType(),
        .m_widthA = m_Geometry->m_widthA,
        .m_widthB = m_Geometry->m_widthB,
        .m_height = m_Geometry->m_height,
        .m_slopeError = {m_slopeError[0], m_slopeError[1], m_slopeError[2], m_slopeError[3], m_slopeError[4], m_slopeError[5], m_slopeError[6]},
        .m_azimuthalAngle = m_Geometry->m_azimuthalAngle.rad,
        .m_material = (double)static_cast<int>(m_material),
        .m_padding = {0.0, 0.0},
    };
}

// ! Workaround for a bug in the gcc/clang compiler:
// https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
OpticalElement::Geometry::Geometry() = default;
OpticalElement::Geometry::Geometry(const Geometry& other) = default;

/**
 * @brief Setting height and width based on geometrical shape
 * @param height Height of the element
 * @param widthA Width of the element
 * @param widthB Only used for trapezoid - Width of the elements edge further to the z-direction
 */
void OpticalElement::Geometry::setHeightWidth(double height, double widthA, double widthB) {
    m_widthB = widthB;
    if (m_geometricalShape == GeometricalShape::ELLIPTICAL) {
        m_widthA = -widthA;
        m_height = -height;
    } else {
        m_widthA = widthA;
        m_height = height;
    }
}

OpticalElement::OpticalElement(const char* name, const std::array<double, 7> slopeError, const Geometry& geometry)
    : m_name(name), m_Geometry(std::make_unique<Geometry>(geometry)) {
    m_slopeError = slopeError;
}

void OpticalElement::setSurface(std::unique_ptr<Surface> surface) {
    m_surfacePtr = std::move(surface);
    if (surface) {
        RAYX_ERR << "surface should be nullptr after move!";
    }
    if (!m_surfacePtr) {
        RAYX_ERR << "m_surfacePtr should NOT be nullptr!";
    }
}

/**
 * calculates element to world coordinates transformation matrix and its
 * inverse
 * @param   position     4 element vector which describes the position
 * of the element in world coordinates
 * @param   orientation  4x4 matrix that describes the orientation of
 * the surface with respect to the world coordinate system
 * @return void
 */
void OpticalElement::calcTransformationMatrices(glm::dvec4 position, glm::dmat4 orientation, glm::dmat4& output, bool calcInMatrix) const {
    glm::dmat4x4 rotation =
        glm::dmat4x4(orientation[0][0], orientation[0][1], orientation[0][2], 0.0, orientation[1][0], orientation[1][1], orientation[1][2], 0.0,
                     orientation[2][0], orientation[2][1], orientation[2][2], 0.0, 0.0, 0.0, 0.0, 1.0);  // o
    glm::dmat4x4 inv_rotation = glm::transpose(rotation);

    if (calcInMatrix) {
        glm::dmat4x4 translation = glm::dmat4x4(1, 0, 0, -position[0], 0, 1, 0, -position[1], 0, 0, 1, -position[2], 0, 0, 0, 1);  // o
        // ray = tran * rot * ray
        glm::dmat4x4 g2e = translation * rotation;
        output = glm::transpose(g2e);
        return;
    } else {
        glm::dmat4x4 inv_translation = glm::dmat4x4(1, 0, 0, position[0], 0, 1, 0, position[1], 0, 0, 1, position[2], 0, 0, 0, 1);  // o
        // inverse of m_inMatrix
        glm::dmat4x4 e2g = inv_rotation * inv_translation;
        output = glm::transpose(e2g);
        return;
    }
}

// ! temporary adjustment for trapezoid (10/11/2021)
double OpticalElement::getWidth() { return m_Geometry->m_widthA; }

double OpticalElement::getHeight() { return m_Geometry->m_height; }

glm::dmat4 OpticalElement::getInMatrix() const {
    // return glmToArray16(m_Geometry->m_inMatrix);,
    glm::dmat4 inMatrix = glm::dmat4();
    calcTransformationMatrices(m_Geometry->m_position, m_Geometry->m_orientation, inMatrix);
    return inMatrix;
}
glm::dmat4 OpticalElement::getOutMatrix() const {
    // return glmToArray16(m_Geometry->m_outMatrix);
    glm::dmat4 outMatrix = glm::dmat4();
    calcTransformationMatrices(m_Geometry->m_position, m_Geometry->m_orientation, outMatrix, false);
    return outMatrix;
}
glm::dvec4 OpticalElement::getPosition() const { return m_Geometry->m_position; }
glm::dmat4x4 OpticalElement::getOrientation() const { return m_Geometry->m_orientation; }

glm::dmat4x4 OpticalElement::getSurfaceParams() const {
    // assert(m_surfacePtr!=nullptr);
    if (m_surfacePtr != nullptr)
        return m_surfacePtr->getParams();
    else {
        RAYX_ERR << "Object without surface!";
        exit(1);
    }
}

std::array<double, 7> OpticalElement::getSlopeError() const { return m_slopeError; }

glm::dmat4x4 OpticalElement::getElementParams() const { return {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; }
}  // namespace RAYX
