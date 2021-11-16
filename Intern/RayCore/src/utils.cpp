#include "utils.h"

/**
 * Calculates photon wavelength (nm) from its energy (eV) or
 * vice verse. For example is used for gratings.
 * @see taken from RAYLIB.FOR
 *
 */
double hvlam(double x) {
    if (x == 0) {
        return 0.0;
    }
    return inm2eV / x;
}

/**
 *  Calculates radians from degree.
 *
 *  @param degree Input in degree, to calculate radians from
 */
double degToRad(double degree) { return degree * PI / 180; }

/**
 *  Calculates degree from radians.
 *
 *  @param rad Input in radians, to calculate degree from
 */
double radToDeg(double rad) { return rad * 180 / PI; }

glm::dmat4x4 getRotationMatrix(double dpsi, double dphi, double dchi) {
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

void printMatrix(std::vector<double> matrix) {
    std::cout << "[Matrix]: size: " << matrix.size() << std::endl;
    std::cout << "\t";
    for (int i = 0; i < int(matrix.size()); i++) {
        std::cout << matrix[i] << ", ";
        if (i % 4 == 3) {
            std::cout << std::endl;
            std::cout << "\t";
        }
    }
    std::cout << std::endl;
}

void printDMat4(glm::dmat4 matrix) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::vector<double> glmToVector16(glm::dmat4x4 m) {
    std::vector<double> matrix = {
        m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]};
    return matrix;
}

glm::dmat4x4 vectorToGlm16(std::vector<double> m) {
    glm::dmat4x4 matrix =
        glm::dmat4x4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9],
                     m[10], m[11], m[12], m[13], m[14], m[15]);
    return matrix;
}

std::vector<double> glmToVector4(glm::dvec4 v) {
    std::vector<double> vector = {v[0], v[1], v[2], v[3]};
    return vector;
}

/**
 * @brief Moves Source Vector at the end of destination Vector.
 *
 * Source Vector is empty after calling this function!
 * @author
 * //stackoverflow.com/questions/17010005/how-to-use-c11-move-semantics-to-append-vector-contents-to-another-vector
 * @param srcVector Source vector to move.
 * @param destVector Destiation vector to move into.
 * @return vector<T>::iterator Iterator to the destination Vector
 */
std::vector<double>::iterator movingAppend(std::vector<double>&& srcVector,
                                           std::vector<double>& destVector) {
    typename std::vector<double>::iterator result;

    if (destVector.empty()) {
        destVector = std::move(srcVector);
        result = std::begin(destVector);
    } else {
        result =
            destVector.insert(std::end(destVector),
                              std::make_move_iterator(std::begin(srcVector)),
                              std::make_move_iterator(std::end(srcVector)));
    }

    srcVector.clear();
    srcVector.shrink_to_fit();

    return result;
}