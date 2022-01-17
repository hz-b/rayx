#include "WorldUserParams.h"

#include <sstream>

#include "Debug.h"

namespace RAYX {
/**
 * constructor for geometric user parameters of an object. In this class the
 * world position and orientation is calculated based on the incidence, exit and
 * azimuthal angles and the distance and misalignment of the element (beam
 * coordinates)
 *
 * angles are given in rad
 * @param alpha         grazing incidence angle in rad (grazing = measured from
 * the axis not from the normal)
 * @param beta          grazing exit angle in rad
 * @param chi           azimuthal angle in rad
 * @param dist          distance to previous element
 * @param misalignment  misalignment of the element
 * @param tangentAngle  = 0 default. only used for ellipsoid mirror because the
 * misalignment coordinate system can differ from the element coordinate system
 * by a rotation through this angle
 */
WorldUserParams::WorldUserParams(double alpha, double beta, double chi,
                                 double dist, std::vector<double> misalignment,
                                 double tangentAngle)
    : m_incidenceAngle(alpha),
      m_exitAngle(beta),
      m_azimuthalAngle(chi),
      m_dist(dist),
      m_misalignment(misalignment),
      m_tangentAngle(tangentAngle) {}

WorldUserParams::WorldUserParams() {}
WorldUserParams::~WorldUserParams() {}

/**
 * calculates the element to beam coordinate transformation for a given element
 * with (user-)parameters p.alpha, p.beta, p.chi, p.dist, p.misalignment this
 * matrix is needed to calculate the position/orientation for an element based
 * on the position/orientation of the previous element
 * @param p         Geometric user params for current element
 *
 * @return 4x4 matrix: element to beam coordinates transformation matrix
 */
glm::dmat4x4 WorldUserParams::calcE2B() {
    double cos_c = cos(m_azimuthalAngle);
    double sin_c = sin(m_azimuthalAngle);
    double sin_b = sin(m_exitAngle);
    double cos_b = cos(m_exitAngle);

    glm::dmat4x4 e2b =
        glm::dmat4x4(cos_c, sin_c, 0, 0,  // M_e2b
                     -sin_c * cos_b, cos_c * cos_b, sin_b, 0, sin_c * sin_b,
                     -cos_c * sin_b, cos_b, 0, 0, 0, 0, 1);

    double dchi = m_misalignment[5];  // rotation around z-axis
    double dphi = m_misalignment[4];  // rotation around y-axis
    double dpsi =
        -m_misalignment[3];  // rotation around x-axis (has to be negative)
    double dx = m_misalignment[0];
    double dy = m_misalignment[1];
    double dz = m_misalignment[2];
    glm::dmat4x4 inverseRotation = glm::dmat4x4(
        cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
        -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi),
        cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi),
        -sin(dpsi) * cos(dphi), 0,
        -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi),
        sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi),
        cos(dpsi) * cos(dphi), 0, 0, 0, 0, 1);
    glm::dmat4x4 inverseTranslation =
        glm::dmat4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, dx, dy, dz, 1);

    glm::dmat4x4 tangentAngleRotation = getTangentAngleRotation();
    glm::dmat4x4 inverseTangentAngleRotation =
        glm::transpose(tangentAngleRotation);

    glm::dmat4x4 inverseMisalignmentMatrix =
        inverseRotation * inverseTranslation;

    e2b = e2b * tangentAngleRotation * inverseMisalignmentMatrix *
          inverseTangentAngleRotation;
    return glm::transpose(e2b);
}

/**
 * calculates the rotation part of the misalignment of an optical element
 * @param misalignment          contains misalignment paramters for the optical
 * element (dx, dy, dz, dpsi, dphi, dchi)
 *
 * @return 4x4 homogeneous rotation matrix
 */
glm::dmat4x4 WorldUserParams::getMisalignmentOrientation() {
    double dchi = m_misalignment[5];  // rotation around z-axis
    double dphi = m_misalignment[4];  // rotation around y-axis
    double dpsi =
        -m_misalignment[3];  // rotation around x-axis (has to be negative)
    glm::dmat4x4 misalignmentMatrix =
        glm::dmat4x4(cos(dphi) * cos(dchi),
                     -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi),
                     -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi),
                     0, sin(dchi) * cos(dphi),
                     cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi),
                     sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi),
                     0, -sin(dphi), -sin(dpsi) * cos(dphi),
                     cos(dpsi) * cos(dphi), 0, 0, 0, 0, 1);
    return glm::transpose(misalignmentMatrix);
}

/**
 * @return azimuthalAngle: chi + dchi in rad
 * = orientation of element in xy-plane + rotational misalignment in xy-plane
 */
double WorldUserParams::getAzimuthalAngle() {
    return m_azimuthalAngle + m_misalignment[5];
}

/**
 * @return the rotation matrix around the x-axis by tangentAngle
 */
glm::dmat4x4 WorldUserParams::getTangentAngleRotation() {
    return glm::dmat4x4(1, 0, 0, 0, 0, cos(m_tangentAngle),
                        -sin(m_tangentAngle), 0,  // rotation around x-axis
                        0, sin(m_tangentAngle), cos(m_tangentAngle), 0, 0, 0, 0,
                        1);
}
/**
 * calculates the orientation (rotation with respect to the origin) in world
 * coordinates
 * @param prev              geometrical parameters of previous optical element
 * (reference element)
 * @param prev_pos          world coordinate position of previous element
 * @param prev_or           world coordiante orientation of previous element.
 *
 * @return 4x4 homogeneous orientation of the current element with respect to
 * the origin
 */
glm::dmat4x4 WorldUserParams::calcOrientation(WorldUserParams prev,
                                              glm::dmat4x4 prev_or) {
    glm::dmat4x4 current_orientation =
        calcOrientation();  // orientation of new element in local coordinate
                            // system
    glm::dmat4x4 prev_e2b =
        prev.calcE2B();  // rotation of new element coordinate system with
                         // respect to previous element

    // new global orientation = previous global orientation * rotation of new
    // element coordinate system with respect to previous element coordinate
    // system
    // * orientation of new element in its element coordinate system
    // (misalignment * local orientation)
    current_orientation = prev_or * prev_e2b * current_orientation;

    /*RAYX_LOG << "Calculated orientation from previous";
    for (int i = 0; i < 4; i++) {
        std::stringstream s;
        s << "\t";
        for (int j = 0; j < 4; j++) {
            s << current_orientation[i][j] << ", ";
        }
        RAYX_LOG << s.str();
    }*/
    return current_orientation;
}

/**
 * calculates the orientation of the optical element with respect to the origin
 * of the world coordinate system
 *
 * @return 4x4 homogeneous orientation of the current element with respect to
 * the origin
 */
glm::dmat4x4 WorldUserParams::calcOrientation() {
    double cos_c = cos(m_azimuthalAngle);
    double sin_c = sin(m_azimuthalAngle);
    double cos_a = cos(m_incidenceAngle);
    double sin_a = sin(m_incidenceAngle);

    glm::dmat4x4 tangentAngleRotation = getTangentAngleRotation();
    glm::dmat4x4 inverseTangentAngleRotation =
        glm::transpose(tangentAngleRotation);

    glm::dmat4x4 misalignmentOr = getMisalignmentOrientation();
    glm::dmat4x4 orientation =
        glm::dmat4x4(cos_c, sin_c, 0, 0, -sin_c * cos_a, cos_c * cos_a, -sin_a,
                     0, -sin_c * sin_a, sin_a * cos_c, cos_a, 0, 0, 0, 0, 1);

    orientation = orientation * tangentAngleRotation * misalignmentOr *
                  inverseTangentAngleRotation;
    /*RAYX_LOG << "Calculated orientation";
    for (int i = 0; i < 4; i++) {
        std::stringstream s;
        s << '\t';
        for (int j = 0; j < 4; j++) {
            s << orientation[i][j] << ", ";
        }
        RAYX_LOG << s.str();
    }*/
    return orientation;
}

/**
 * calculates the position of an optical element based on its geometric
 * paramters and the geometric parameters of the previous element
 * @param prev              geometric parameters of the previous optical element
 * @param prev_pos          world coordinate position of the previous element in
 * homogeneous coordinates (4d vector)
 * @param prev_or           world coordiante orientation (rotation) of the
 * previous element in homogenous coordinates (4x4 matrix)
 *
 * @return position of current element in homogeneous world coordinates.
 */
glm::dvec4 WorldUserParams::calcPosition(WorldUserParams prev,
                                         glm::dvec4 prev_pos,
                                         glm::dmat4x4 prev_or) {
    glm::dvec4 local_position = glm::dvec4(
        0.0, 0.0, m_dist,
        0.0);  // position of new element with respect to the previous element
    glm::dmat4x4 orientation = calcOrientation(prev, prev_or);

    glm::dvec4 new_offset =
        glm::dvec4(m_misalignment[0], m_misalignment[1], m_misalignment[2], 0);
    glm::dvec4 prev_offset =
        glm::dvec4(prev.getMisalignment()[0], prev.getMisalignment()[1],
                   prev.getMisalignment()[2], 0);
    glm::dmat4x4 prev_e2b = prev.calcE2B();

    glm::dvec4 position =
        prev_pos - prev_or * prev.getTangentAngleRotation() *
                       prev_offset;  // remove misalignment (possibly in
                                     // different coord system -> tangentAngle)
                                     // from position of previous element
    // RAYX_LOG << "previous position = " << position[0] << ", " << position[1]
    //          << ", " << position[2] << ", " << position[3];
    position =
        position +
        prev_or * prev_e2b *
            local_position;  // add the distance from previous to new element to
                             // the position of the previous element
    position =
        position +
        orientation * getTangentAngleRotation() *
            new_offset;  // add misalignment (possibly in different coord system
                         // -> tangentAngle) of new element to the position

    /*std::stringstream s;
    for (int i = 0; i < 4; i++) {
        s << position[i] << ", ";
    }
    RAYX_LOG << s.str();*/
    return position;
}

/**
 * calculates the position of an optical element that does not have a
 * predecessor / the geometrical parameters are with respect to the origin
 * (light source)
 *
 * @return position of optical element in homogeneous world coordinates
 */
glm::dvec4 WorldUserParams::calcPosition() {
    glm::dvec4 position = glm::dvec4(0.0, 0.0, m_dist, 0.0);
    glm::dmat4x4 orientation = calcOrientation();
    glm::dmat4x4 tangentAngleRotation = getTangentAngleRotation();

    glm::dvec4 offset = glm::dvec4(m_misalignment[0], m_misalignment[1],
                                   m_misalignment[2], 1.0);

    // add misalignment (possibly in different coord system -> tangentAngle) of
    // new element to the position
    position = position + orientation * tangentAngleRotation * offset;
    /*std::stringstream s;
    s << "Position: ";
    for (int i = 0; i < 4; i++) {
        s << position[i] << ", ";
    }
    RAYX_LOG << s.str();*/
    return position;
}

std::vector<double> WorldUserParams::getMisalignment() {
    return m_misalignment;
}

}  // namespace RAYX