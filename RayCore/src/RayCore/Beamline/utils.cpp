#include "utils.h"

/**
 * calculates photon wavelength (nm) from its energy (eV) or vice verse
 * taken from RAYLIB.FOR
 * used for eg gratings
*/
double hvlam(double x) {
    if(x==0) {
        return 0.0;
    }
    return inm2eV / x;
}

/**
 * from degree to rad
*/
double rad(double degree) {
    return degree  * PI/180;
}

/**
 * from rad to degree
*/
double degree(double rad) {
    return rad *180/PI;
}

/**
 * calculates matrix product of two homogeneous 4x4 matrices
 * vectors are interpreted as {1st column, 2nd column, 3rd column,  4th column} Matrices
 * @param std::vector<double> A  left matrix
 * @param std::vector<double> B  right matrix
 * @return resulting matrix as 16 entry std::vector<double>
 */
std::vector<double> getMatrixProductAsVector(std::vector<double> A, std::vector<double> B) {
    std::vector<double> result (16);
    for(int i=0; i<16; i++) {
        int mod = int (i%4);
        int div = int(i/4);
        result[i] = A[mod]*B[div*4] + A[mod+1*4]*B[div*4+1] + A[mod+2*4]*B[div*4+2] + A[mod+3*4]*B[div*4+3];
    }
    return result;
}

/**
 * returns 16 entry vector to 4x4 matrix 
 * @param V     vector
 * @return M    vector as matrix
*/
Matrix getVectorAsMatrix(std::vector<double> V)
{
    Matrix M(4, std::vector<double>(4));
    for(int i = 0; i<4; i++) {
        for(int j = 0; j<4; j++) {
            M[i][j] = V[i*4+j];
        }
    }
    return M;
}

/**
 * calculates matrix product of two homogeneous 4x4 matrices
 * @param matrix A  left matrix
 * @param matrix B  right matrix
 * @return resulting matrix
 */
Matrix getMatrixProduct(Matrix A, Matrix B) {
    Matrix result(4, std::vector<double>(4));
    
    //std::vector<double> result (16);
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            for(int k=0; k<4; k++) {
                result[i][j] += A[i][k] * B[k][j];
                //result[i*4+j] += A[i][k] * B[k][j];
                // A[mod]*B[div*4] + A[mod+1*4]*B[div*4+1] + A[mod+2*4]*B[div*4+2] + A[mod+3*4]*B[div*4+3];
            }
            //std::cout << div << ", " <<mod << ", " << result[i] << std::endl;
        }
    }
    return result;
}