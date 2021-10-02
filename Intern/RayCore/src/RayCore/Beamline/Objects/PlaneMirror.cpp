#include "PlaneMirror.h"

namespace RAYX
{

    /**
     * angles given in degree and stored in rad
     * initializes transformation matrices, and parameters for the quadric in super class (optical element)
     * sets mirror-specific parameters in this class
     * @param name                  name of the plane mirror
     * @param width                 width of mirror (x-dimension in element coord. sys.)
     * @param height                height of mirror (z-dimension in element coord. sys.)
     * @param grazingIncidence      desired incidence angle of the main ray
     * @param azimuthal             rotation of mirror around z-axis
     * @param dist                  distance to preceeding element
     * @param misalignmentParams    angles and distances for the mirror's misalignment
     * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * @param previous              pointer to previous element in beamline, needed for calculating transformation matrices in global coordinate system
     * @param global
     *
    */
    PlaneMirror::PlaneMirror(const char* name, const double width, const double height, const double grazingIncidence, const double azimuthal, const double dist, const std::vector<double> misalignmentParams, const std::vector<double> slopeError, const std::shared_ptr<OpticalElement> previous, bool global)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width,
            height, rad(grazingIncidence), rad(azimuthal), rad(grazingIncidence),
            dist, misalignmentParams, { 0,0,0,0,0,0 }, slopeError, previous, global)
    {
        setSurface(std::make_unique<Quadric>(std::vector<double>{0,0,0,0, 1,0,0,-1, 0,0,0,0, 0,0,0,0}));
    }

    /**
     * initializes transformation matrices, and parameters for the quadric in super class (optical element)
     * sets mirror-specific parameters in this class
     * @param name                  name of the plane mirror
     * @param width                 width of mirror (x-dimension in element coord. sys.)
     * @param height                height of mirror (z-dimension in element coord. sys.)
     * @param position              position in world coordinates (in homogeneous coordinates)
     * @param orientation           orientation in world coordinates
     * @param slopeError            7 slope error parameters: x-y sagittal (0), y-z meridional (1), thermal distortion x (2),y (3),z (4), cylindrical bowing amplitude y(5) and radius (6)
     * 
     */
    PlaneMirror::PlaneMirror(const char* name, const double width, const double height, glm::dvec4 position, glm::dmat4x4 orientation, const std::vector<double> slopeError)
        : OpticalElement(name, { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, width,
            height, position, orientation, { 0,0,0,0,0,0 }, slopeError)
    {
        setSurface(std::make_unique<Quadric>(std::vector<double>{0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0}));
    }

    PlaneMirror::~PlaneMirror()
    {
    }

}
