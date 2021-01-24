#include "Core.h"
#include <vector>
#include <iostream>
#include <stdexcept>
#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;
typedef std::vector<std::vector<double>> Matrix;

const double PI = 3.14159265358979323; 

namespace RAY {
    class RAY_API Quadric {
        public:
            Quadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix);
            Quadric(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement);
            
            std::vector<double> getQuadric();
            void editQuadric(std::vector<double> inputPoints);
            std::vector<double> getAnchorPoints();
            void setInMatrix(std::vector<double> inputMatrix);
            void setOutMatrix(std::vector<double> inputMatrix);
            std::vector<double> getInMatrix();
            std::vector<double> getOutMatrix();
            std::vector<double> getMisalignmentMatrix();
            void setMisalignment(std::vector<double> misalignment);
            std::vector<double> getMatrixProduct(Matrix A, Matrix B);
            std::vector<double> getInverseMisalignmentMatrix();
    
            Quadric();
            ~Quadric();
        private:
            std::vector<double> m_anchorPoints;
            std::vector<double> m_misalignmentParams;
            std::vector<double> m_misalignmentMatrix;
            std::vector<double> m_inverseMisalignmentMatrix;
            std::vector<double> m_inMatrix;
            std::vector<double> m_outMatrix;
    };
}