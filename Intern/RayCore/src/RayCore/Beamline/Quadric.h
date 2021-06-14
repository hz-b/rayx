#pragma once
#include "Core.h"
#define RAYCORE_QUADRIC_DOUBLE_AMOUNT 48;
#include "utils.h"

namespace RAY {
    class RAY_API Quadric {
        public:
            Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix, std::vector<double> misalignmentMatrix, std::vector<double> inverseMisalignmentMatrix, std::vector<double> OParameters, std::vector<double> EParameters);
            Quadric(const char* name, std::vector<double> inputPoints, std::vector<double> EParameters, double width, double height, double alpha, double chi, double beta, double dist, std::vector<double> misalignmentParams, std::vector<double> tempMisalignmentParams, std::vector<double> slopeError, Quadric* previous);
            Quadric(const char* name, double width, double height, std::vector<double> slopeError, Quadric* previous);

            std::vector<double> getQuadric();
            void setElementParameters(std::vector<double> params);
            void setObjectParameters(std::vector<double> params);
            void editQuadric(std::vector<double> inputPoints);
            std::vector<double> getAnchorPoints();
            void setInMatrix(std::vector<double> inputMatrix);
            void setOutMatrix(std::vector<double> inputMatrix);

            std::vector<double> getInMatrix();
            std::vector<double> getOutMatrix();
            std::vector<double> getMisalignmentMatrix();
            std::vector<double> getInverseMisalignmentMatrix();
            std::vector<double> getMisalignmentParams();
            std::vector<double> getB2E();
            std::vector<double> getE2B();
            std::vector<double> getInvB2E();
            std::vector<double> getInvE2B();
            std::vector<double> getG2E();
            std::vector<double> getE2G();
            
            std::vector<double> getInTransMis();
            std::vector<double> getOutTransMis();
            std::vector<double> getTempMisalignmentParams();
            std::vector<double> getTempMisalignmentMatrix();
            std::vector<double> getInverseTempMisalignmentMatrix();
            std::vector<double> getObjectParameters();
            std::vector<double> getElementParameters();
            std::vector<double> getSlopeError();
            const char* getName();

            void calcTransformationMatrices(double alpha, double chi, double beta, double dist, std::vector<double> misalignment);
            void setTemporaryMisalignment(std::vector<double> misalignment);
            
            Quadric();
            ~Quadric();
        private:
            const char* m_name;
            Quadric* m_previous;
            std::vector<double> m_anchorPoints;
            // dx,dy,dz,psi(x-axis),dphi(y-axis),dchi(z-axis) angles in rad, the order is fixed. This is added to the ray together with the InTrans and OutTrans in the beginning and end of the tracing process.
            std::vector<double> m_misalignmentParams; 
            // stored only for completeness and not to transfer to the shader, these transformations are part of inMatrix/outMatrix which are transferred.
            std::vector<double> d_misalignmentMatrix;
            std::vector<double> d_inverseMisalignmentMatrix;
            // beam element transformation matrices derived directly from params alpha, beta, chi and distance
            std::vector<double> d_b2e; // inTrans M_b2e
            std::vector<double> d_e2b; // m_outTrans; // M_e2b
            std::vector<double> d_inv_b2e; // m_invInTrans; (M_b2e)^-1
            std::vector<double> d_inv_e2b; // m_invOutTrans; (M_e2b)^-1 = (M_e2b)^T
            // transformation between global and element coordinate system, derived from beam-element matrices (of this and previous element)
            std::vector<double> d_g2e; // m_inTransMis
            std::vector<double> d_e2g; // m_outTransMis;
            
            // sometimes it is necessary to remove (partly (ellipsoid) or the whole (RZP)) misalignment midtracing, this is stored here
            std::vector<double> m_temporaryMisalignmentParams;
            std::vector<double> m_temporaryMisalignmentMatrix;
            std::vector<double> m_inverseTemporaryMisalignmentMatrix;

            // stores m_misalignmentMatrix and d_b2e / d_g2e
            std::vector<double> m_inMatrix;
            // stores d_e2g / d_e2b and m_inverseMisalignmentMatrix
            std::vector<double> m_outMatrix;
            // things every optical element has (e.g. slope error) (16 entries -> one dmat4x4 in shader)
            std::vector<double> m_objectParameters; 
            // additional element-specific parameters that are used for tracing (16 entries -> one dmat4x4 in shader)
            std::vector<double> m_elementParameters;
            // 7 paramters that specify the slope error, are stored in objectParamters to give to shader
            std::vector<double> m_slopeError;
            
    };
}