#pragma once

#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "BeamlineObject.h"
#include "Core.h"
#include "Surface/Surface.h"
#include "utils.h"

namespace RAYX
{

    class RAYX_API OpticalElement : public BeamlineObject
    {
    public:

        OpticalElement(const char* name, const std::vector<double> surfacePoints, const std::vector<double> inputInMatrix, const std::vector<double> inputOutMatrix, const std::vector<double> misalignmentMatrix, const std::vector<double> inverseMisalignmentMatrix, const std::vector<double> OParameters, const std::vector<double> EParameters);
        OpticalElement(const char* name, const std::vector<double> EParameters, const double width, const double height, const double alpha, const double chi, const double beta, const double dist, const std::vector<double> misalignmentParams, const std::vector<double> tempMisalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global);
        OpticalElement(const char* name, const double width, const double height, const double chi, const double dist, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous);
        OpticalElement(const char* name, const double chi, const double dist, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous);

        void setElementParameters(std::vector<double> params);
        void setDimensions(double width, double height);
        void setInMatrix(std::vector<double> inputMatrix);
        void setOutMatrix(std::vector<double> inputMatrix);
        void setSurface(std::unique_ptr<Surface> surface);
        void updateObjectParams();

        void setAlpha(double alpha);
        void setBeta(double beta);

        double getWidth() const;
        double getHeight() const;
        double getBeta() const;
        double getAlpha() const;
        double getChi() const;
        double getDistanceToPreceedingElement() const;

        std::vector<double> getInMatrix() const;
        std::vector<double> getOutMatrix() const;
        std::vector<double> getMisalignmentMatrix() const;
        std::vector<double> getInverseMisalignmentMatrix() const;
        std::vector<double> getMisalignmentParams() const;
        std::vector<double> getB2E() const;
        std::vector<double> getE2B() const;
        std::vector<double> getInvB2E() const;
        std::vector<double> getInvE2B() const;
        std::vector<double> getG2E() const;
        std::vector<double> getE2G() const;

        std::vector<double> getInTransMis() const;
        std::vector<double> getOutTransMis() const;
        std::vector<double> getTempMisalignmentParams() const;
        std::vector<double> getTempMisalignmentMatrix() const;
        std::vector<double> getInverseTempMisalignmentMatrix() const;
        std::vector<double> getObjectParameters();
        std::vector<double> getElementParameters() const;
        std::vector<double> getSurfaceParams() const;
        std::vector<double> getSlopeError() const;

        void calcTransformationMatrices(const std::vector<double> misalignment, bool global);
        void setTemporaryMisalignment(std::vector<double> misalignment);

        OpticalElement();
        ~OpticalElement();

    private:
        // Geometric Parameter
        double m_width;
        double m_height;
        // 7 paramters that specify the slope error, are stored in objectParamters to give to shader
        std::vector<double> m_slopeError;
        // stored only for completeness and not to transfer to the shader, these transformations are part of inMatrix/outMatrix which are transferred.
        std::vector<double> d_misalignmentMatrix;
        std::vector<double> d_inverseMisalignmentMatrix;


        // User/Design Parameter
        double m_alpha;
        double m_beta;
        double m_chi;
        double m_distanceToPreceedingElement;
        // previous optical element (needed for read access of the transformation matrices to derive global coordinates for this element)
        std::shared_ptr<OpticalElement> m_previous;
        // dx,dy,dz,psi(x-axis),dphi(y-axis),dchi(z-axis) angles in rad, the order is fixed. This is added to the ray together with the InTrans and OutTrans in the beginning and end of the tracing process.
        std::vector<double> m_misalignmentParams;

        // transformation matrices:
        // beam element transformation matrices derived directly from params alpha, beta, chi and distance
        // TODO(Jannis): remove and make local for functions that need them
        std::vector<double> d_b2e; // inTrans M_b2e
        std::vector<double> d_e2b; // outTrans M_e2b
        std::vector<double> d_inv_b2e; // invInTrans; (M_b2e)^-1
        std::vector<double> d_inv_e2b; // invOutTrans; (M_e2b)^-1 = (M_e2b)^T
        // transformation between global and element coordinate system, derived from beam-element matrices (of this and previous element, see wiki)
        std::vector<double> d_g2e; // m_inTransMis
        std::vector<double> d_e2g; // m_outTransMis;
        // ----

        // Surface (eg Quadric or if eg torus something else)
        std::unique_ptr<Surface> m_surface;
        std::vector<double> m_surfaceParams; // used to be anchor points

        // TODO(Jannis): remove temporary misalignment (maybe move to rzp)
        // sometimes it is necessary to remove (partly (ellipsoid) or the whole (RZP)) misalignment midtracing, the parameters (see misalignment) is stored here and the corresponding
        // transformation matrix in m_temporaryMisalignmentMatrix (0's if not needed)
        std::vector<double> m_temporaryMisalignmentParams;

        // These four transformation matrices matrices are put to the shader:
        std::vector<double> m_temporaryMisalignmentMatrix;
        std::vector<double> m_inverseTemporaryMisalignmentMatrix;

        // stores d_misalignmentMatrix and d_b2e / d_g2e as one final transformation matrix
        std::vector<double> m_inMatrix;
        // stores d_e2g / d_e2b and d_inverseMisalignmentMatrix
        std::vector<double> m_outMatrix;


        // things every optical element has (e.g. slope error) (16 entries -> one dmat4x4 in shader)
        // also put to shader
        std::vector<double> m_objectParameters;
        // additional element-specific parameters that are used for tracing (16 entries -> one dmat4x4 in shader)
        std::vector<double> m_elementParameters;
    };


} // namespace RAYX

