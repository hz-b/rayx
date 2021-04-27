#pragma once
#include "Core.h"
#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;
#include "utils.h"

namespace RAY {
    class RAY_API Quadric {
        public:
            Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> parameters);
            Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> parameters, double alpha, double chi, double beta, double distanceToPreceedingElement, std::vector<double> misalignment, std::vector<double> tempMisalignment);
            Quadric(const char* name);

            std::vector<double> getQuadric();
            std::vector<double> getParams();
            void setParameters(std::vector<double> params);
            void editQuadric(std::vector<double> inputPoints);
            std::vector<double> getAnchorPoints();
            void setInMatrix(std::vector<double> inputMatrix);
            void setOutMatrix(std::vector<double> inputMatrix);

            std::vector<double> getInMatrix();
            std::vector<double> getOutMatrix();
            std::vector<double> getMisalignmentMatrix();
            std::vector<double> getInverseMisalignmentMatrix();
            std::vector<double> getMisalignmentParams();
            std::vector<double> getInTrans();
            std::vector<double> getOutTrans();
            std::vector<double> getTempMisalignmentParams();
            std::vector<double> getTempMisalignmentMatrix();
            std::vector<double> getInverseTempMisalignmentMatrix();
            std::vector<double> getParameters();
            const char* getName();

            void calcTransformationMatrices(double alpha, double chi, double beta, double dist, std::vector<double> misalignment);
            void setTemporaryMisalignment(std::vector<double> misalignment);
            
            Quadric();
            ~Quadric();
        private:
            const char* m_name;
            std::vector<double> m_anchorPoints;
            // dx,dy,dz,psi(x-axis),dphi(y-axis),dchi(z-axis) angles in rad, the order is fixed. This is added to the ray together with the InTrans and OutTrans in the beginning and end of the tracing process.
            std::vector<double> m_misalignmentParams; 
            // stored only for completeness and not to transfer to the shader, these transformations are part of inMatrix/outMatrix which are transferred.
            std::vector<double> d_misalignmentMatrix;
            std::vector<double> d_inverseMisalignmentMatrix;
            std::vector<double> d_inTrans;
            std::vector<double> d_outTrans;

            // sometimes it is necessary to remove (partly (ellipsoid) or the whole (RZP)) misalignment midtracing, this is stored here
            std::vector<double> m_temporaryMisalignmentParams;
            std::vector<double> m_temporaryMisalignmentMatrix;
            std::vector<double> m_inverseTemporaryMisalignmentMatrix;

            // stores m_misalignmentMatrix and m_inTrans (Beam2Element transformation)
            std::vector<double> m_inMatrix;
            // stores m_outTrans (Element2Beam) and m_inverseMisalignmentMatrix
            std::vector<double> m_outMatrix;
            // additional element-specific parameters that are used for tracing
            std::vector<double> m_parameters;
    };
}