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
double rad(double degree) {
    return degree * PI / 180;
}

/**
 *  Calculates degree from radians.
 *
 *  @param rad Input in radians, to calculate degree from
*/
double degree(double rad) {
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