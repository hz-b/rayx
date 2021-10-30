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
double degToRad(double degree) {
    return degree * PI / 180;
}

/**
 *  Calculates degree from radians.
 *
 *  @param rad Input in radians, to calculate degree from
*/
double radToDeg(double rad) {
    return rad * 180 / PI;
}

/**
 * Calculates matrix product of two homogeneous 4x4 matrices. Vectors are interpreted
 * as {1st column, 2nd column, 3rd column,  4th column} Matrices.
 *
 * @param   A   left matrix (double vector)
 * @param   B   right matrix (double vector)
 * @return  resulting matrix as 16 entry std::vector<double>
 */
std::vector<double> getMatrixProductAsVector(std::vector<double> A, std::vector<double> B) {
    std::vector<double> result(16);
    for (int i = 0; i < 16; i++) {
        int mod = int(i % 4);
        int div = int(i / 4);
        result[i] = A[mod] * B[div * 4] + A[mod + 1 * 4] * B[div * 4 + 1] + A[mod + 2 * 4] * B[div * 4 + 2] + A[mod + 3 * 4] * B[div * 4 + 3];
    }
    return result;
}

/**
 * Returns 4x4 matrix created from 16 entry vector.
 *
 * @param   V   vector
 * @return  matrix created from vector
*/
Matrix getVectorAsMatrix(std::vector<double> V)
{
    Matrix M(4, std::vector<double>(4));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            M[i][j] = V[i * 4 + j];
        }
    }
    return M;
}


// TODO(Jannis): with glm this should be unnecessary
/**
 * Calculates matrix product of two homogeneous 4x4 matrices.
 *
 * @param A  left matrix
 * @param B  right matrix
 * @return resulting matrix
 */
Matrix getMatrixProduct(Matrix A, Matrix B) {
    Matrix result(4, std::vector<double>(4));

    //std::vector<double> result (16);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                result[i][j] += A[i][k] * B[k][j];
                //result[i*4+j] += A[i][k] * B[k][j];
                // A[mod]*B[div*4] + A[mod+1*4]*B[div*4+1] + A[mod+2*4]*B[div*4+2] + A[mod+3*4]*B[div*4+3];
            }
            //std::cout << div << ", " <<mod << ", " << result[i] << std::endl;
        }
    }
    return result;
}

void printMatrix(std::vector<double> matrix) {
    std::cout << "[Matrix]: size: " << matrix.size() << std::endl;
    std::cout << "\t";
    for (int i = 0; i < int(matrix.size()); i++) {
        std::cout << matrix[i] << ", ";
        if (i % 4 == 3) {
            std::cout << std::endl;
            std::cout << "\t";}
    }
    std::cout << std::endl;
}

void printDMat4(glm::dmat4 matrix) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            std::cout << matrix[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


std::vector<double> glmToVector16(glm::dmat4x4 m) {
    std::vector<double> matrix = {m[0][0], m[0][1], m[0][2], m[0][3],
                            m[1][0], m[1][1], m[1][2], m[1][3],
                            m[2][0], m[2][1], m[2][2], m[2][3],
                            m[3][0], m[3][1], m[3][2], m[3][3]};
    return matrix;
}

glm::dmat4x4 vectorToGlm16(std::vector<double> m) {
    glm::dmat4x4 matrix = glm::dmat4x4( m[0], m[1], m[2], m[3],
                            m[4], m[5], m[6], m[7],
                            m[8], m[9], m[10], m[11],
                            m[12], m[13], m[14], m[15] );
    return matrix;
}