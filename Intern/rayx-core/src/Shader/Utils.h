#include "Adapt.h"
#include "Ray.h"

// converts energy in eV to wavelength in nm
double RAYX_API hvlam(double x);

double infinity();

// multiplies position and direction of ray r with transformation matrix m
// r = dot(m, r)
Ray RAYX_API rayMatrixMult(Ray r, const dmat4 m);

Ray rotationAroundX(Ray r, double sin_psi, double cos_psi);

//TODO: doku
dvec3 RAYX_API cubicPosition(Ray r, double alpha);

dvec3 RAYX_API cubicDirection(Ray r, double alpha);

/**
 * uses given transformation matrix to transform ray position and direction into
 * Object coordinates also takes misalignment of the object into account
 * @param r				input ray in Ray-coordinates
 * @param misalignment	4x4 homogeneous matrix that contains the misalignment of
 * the element (dx, dy, dz, dphi, dpsi, dchi)
 * @param inTrans		4x4 homogeneous matrix that contains the transformation
 * of ray-coordinates to object coordinates (distance to previous element,
 * grazing incidence angle, azimuthal angle)
 *
 * output: nothing. changes r in place
 */
void inTransformation(RAYX_INOUT(Ray) r, const dmat4 misalignment, const dmat4 inTrans);

/**
 * uses given transformation matrix to transform ray position and direction back
 * into Ray coordinates also takes misalignment of the object into account. Note
 * that the order of the transformations is reversed and we use the inverse of
 * the misalignment matrix
 * @param r				input ray in Ray-coordinates
 * @param invMisalignment	4x4 homogeneous matrix that contains the
 * misalignment of the element (dx, dy, dz, dphi, dpsi, dchi)
 * @param outTrans		3x3 homogeneous matrix (in 4x4 matrix for
 * standardization) that contains the transformation of object coordinates to
 * ray-coordinates (rotations only: grazing exit angle, neg. azimuthal angle)
 *
 * output: nothing. changes r in place
 */
void outTransformation(RAYX_INOUT(Ray) r, const dmat4 invMisalignment, const dmat4 outTrans);

// only for testing purposes. generally, each angle in the shader is in rad!!
double deg2rad(double degree);

// returns angle between ray direction and surface normal at intersection point
double RAYX_API getIncidenceAngle(Ray r, dvec3 normal);

/** creates a Müller matrix (see p. 32 of Schäfers, 2007)
 * @param R_s
 * @param R_p
 * @param delta
 *
 */
dmat4 muller_matrix(double R_s, double R_p, double delta);

/** returns updated stokes vector
 * @param r			ray
 * @param real_S	real part of the complex s-polarization
 * @param real_P	real part of the complex p-polarization
 * @param delta		phase difference between complex s and p polarization
 * @param azimuthal	angle of rotation of element in xy plane (chi + dchi
 * (misalignment))
 * updates stokes vector of ray
 * @returns `true`, if ray should be absorbed
 */
bool update_stokes(RAYX_INOUT(Ray) r, double real_S, double real_P, double delta, double azimuthal);

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normal_cartesian(dvec3 normal, double x_rad, double z_rad);

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normal_cylindrical(dvec3 normal, double x_rad, double z_rad);

/**
adds slope error to the normal
@params:
    normal: normal vector
    error: the slope error to apply
    O_type: cartesian or cylindrical type of slope error (0=cartesian,
1=cylindrical) (1 only for ellipsis relevant) returns new normal if there is a
slope error in either x or z direction or the unmodified normal otherwise.
*/
dvec3 applySlopeError(dvec3 normal, SlopeError error, int O_type);

/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@params: lots
@returns: (inplace) DX, DZ
*/
void RAYX_API RZPLineDensity(Ray r, dvec3 normal, RZPBehaviour b, RAYX_INOUT(double) DX, RAYX_INOUT(double) DZ);

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
Ray refrac2D(Ray r, dvec3 normal, double az, double ax);

// refraction function used for gratings
Ray refrac(Ray r, dvec3 normal, double linedensity);

/** variable line spacing for gratings
@params
    lineDensity: general line density?
    z: z-coordinate of ray position
    vls[6]: 6 vls parameters given by user
@returns line density specifically for this z-coordinate
*/
double RAYX_API vlsGrating(double lineDensity, double z, double vls[6]);

/*
 * simplified refraction function used for plane gratings
 * normal is always the same (0,1,0) -> no rotation and thus no trigonometric
 * functions necessary
 * @param r			ray
 * @param normal		normal at ray-object intersection (for planes always
 * (0,1,0))
 * @param a			a = WL * D0 * ORD * 1.e-6  with D0: line density (l/mm);
 * WL:wavelength (nm); ORD order of diffraction
 */
Ray RAYX_API refrac_plane(Ray r, ALLOW_UNUSED dvec3 normal,
                          double a);  // TODO fix unused var

// Cutout helper functions:

// checks whether the point (x, z) is within the cutout.
bool RAYX_API inCutout(Cutout cutout, double x, double z);

// returns a matrix M where (M[i].x, M[i].z) are the key points of our cutout.
// The key points are typically points on the boundary of the cutout.
dmat4 RAYX_API keyCutoutPoints(Cutout cutout);

// returns width and length of the bounding box.
dvec2 RAYX_API cutoutBoundingBox(Cutout cutout);

// checks whether c1 is a subset of c2, and prints an error otherwise.
// might not find all subset-violations, but should find most of them.
// (might not find all Ellipsoid vs Trapezoid violations)
void RAYX_API assertCutoutSubset(Cutout c1, Cutout c2);


