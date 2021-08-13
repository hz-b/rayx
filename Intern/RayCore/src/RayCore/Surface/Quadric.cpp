#include "Quadric.h"
#include <cassert>
#include <math.h>

namespace RAYX
{
    /**
     * standard constructor
     * this class the parameters for the quadric equation!
     *
     * angles given in rad
     * define transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
     * @param: inputPoints      Matrix A for quadric surfaces with a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     *                      a_21,a_31,a_32,a_41,a_42,a_43 are never used for quadric surfaces because the matrix is symmetrial,
    */
    Quadric::Quadric(const std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16); //parameter size ==6?
        m_anchorPoints = inputPoints;
    }



    Quadric::Quadric() {} // TODO

    Quadric::~Quadric()
    {
    }

    /**
     * set a new set of parameters a_11 to a_44 for the quadric function
     * order: a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     * @param inputPoints   16 entry vector a_11 to a_44
     * @return void
    */
    void Quadric::setAnchorPoints(std::vector<double> inputPoints)
    {
        assert(inputPoints.size() == 16);
        m_anchorPoints = inputPoints;
    }

    std::vector<double> Quadric::getParams() const {
        std::cout << "return anchor points" << std::endl;
        return m_anchorPoints;
    }

} // namespace RAYX