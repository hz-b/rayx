#include "BeamlineObject.h"
#include "Core.h"

namespace RAY {
    class RAY_API Quadric : public BeamlineObject {
        public:
            Quadric(std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix);
            Quadric(std::vector<double> inputPoints, double alpha, double chi, double beta, double distanceToPreceedingElement);
            std::vector<double> getQuadric();
            void editQuadric(std::vector<double> inputPoints);
            std::vector<double> getAnchorPoints();
            void setInMatrix(std::vector<double> inputMatrix);
            void setOutMatrix(std::vector<double> inputMatrix);
            std::vector<double> getInMatrix();
            std::vector<double> getOutMatrix();

            Quadric();
            ~Quadric();
        private:
            std::vector<double> anchorPoints;
            std::vector<double> inMatrix;
            std::vector<double> outMatrix;
    };
}