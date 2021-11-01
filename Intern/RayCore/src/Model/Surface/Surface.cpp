#include "Surface.h"
#include <cassert>
#include <math.h>

namespace RAYX
{
    /**
     * standard constructor 
     * this class uses the parameters for the quadric equation!
     *
     * angles given in rad
     * define transformation matrices based on grazing incidence (alpha) and exit (beta) angle, azimuthal angle (chi) and distance to preceeding element
     * @param inputPoints      Matrix A for quadric surfaces with a_11,a_12,a_13,a_14, a_21,a_22,a_23,a_24, a_31,a_32,a_33,a_34, a_41,a_42,a_43,a_44
     *                      a_21,a_31,a_32,a_41,a_42,a_43 are never used for quadric surfaces because the matrix is symmetrial,
    */
    Surface::Surface() {}

    Surface::~Surface()
    {
    }

} // namespace RAYX