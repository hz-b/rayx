#include "calculateWorldCoordinates.h"

/**
 * calculates the element to beam coordinate transformation for a given element with (user-)parameters p.alpha, p.beta, p.chi, p.dist, p.misalignment
 * this matrix is needed to calculate the position/orientation for an element based on the position/orientation of the previous element
 * @param p         Geometric user params for current element
 * 
 * @return 4x4 matrix: element to beam coordinates transformation matrix
 */
glm::dmat4x4 calcE2B(GeometricUserParams p) {
    double cos_c = cos(rad(p.chi));
    double sin_c = sin(rad(p.chi));
    double sin_b = sin(rad(p.beta));
    double cos_b = cos(rad(p.beta));

    
    glm::dmat4x4 e2b = glm::dmat4x4( cos_c, sin_c, 0, 0, // M_e2b
                -sin_c * cos_b, cos_c * cos_b,  sin_b, 0,
                sin_c * sin_b, -cos_c * sin_b, cos_b, 0,
                0, 0, 0, 1 );
    
    double dchi = p.misalignment[5]; // rotation around z-axis
    double dphi = p.misalignment[4]; // rotation around y-axis
    double dpsi = -p.misalignment[3]; // rotation around x-axis (has to be negative)
    double dx = p.misalignment[0];
    double dy = p.misalignment[1];
    double dz = p.misalignment[2];
    glm::dmat4x4 inverseRotation = glm::dmat4x4( cos(dphi) * cos(dchi), sin(dchi) * cos(dphi), -sin(dphi), 0,
                            -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), -sin(dpsi) * cos(dphi), 0,
                            -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), cos(dpsi) * cos(dphi), 0,
                            0,0,0,1 );
    glm::dmat4x4 inverseTranslation = glm::dmat4x4( 1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            dx, dy, dz, 1 );
    glm::dmat4x4 inverseMisalignmentMatrix = inverseRotation * inverseTranslation;
    
    e2b = e2b * inverseMisalignmentMatrix;
    return glm::transpose(e2b); //o
}

/**
 * calculates the rotation part of the misalignment of an optical element
 * @param misalignment          contains misalignment paramters for the optical element (dx, dy, dz, dpsi, dphi, dchi)
 * 
 * @return 4x4 homogeneous rotation matrix 
 */
glm::dmat4x4 getMisalignmentOrientation(std::vector<double> misalignment) {
    double dchi = misalignment[5]; // rotation around z-axis
    double dphi = misalignment[4]; // rotation around y-axis
    double dpsi = -misalignment[3]; // rotation around x-axis (has to be negative)
    glm::dmat4x4 misalignmentMatrix = glm::dmat4x4( 
                            cos(dphi) * cos(dchi), -cos(dpsi) * sin(dchi) - sin(dpsi) * sin(dphi) * cos(dchi), -sin(dpsi) * sin(dchi) + cos(dpsi) * sin(dphi) * cos(dchi), 0,
                            sin(dchi) * cos(dphi), cos(dpsi) * cos(dchi) - sin(dpsi) * sin(dphi) * sin(dchi), sin(dpsi) * cos(dchi) + cos(dpsi) * sin(dphi) * sin(dchi), 0,
                            -sin(dphi), -sin(dpsi) * cos(dphi), cos(dpsi) * cos(dphi), 0,
                            0, 0, 0, 1 );
    return glm::transpose(misalignmentMatrix); // o
}

/**
 * calculates the position of an optical element based on its geometric paramters and the geometric parameters of the previous element
 * @param current           geometric parameters of the current optical element
 * @param prev              geometric parameters of the previous optical element
 * @param prev_pos          world coordinate position of the previous element in homogeneous coordinates (4d vector)
 * @param prev_or           world coordiante orientation (rotation) of the previous element in homogenous coordinates (4x4 matrix)
 * 
 * @return position of current element in homogeneous world coordinates.
 */
glm::dvec4 calcPosition(GeometricUserParams current, GeometricUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or) 
{
    glm::dvec4 local_position = glm::dvec4(0.0, 0.0, current.dist, 0.0); // position of new element with respect to the previous element
    glm::dmat4x4 orientation = calcOrientation(current);

    glm::dvec4 new_offset = glm::dvec4(current.misalignment[0], current.misalignment[1], current.misalignment[2], 0);
    glm::dvec4 prev_offset = glm::dvec4(prev.misalignment[0], prev.misalignment[1], prev.misalignment[2], 0);
    glm::dmat4x4 prev_e2b = calcE2B(prev);
    
    glm::dvec4 position = prev_pos - prev_or * prev_offset; // remove misalignemnt from position of previous element
    position = position + prev_or * prev_e2b * local_position; // add the distance from previous to new element to the position of the previous element
    position = position + orientation * new_offset; // add misalignment of new element to the position      
    
    for(int i = 0; i<4; i++) {
        std::cout << position[i] << ", " << std::endl;
    }
    return position;
}

/**
 * calculates the position of an optical element that does not have a predecessor / the geometrical parameters are with respect to the origin (light source)
 * @param current           geometrical parameters with respect to the origin
 * 
 * @return position of optical element in homogeneous world coordinates
*/
glm::dvec4 calcPosition(GeometricUserParams current) 
{
    glm::dvec4 position = glm::dvec4(0.0, 0.0, current.dist, 1.0);
    glm::dmat4x4 orientation = calcOrientation(current);

    glm::dvec4 offset = glm::dvec4(current.misalignment[0], current.misalignment[1], current.misalignment[2], 0);
    position = position  + orientation * offset;

    for(int i = 0; i<4; i++) {
        std::cout << position[i] << ", " << std::endl;
    }
    return position;
}

/**
 * calculates the orientation (rotation with respect to the origin) in world coordinates
 * @param current           geometrical parameters of current optical element
 * @param prev              geometrical parameters of previous optical element (reference element)
 * @param prev_pos          world coordinate position of previous element
 * @param prev_or           world coordiante orientation of previous element.
 * 
 * @return 4x4 homogeneous orientation of the current element with respect to the origin 
 */
glm::dmat4x4 calcOrientation(GeometricUserParams current, GeometricUserParams prev, glm::dvec4 prev_pos, glm::dmat4x4 prev_or) 
{
    glm::dmat4x4 current_misalignmentOr = getMisalignmentOrientation(current.misalignment); // local rotational misalignment
    glm::dmat4x4 current_orientation = calcOrientation(current); // orientation of new element in local coordinate system
    glm::dmat4x4 prev_e2b = calcE2B(prev); // rotation of new element coordinate system with respect to previous element

    // new global orientation = previous global orientation * rotation of new element coordinate system with respect to previous element coordinate system
    // * orientation of new element in its element coordinate system (misalignment * local orientation)
    current_orientation = prev_or * prev_e2b * current_misalignmentOr * current_orientation;
    
    std::cout << "calculated orientation from previous" << std::endl;
    for(int i = 0; i<4; i++) {
        for(int j = 0; j<4; j++) {
            std::cout << current_orientation[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    return current_orientation;
}

/**
 * calculates the orientation of the optical element with respect to the origin of the world coordinate system
 * @param current               geometrical user parameters of the current optical element
 * 
 * @return 4x4 homogeneous orientation of the current element with respect to the origin 
 */
glm::dmat4x4 calcOrientation(GeometricUserParams current) 
{
    double cos_c = cos(rad(current.chi));
    double sin_c = sin(rad(current.chi));
    double cos_a = cos(rad(current.alpha));
    double sin_a = sin(rad(current.alpha));

    glm::dmat4x4 misalignmentOr = getMisalignmentOrientation(current.misalignment);
    /*glm::dmat4x4 orientation = glm::dmat4x4(cos_c, -sin_c * cos_a, -sin_c * sin_a, 0, // M_b2e
                                sin_c, cos_c * cos_a, sin_a * cos_c, 0,
                                0, -sin_a, cos_a, 0,
                                0, 0, 0, 1 );*/
    glm::dmat4x4 orientation = glm::dmat4x4(
                                cos_c,              sin_c,          0,      0, // M_b2e
                                -sin_c * cos_a,     cos_c * cos_a,  -sin_a, 0,
                                -sin_c * sin_a,     sin_a * cos_c,  cos_a,  0,
                                0,                  0,              0,      1 );
                                
    orientation = orientation * misalignmentOr;
    std::cout << "calculated orientation" << std::endl;
    for(int i = 0; i<4; i++) {
        for(int j = 0; j<4; j++) {
            std::cout << orientation[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    return orientation; // o
}