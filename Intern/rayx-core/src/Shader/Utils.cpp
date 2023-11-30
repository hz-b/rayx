#include "Utils.h"
#include "InvocationState.h"
#include "Complex.h"
#include "Approx.h"
#include "Rand.h"
#include "Helper.h"

// converts energy in eV to wavelength in nm
double RAYX_API hvlam(double x) {
    if (x == 0) {
        return 0.0;
    }
    return inm2eV / x;
}

// originally we calculcated 1/0, which might be UB (https://stackoverflow.com/questions/5802351/what-happens-when-you-divide-by-0-in-a-shader).
// `return 1e+308 * 2.0;` correctly returns positive infinity on my machine (Rudi), but we have no guarantee that it always does.
// So instead we return a sufficiently large number to be used like positive infinity.
double infinity() { return 1e+308; }

// multiplies position and direction of ray r with transformation matrix m
// r = dot(m, r)
Ray RAYX_API rayMatrixMult(Ray r, const dmat4 m) {
    r.m_position = dvec3(m * dvec4(r.m_position, 1));
    r.m_direction = dvec3(m * dvec4(r.m_direction, 0));
    return r;
}

Ray rotationAroundX(Ray r, double sin_psi, double cos_psi) {
    dmat4 rotation_matrix = dmat4(1, 0, 0, 0, 0, cos_psi, -sin_psi, 0, 0, sin_psi, cos_psi, 0, 0, 0, 0, 1);
    return rayMatrixMult(r, rotation_matrix);
}

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
dvec3 RAYX_API cubicPosition(Ray r, double alpha) {
    
    double yy = r.m_position[1];
    double y = yy * r8_cos(alpha) - r.m_position[2] * r8_sin(alpha);
    double z = (r.m_position[2]) * r8_cos(alpha) + yy * r8_sin(alpha);

    return dvec3(0, y, z);
}

// rotates for the cubic collision by angle alpha (taken from RAY-UI)
dvec3 RAYX_API cubicDirection(Ray r, double alpha) {
    
    double am = r.m_direction[1];
    double dy = r.m_direction[1] * r8_cos(alpha) - r.m_direction[2] * r8_sin(alpha);
    double dz = r.m_direction[2] * r8_cos(alpha) + am * r8_sin(alpha);

    return dvec3(0, dy, dz);
}

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
void inTransformation(RAYX_INOUT(Ray) r, const dmat4 misalignment, const dmat4 inTrans) {
    r.m_position = dvec3(misalignment * (inTrans * dvec4(r.m_position, 1)));
    r.m_direction = dvec3(misalignment * (inTrans * dvec4(r.m_direction, 0)));
}

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
void outTransformation(RAYX_INOUT(Ray) r, const dmat4 invMisalignment, const dmat4 outTrans) {
    r.m_position = dvec3(outTrans * (invMisalignment * dvec4(r.m_position, 1)));
    r.m_direction = dvec3(outTrans * (invMisalignment * dvec4(r.m_direction, 0)));
}

// only for testing purposes. generally, each angle in the shader is in rad!!
double deg2rad(double degree) {
    // double PI = 3.141592653589793238462643383279502884197169399;//3 7 5 1 0 5
    // 8 2 0 9 7 4 9 4 4 5 9
    return PI * degree / 180;
}

// returns forial of a (from RAYLIB.for) (a!)
double RAYX_API fact(int a) {
    if (a < 0) {
        return a;
    }
    double f = 1;
    for (int i = 2; i <= a; i++) {
        f *= i;
    }
    return f;
}

// returns dvec2(atomic mass, density) extracted from materials.xmacro
dvec2 RAYX_API getAtomicMassAndRho(int material) {
    switch (material) {
#define X(e, z, a, rho) \
    case z:             \
        return dvec2(a, rho);
#include "../Material/materials.xmacro"
#undef X
    }
    _throw("invalid material in getAtomicMassAndRho");
    return dvec2(0.0, 0.0);
}

#include "../Shader/NffPalik.h"

int RAYX_API getPalikEntryCount(int material) {
    int m = material - 1;  // in [0, 91]
    // this counts how many doubles are in between the materials index, and the
    // next index in the table. division by 3, because each entry has 3 members
    // currently: energy, n, k, padding.
    return (inv_matIdx[m + 1] - inv_matIdx[m]) / 3;
}

int RAYX_API getNffEntryCount(int material) {
    int m = material - 1;  // in [0, 91]
    // the offset of 92 (== number of materials), skips the palik table and
    // reaches into the nff table. the rest of the logic is as above.
    return (inv_matIdx[92 + m + 1] - inv_matIdx[92 + m]) / 3;
}

PalikEntry RAYX_API getPalikEntry(int index, int material) {
    int m = material - 1;  // in [0, 91]
    // inv_matIdx[m] is the start of the Palik table of material m.
    // 3*index skips 'index'-many entries.
    int i = inv_matIdx[m] + 3 * index;

    PalikEntry e;
    e.m_energy = inv_mat[i];
    e.m_n = inv_mat[i + 1];
    e.m_k = inv_mat[i + 2];

    return e;
}

NffEntry RAYX_API getNffEntry(int index, int material) {
    int m = material - 1;  // in [0, 91]
    // inv_matIdx[92+m] is the start of the Nff table of material m.
    // 3*index skips 'index'-many entries.
    int i = inv_matIdx[92 + m] + 3 * index;

    NffEntry e;
    e.m_energy = inv_mat[i];
    e.m_f1 = inv_mat[i + 1];
    e.m_f2 = inv_mat[i + 2];

    return e;
}

// returns dvec2 to represent a complex number
dvec2 RAYX_API getRefractiveIndex(double energy, int material) {
    if (material == -1) {  // vacuum
        return dvec2(1., 0.);
    }

    // out of range check
    if (material < 1 || material > 92) {
        _throw("getRefractiveIndex material out of range!");
        return dvec2(-1.0, -1.0);
    }

    // try to get refractive index using Palik table
    if (getPalikEntryCount(material) > 0) {           // don't try binary search if there are 0 entries!
        int low = 0;                                  // <= energy
        int high = getPalikEntryCount(material) - 1;  // >= energy

        PalikEntry low_entry = getPalikEntry(low, material);
        PalikEntry high_entry = getPalikEntry(high, material);

        if (low_entry.m_energy <= energy && energy <= high_entry.m_energy) {  // if 'energy' is in range of tha PalikTable
            // binary search
            while (high - low > 1) {
                int center = (low + high) / 2;
                PalikEntry center_entry = getPalikEntry(center, material);
                if (energy < center_entry.m_energy) {
                    high = center;
                } else {
                    low = center;
                }
            }

            PalikEntry entry = getPalikEntry(low, material);
            return dvec2(entry.m_n, entry.m_k);
        }
    }

    // get refractive index with Nff table
    if (getNffEntryCount(material) > 0) {           // don't try binary search if there are 0 entries!
        int low = 0;                                // <= energy
        int high = getNffEntryCount(material) - 1;  // >= energy

        // binary search
        while (high - low > 1) {
            int center = (low + high) / 2;
            NffEntry center_entry = getNffEntry(center, material);
            if (energy < center_entry.m_energy) {
                high = center;
            } else {
                low = center;
            }
        }

        dvec2 massAndRho = getAtomicMassAndRho(material);
        double mass = massAndRho.x;
        double rho = massAndRho.y;

        NffEntry entry = getNffEntry(low, material);
        double e = entry.m_energy;
        double n = 1 - (415.252 * rho * entry.m_f1) / (e * e * mass);
        double k = (415.252 * rho * entry.m_f2) / (e * e * mass);

        return dvec2(n, k);
    }

    _throw("getRefractiveIndex: no matching entry found!");
    return dvec2(-1.0, -1.0);
}

// returns angle between ray direction and surface normal at intersection point
double RAYX_API getIncidenceAngle(Ray r, dvec3 normal) {
    normal = normalize(normal);
    double ar = dot(dvec3(normal), r.m_direction);
    // cut to interval [-1,1]
    if (ar < -1.0)
        ar = -1.0;
    else if (ar > 1.0)
        ar = 1.0;

    double theta = PI - r8_acos(ar);
    int gt = int(theta > PI / 2);
    // theta in [0, pi]
    theta = gt * (PI - theta) + (1 - gt) * theta;
    return theta;
}

/** calculates cosinus of transmittance angle with snell's law
 * cosinus is needed in fresnel formula
 * sin(incidence_angle)² = 1 - cos(incidence_angle)²
 * ratio² = (cn1 / cn2)²
 * sin(transmittance_angle)² = (cn1 / cn2)² * sin(incidence_angle)²
 * cos(transmittance_angle) = sqrt( 1 - sin(transmittance_angle)²)
 *
 * @param cos_incidence			cosinus of complex incidence angle
 * @param cn1					complex refractive index of material from which
 * the ray is coming
 * @param cn2					complex refractive index of material into which
 * the ray is going
 * @return cos_transmittance 	cosinus of complex transmittance angle
 */
dvec2 RAYX_API snell(dvec2 cos_incidence, dvec2 cn1, dvec2 cn2) {
    /*dvec2 sin_incidence = cx_sqrt( cx_sub(dvec2(1.0,0.0),
    cx_mul(cos_incidence, cos_incidence)) ); dvec2 refractive_ratio =
    cx_div(cn1, cn2); dvec2 sin_transmittance = cx_mul(sin_incidence,
    refractive_ratio); dvec2 cos_transmittance = cx_sqrt( cx_sub(dvec2(1.0,0.0),
    cx_mul(sin_transmittance, sin_transmittance))); return cos_transmittance;*/
    dvec2 sin_incidence_square = cx_sub(dvec2(1.0, 0.0), cx_mul(cos_incidence, cos_incidence));  // 1 - cos(inc)**2
    dvec2 refractive_ratio = cx_div(cn1, cn2);                                                   // cn1 / cn2
    dvec2 ratio_square = cx_mul(refractive_ratio, refractive_ratio);
    dvec2 sin_transmittance_square = cx_mul(sin_incidence_square,
                                            ratio_square);  // sin(tr)**2 = sin(inc)**2 * (cn1/cn2)**2
    dvec2 cos_transmittance = cx_sqrt(cx_sub(dvec2(1.0, 0.0),
                                             sin_transmittance_square));  // cos = sqrt( 1 - ratio**2*sin(tr)**2 )
    return cos_transmittance;
}

/** calculates complex s- and p-polarization with fresnel formulas
 * @param cn1					complex refractive index of material from which
 * the ray is coming
 * @param cn2					complex refractive index of material into which
 * the ray is going
 * @param cos_incidence			cosinus of complex incidence angle
 * @param cos_transmittance 	cosinus of complex transmittance angle
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 *
 */
void RAYX_API fresnel(dvec2 cn1, dvec2 cn2, dvec2 cos_incidence, dvec2 cos_transmittance, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P) {
    dvec2 mat1_incidence = cx_mul(cn1, cos_incidence);  // material one (incoming side)
    dvec2 mat1_transmittance = cx_mul(cn1, cos_transmittance);
    dvec2 mat2_incidence = cx_mul(cn2, cos_incidence);  // material two (into which the ray goes)
    dvec2 mat2_transmittance = cx_mul(cn2, cos_transmittance);

    complex_S = cx_div(cx_sub(mat1_incidence, mat2_transmittance), cx_add(mat1_incidence, mat2_transmittance));
    complex_P = -cx_div(cx_sub(mat1_transmittance, mat2_incidence), cx_add(mat1_transmittance, mat2_incidence));
    return;
}

/** computes complex s and p polarization over all layers with fresnel and
 * snell's law
 * @param energy 				energy of ray
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @return complex_S			complex s-polarization
 * @return complex_P			complex p-polarization
 */
void RAYX_API reflectance(double energy, double incidence_angle, RAYX_INOUT(dvec2) complex_S, RAYX_INOUT(dvec2) complex_P, int material) {
    dvec2 cos_incidence = dvec2(r8_cos(incidence_angle),
                                0.0);  // complex number, initialization only for first layer, the
                                       // others are then derived from this with snell's law

    const int vacuum_material = -1;

    // todo number of layers: loop over layers from top to bottom, transmittance
    // angle for each layer, so far only one layer (substrate?) store cosinuses
    // in array, bc needed in later loop for fresnel (or maybe only one loop is
    // enough?) todo refractive indices of materials in extra buffer?
    dvec2 cn1 = getRefractiveIndex(energy, vacuum_material);
    dvec2 cn2 = getRefractiveIndex(energy, material);
    dvec2 cos_transmittance = snell(cos_incidence, cn1, cn2);

    // todo again iterate over layers but from bottom to top, update s and p
    // polarization in each iteration:
    fresnel(cn2, cn1, cos_transmittance, cos_incidence, complex_S, complex_P);
    // todo roughness (uses wavelength/energy)
    // todo other calculations for layers that are not substrate (which is the
    // first layer)
    return;
}
/**
 * computes complex number a + i*b in euler form:
 * euler = r * e^(i * phi) where r = sqrt(a**2 + b**2) = radius and phi =
 * atan2(a,b) = (absolute) phase
 */
dvec2 RAYX_API cartesian_to_euler(dvec2 complex) {
    double r = dot(complex,
                   complex);  // r = sqrt(a**2 + b**2), why not take sqrt in fortran
                              // code?, maybe better bc square root looses precision
    dvec2 euler;
    if (r < 0 || r > 1) {
        euler = dvec2(infinity(), infinity());
        return euler;
    }
    euler = dvec2(r, r8_atan2(complex.y, complex.x));  // phi in rad
    return euler;
}

/** computes the difference in the phases of 2 complex number written in euler
 * form: r * e^(i * phi)
 * @param euler1 		first complex number // s
 * @param euler2		second complex number // p
 * @return delta = phi1 - phi2, in [-pi/2, pi/2] degrees
 */
double phase_difference(dvec2 euler1, dvec2 euler2) {
    double delta = euler2.y - euler1.y;  // p - s
    delta = delta - int(delta > 180) * 360.0 + int(delta < -180) * 360.0;
    return delta;
}

/** efficiency calculation
 * uses complex numbers for s- and p-polarisation:
 * complex_S = a + i*b as dvec2.
 * in euler form: complex_S = r * e^(i * phi) where r = sqrt(a**2 + b**2) =
 * radius and phi = atan2(a,b) = (absolute) phase
 * @param real_S				inout, s-polarization (senkrecht), radius of
 * complex number
 * @param real_P				inout, p-polarization (parallel), radius of
 * complex number
 * @param delta					phase difference between angles of complex
 * numbers complex_S, complex_P
 * @param incidence_angle		normal incidence angle
 * @param material				material the photon collides with
 * @param others
 */
void efficiency(Ray r, RAYX_OUT(double) real_S, RAYX_OUT(double) real_P, RAYX_OUT(double) delta, double incidence_angle, int material) {
    dvec2 complex_S, complex_P;
    reflectance(r.m_energy, incidence_angle, complex_S, complex_P, material);

    dvec2 euler_P = cartesian_to_euler(complex_P);
    dvec2 euler_S = cartesian_to_euler(complex_S);

    delta = phase_difference(euler_S, euler_P);
    real_S = euler_S.x;
    real_P = euler_P.x;
}

/** creates a Müller matrix (see p. 32 of Schäfers, 2007)
 * @param R_s
 * @param R_p
 * @param delta
 *
 */
dmat4 muller_matrix(double R_s, double R_p, double delta) {
    double c = r8_cos(delta);
    double s = r8_sin(delta);
    return dmat4((R_s + R_p) / 2.0, (R_p - R_s) / 2.0, 0.0, 0.0, (R_p - R_s) / 2.0, (R_s + R_p) / 2.0, 0.0, 0.0, 0.0, 0.0, R_s * R_p * c,
                 R_s * R_p * s, 0.0, 0.0, -R_s * R_p * s, R_s * R_p * c);
}

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
bool update_stokes(RAYX_INOUT(Ray) r, double real_S, double real_P, double delta, double azimuthal) {
    dvec4 stokes_old = r.m_stokes;
    double c_chi = r8_cos(azimuthal);
    double s_chi = r8_sin(azimuthal);
    dmat4 rot = dmat4(1, 0, 0, 0, 0, c_chi, -s_chi, 0, 0, s_chi, c_chi, 0, 0, 0, 0, 1);
    dmat4 inv_rot = transpose(rot);

    dvec4 stokes_new = muller_matrix(real_S, real_P, delta) * rot * stokes_old;
    ;
    r.m_stokes = inv_rot * stokes_new;

    double rn = squaresDoubleRNG(inv_ctr);
    // throw ray away with certain probability
    return (r.m_stokes.x / stokes_old.x) - rn <= 0;
}

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normal_cartesian(dvec3 normal, double x_rad, double z_rad) {
    double FX = normal[0];
    double FY = normal[1];
    double FZ = normal[2];

    double cosx = r8_cos(x_rad);
    double sinx = r8_sin(x_rad);
    double cosz = r8_cos(z_rad);
    double sinz = r8_sin(z_rad);

    // put in matrix mult?
    double FY2 = FY * cosz + FZ * sinz;
    normal[0] = FX * cosx + FY2 * sinx;
    normal[1] = FY2 * cosx - FX * sinx;
    normal[2] = FZ * cosz - FY * sinz;

    return normal;
}

/**
turn the normal vector through x_rad and z_rad
@params:  	normal: the normal vector
            x_rad: angle in rad for x-axis
            z_rad: angle in rad for z-axis
returns modified normal vector
*/
dvec3 RAYX_API normal_cylindrical(dvec3 normal, double x_rad, double z_rad) {
    double normFXFY = sqrt(normal[0] * normal[0] + normal[1] * normal[1]);
    double arcTanFXFY = r8_atan2(normal[1], normal[0]);
    double sinz = r8_sin(z_rad);
    double cosz = r8_cos(z_rad);

    normal[0] = r8_cos(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[1] = r8_sin(x_rad + arcTanFXFY) * (normFXFY * cosz + normal[2] * sinz);
    normal[2] = normal[2] * cosz - normFXFY * sinz;

    return normal;
}

/**
adds slope error to the normal
@params:
    normal: normal vector
    error: the slope error to apply
    O_type: cartesian or cylindrical type of slope error (0=cartesian,
1=cylindrical) (1 only for ellipsis relevant) returns new normal if there is a
slope error in either x or z direction or the unmodified normal otherwise.
*/
dvec3 applySlopeError(dvec3 normal, SlopeError error, int O_type) {
    double slopeX = error.m_sag;
    double slopeZ = error.m_mer;

    // only calculate the random number if at least one slope error is not 0,
    // since the calculation is costly (sin, cos, log involved)
    if (slopeX != 0 || slopeZ != 0) {
        double random_values[2] = {squaresNormalRNG(inv_ctr, 0, slopeX), squaresNormalRNG(inv_ctr, 0, slopeZ)};

        /*double x = random_values[0] * slopeX; // to get normal distribution
        from std.-norm. multiply by sigma (=slopeX) -> mu + x * sigma but mu=0
        here double z = random_values[1] * slopeZ;*/

        double x_rad = deg2rad(random_values[0] / 3600.0);
        double z_rad = deg2rad(random_values[1] / 3600.0);

        if (O_type == 0) {  // default
            normal = normal_cartesian(normal, x_rad, z_rad);
        } else if (O_type == 1) {
            normal = normal_cylindrical(normal, x_rad, z_rad);
        }
    }

    return normal;
}

/**
calculates DX and DZ (line spacing in x and z direction) at a given point for a
given direction on the grating
@params: lots
@returns: (inplace) DX, DZ
*/
void RAYX_API RZPLineDensity(Ray r, dvec3 normal, RZPBehaviour b, RAYX_INOUT(double) DX, RAYX_INOUT(double) DZ) {
    int IMAGE_TYPE = int(b.m_imageType);
    int RZP_TYPE = int(b.m_rzpType);
    double risag = b.m_designSagittalEntranceArmLength;
    double rosag = b.m_designSagittalExitArmLength;
    double rimer = b.m_designMeridionalEntranceArmLength;
    double romer = b.m_designMeridionalExitArmLength;
    double alpha = b.m_designAlphaAngle;
    double beta = b.m_designBetaAngle;
    double WL = 1e-06 * b.m_designWavelength;  // source energy/design energy
    double Ord = b.m_orderOfDiffraction;

    double FX = normal.x;
    double FY = normal.y;
    double FZ = normal.z;
    double X = r.m_position.x;
    double Y = r.m_position.y;
    double Z = r.m_position.z;

    if (RZP_TYPE == 1)  // meridional (wie VLS grating)
        X = 0;

    // avoid calculating the same sinus/cosinus multiple times (costly)
    double s_beta = r8_sin(beta);
    double c_beta = r8_cos(beta);
    double s_alpha = r8_sin(alpha);
    double c_alpha = r8_cos(alpha);

    /*
    if (DERIVATION_METHOD == 1) {
        DX = getLineDensity1d(ptr_dx,x,z);
        DZ = getLineDensity1d(ptr_dz,x,z);
        return;
    }
    */
    double xi = 0;
    double yi = 0;
    double zi = 0;
    double xm = 0;
    double ym = 0;
    double zm = 0;

    if (IMAGE_TYPE == IT_POINT2POINT) {
        if (FX == 0 && FZ == 0) {  // plane
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = X;
            ym = rosag * s_beta;
        } else {  // more general case, can be reduced to the plane with normal
                  // = (0,1,0) and y = 0
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = (FY * X - FX * Y + FX * risag * s_alpha);
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = FX * FZ * X + (FX * FX + FY * FY) * (-Z + rosag * c_beta) + FY * FZ * (Y - rosag * s_beta);
            xm = (FY * X - FX * Y + FX * rosag * s_beta);
            ym = -(FX * X) - FY * Y - FZ * Z + FZ * rosag * c_beta + FY * rosag * s_beta;
        }
    } else if (IMAGE_TYPE == IT_ASTIGMATIC2ASTIGMATIC) {
        double s_rim = sign(rimer);
        double s_rom = sign(romer);
        double c_2alpha = r8_cos(2 * alpha);
        double c_2beta = r8_cos(2 * beta);
        if (FX == 0 && FZ == 0) {  //   !plane

            zi = s_rim * (rimer * c_alpha + Z);
            xi = (s_rim * X * (c_alpha * Z - 2 * s_alpha * s_alpha * rimer + s_alpha * Y + rimer)) /
                 (c_alpha * Z - 2 * s_alpha * s_alpha * risag + s_alpha * Y + risag);
            yi = s_rim * (-rimer * s_alpha + Y);
            zm = s_rom * (romer * c_beta - Z);
            xm = (s_rom * X * (-c_beta * Z - 2 * s_beta * s_beta * romer + s_beta * Y + romer)) /
                 (c_beta * Z + 2 * s_beta * s_beta * rosag - s_beta * Y - rosag);
            ym = s_rom * (romer * s_beta - Y);
        } else {
            double denominator = Z * c_alpha + risag * c_2alpha + Y * s_alpha;
            double nominator = X * (Z * c_alpha + rimer * c_2alpha + Y * s_alpha);
            zi = s_rim * ((FX * FX + FY * FY) * (Z + rimer * c_alpha) - FY * FZ * (Y - rimer * s_alpha) - (FX * FZ * nominator) / denominator);
            xi = s_rim * (-(FX * Y) + FX * rimer * s_alpha + (FY * nominator) / denominator);
            yi = s_rim * (FZ * (Z + rimer * c_alpha) + FY * (Y - rimer * s_alpha) + (FX * nominator) / denominator);

            denominator = (-(Z * c_beta) + rosag * c_2beta + Y * s_beta);
            nominator = X * (-(Z * c_beta) + romer * c_2beta + Y * s_beta);
            zm = s_rom * ((FX * FX + FY * FY) * (-Z + romer * c_beta) + FY * FZ * (Y - romer * s_beta) + (FX * FZ * nominator) / denominator);
            xm = s_rom * (FX * (Y - romer * s_beta) - (FY * nominator) / denominator);
            ym = s_rom * (FZ * (-Z + romer * c_beta) + FY * (-Y + romer * s_beta) - (FX * nominator) / denominator);
        }
        double ris = sqrt(zi * zi + xi * xi + yi * yi);
        double rms = sqrt(zm * zm + xm * xm + ym * ym);

        double ai = zi / ris;
        double bi = -xi / ris;
        double am = -zm / rms;
        double bm = xm / rms;

        // double ci = yi/ris; // for what?
        // double cm = -ym/rms;

        DZ = (ai + am) / (WL * Ord);
        DX = (-bi - bm) / (WL * Ord);

        return;
    } else if (IMAGE_TYPE == IT_POINT2HORIZONTAL_LINE) {
        // TODO don't use magic constants

        if (FX == 0.0 && FZ == 0.0) {  // plane
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = 0;
            ym = rosag * s_beta;
        } else {
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = FY * X - FX * Y + FX * risag * s_alpha;
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = (FX * FX + FY * FY) * (-Z + rosag * c_beta) + FY * FZ * (Y - rosag * s_beta);
            xm = FX * (-Y + rosag * s_beta);
            ym = -(FY * Y) - FZ * Z + FZ * rosag * c_beta + FY * rosag * s_beta;
        }
    } else if (IMAGE_TYPE == IT_POINT2HORIZONTAL_DIVERGENT_LINE) {
        if (FX == 0.0 && FZ == 0.0) {
            zi = -(risag * c_alpha + Z);
            xi = X;
            yi = risag * s_alpha;
            zm = rosag * c_beta - Z;
            xm = X / zi * zm;  // not anymore negative as zi is negative!g
            ym = rosag * s_beta;
        } else {
            zi = FX * FZ * X - (FX * FX + FY * FY) * (Z + risag * c_alpha) + FY * FZ * (Y - risag * s_alpha);
            xi = FY * X - FX * Y + FX * risag * s_alpha;
            yi = -(FX * X) - FY * Y - FZ * Z - FZ * risag * c_alpha + FY * risag * s_alpha;
            zm = (FX * FZ * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha) + (FX * FX + FY * FY) * (-Z + rosag * c_beta) +
                 FY * FZ * (Y - rosag * s_beta);
            xm = (FY * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha) + FX * (-Y + rosag * s_beta);
            ym = -((FX * X * (Z - rosag * c_beta)) / (Z + risag * c_alpha)) + FZ * (-Z + rosag * c_beta) + FY * (-Y + rosag * s_beta);
        }
    } else {
        _throw("unsupported ImageType!");
    }

    double ris = sqrt(zi * zi + xi * xi + yi * yi);
    double rms = sqrt(zm * zm + xm * xm + ym * ym);

    double ai = xi / ris;
    double am = xm / rms;
    double bi = zi / ris;
    double bm = zm / rms;

    DX = (ai + am) / (WL * Ord);
    DZ = (-bi - bm) / (WL * Ord);

    return;
}

/**
calculates refracted ray
@params: 	r: ray
            normal: normal at intersection point of ray and element
            az: line spacing in z direction
            ax: line spacing in x direction
@returns: refracted ray (position unchanged, direction changed), weight = ETYPE_BEYOND_HORIZON if
"ray beyond horizon"
*/
Ray refrac2D(Ray r, dvec3 normal, double az, double ax) {
    double eps1 = -r8_atan(normal.x / normal.y);
    double del1 = r8_asin(normal.z);

    double cos_d = r8_cos(-del1);
    double sin_d = r8_sin(-del1);
    double cos_e = r8_cos(-eps1);
    double sin_e = r8_sin(-eps1);
    dmat3 rot = dmat3(cos_e, cos_d * sin_e, sin_d * sin_e, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, -sin_d, cos_d);
    dmat3 inv_rot = dmat3(cos_e, -sin_e, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, sin_d * sin_e, sin_d * cos_e, cos_d);
    r.m_direction = rot * r.m_direction;

    double x1 = r.m_direction.x - ax;
    double z1 = r.m_direction.z - az;
    double y1 = 1 - x1 * x1 - z1 * z1;

    if (y1 > 0) {
        y1 = sqrt(y1);

        r.m_direction.x = x1;
        r.m_direction.y = y1;
        r.m_direction.z = z1;
        r.m_direction = inv_rot * r.m_direction;
    } else {  // beyond horizon - when divergence too large
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON);
    }
    return r;
}

// refraction function used for gratings
Ray refrac(Ray r, dvec3 normal, double linedensity) {
    double xy = normal[0] / normal[1];
    double zy = normal[2] / normal[1];
    double sqq = sqrt(1 + zy * zy + xy * xy);
    double an_x = xy / sqq;
    double an_y = -1.0 / sqq;
    double an_z = zy / sqq;

    double eps1 = r8_atan(an_x / an_y);  //-atan(an_x/an_z) around z, chi
    double del1 = r8_asin(an_z);         // sign(an_z) * r8_atan(sqrt( (an_z*an_z) / (1-an_z*an_z) )); //
                                         // -asin(an_z); // -asin around x, psi
    double cos_d = r8_cos(del1);
    double a1 = linedensity * cos_d;
    double sin_d = r8_sin(-del1);
    double cos_e = r8_cos(-eps1);
    double sin_e = r8_sin(-eps1);
    dmat4 rot = dmat4(cos_e, cos_d * sin_e, sin_d * sin_e, 0, -sin_e, cos_d * cos_e, sin_d * cos_e, 0, 0, -sin_d, cos_d, 0, 0, 0, 0, 1);
    dmat4 inv_rot = dmat4(cos_e, -sin_e, 0, 0, cos_d * sin_e, cos_d * cos_e, -sin_d, 0, sin_d * sin_e, sin_d * cos_e, cos_d, 0, 0, 0, 0, 1);
    r.m_direction = dvec3(rot * dvec4(r.m_direction, 0));

    double y1 = (r.m_direction[1] * r.m_direction[1] + r.m_direction[2] * r.m_direction[2] - ((r.m_direction[2] - a1) * (r.m_direction[2] - a1)));
    if (y1 > 0) {
        y1 = sqrt(y1);
        r.m_direction[1] = y1;
        r.m_direction[2] -= a1;
        r.m_direction = dvec3(inv_rot * dvec4(r.m_direction, 0));
    } else {
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON);
    }
    return r;
}

/** variable line spacing for gratings
@params
    lineDensity: general line density?
    z: z-coordinate of ray position
    vls[6]: 6 vls parameters given by user
@returns line density specifically for this z-coordinate
*/
double RAYX_API vlsGrating(double lineDensity, double z, double vls[6]) {
    // lineDensity = lineDensity * (1 + 2*b2*z + 3*b3*z**2 + 4*b4*z**3 +
    // 5*b5*z**4 + 6*b6*z**5 + 7*b7*z**6)
    double z2 = z * z;
    double z3 = z2 * z;
    double z4 = z3 * z;
    double z5 = z4 * z;
    double z6 = z5 * z;
    double a = lineDensity * (1 + 2 * vls[0] * z + 3 * vls[1] * z2 + 4 * vls[2] * z3 + 5 * vls[3] * z4 + 6 * vls[4] * z5 + 7 * vls[5] * z6);
    return a;
}

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
                          double a) {  // TODO fix unused var
    double y1 = r.m_direction[1] * r.m_direction[1] + r.m_direction[2] * r.m_direction[2] - (r.m_direction[2] - a) * (r.m_direction[2] - a);
    if (y1 > 0) {
        y1 = sqrt(y1);
        r.m_direction[1] = y1;
        r.m_direction[2] = r.m_direction[2] - a;
    } else {
        recordFinalEvent(r, ETYPE_BEYOND_HORIZON);
    }
    return r;
}

/**returns first bessel function of parameter v*/
double RAYX_API bessel1(double v) {
    if (v < 0.0 || v > 20.0) {
        return 0.0;
    }

    double sum = 0;
    int large = 30;

    double PO1;
    double PO2;
    double FA1;
    for (int small = 0; small <= large; small++) {
        PO1 = dpow(-1.0, small);
        PO2 = dpow(v / 2.0, 2 * small + 1);
        FA1 = fact(small);
        sum += (PO1 / (FA1 * FA1 * (small + 1))) * PO2;
    }
    return sum;
}

/**
calculates the Bessel diffraction effects on circular slits and on circular
zoneplates
@params:	radius		radius < 0 (mm)
            wl			wavelength (nm)
            dphi, dpsi  angles of diffracted ray
@returns
    results stored in dphi, dpsi (inout)
*/
void bessel_diff(double radius, double wl, RAYX_INOUT(double) dphi, RAYX_INOUT(double) dpsi) {
    double b = abs(radius) * 1e06;
    double ximax = 5.0 * wl / b;

    double rn1[3];
    double c = -1;
    while (c < 0) {  // while c = wd - rn1[2] < 0 continue
        for (int i = 0; i < 3; i++) {
            rn1[i] = squaresDoubleRNG(inv_ctr);
        }

        dphi = rn1[0] * ximax;
        dpsi = rn1[1] * ximax;
        double xi = sqrt(0.5 * (dphi * dphi + dpsi * dpsi));
        double u = 2.0 * PI * b * xi / wl;
        double wd = 1;
        if (u != 0) {
            wd = 2.0 * bessel1(u) / u;
            wd = wd * wd;
        }
        c = wd - rn1[2];
    }

    // 50% neg/pos sign
    dphi = sign(squaresDoubleRNG(inv_ctr) - 0.5) * dphi;
    dpsi = sign(squaresDoubleRNG(inv_ctr) - 0.5) * dpsi;
}

/**
 * calculates fraunhofer diffraction effects on rectangular slits
 * @param dim		dimension (x or y) (mm)
 * @param wl			wavelength (nm)
 * @param dAngle 	diffraction angle (inout)
 * @return result stored in dAngle
 */
void fraun_diff(double dim, double wl, RAYX_INOUT(double) dAngle) {
    if (dim == 0) return;        // no diffraction in this direction
    double b = dim * 1e06;       // slit opening
    double div = 10.0 * wl / b;  // up to 2nd maximum

    double rna[2];  // 2 uniform random numbers in [0,1]
    double c = -1;
    while (c < 0) {  // while c = wd - uni[1] < 0 continue
        for (int i = 0; i < 2; i++) {
            rna[i] = squaresDoubleRNG(inv_ctr);
        }
        dAngle = (rna[0] - 0.5) * div;
        double u = PI * b * r8_sin(dAngle) / wl;
        double wd = 1;
        if (u != 0) {
            wd = r8_sin(u) / u;
            wd = wd * wd;
        }
        c = wd - rna[1];
    }
}


// Cutout helper functions:

// checks whether the point (x, z) is within the cutout.
bool RAYX_API inCutout(Cutout cutout, double x, double z) {
    if (cutout.m_type == CTYPE_UNLIMITED) {
        return true;
    } else if (cutout.m_type == CTYPE_RECT) {
        RectCutout rect = deserializeRect(cutout);
        double x_min = -rect.m_width / 2.0;
        double x_max = rect.m_width / 2.0;
        double z_min = -rect.m_length / 2.0;
        double z_max = rect.m_length / 2.0;

        return !(x <= x_min || x >= x_max || z <= z_min || z >= z_max);
    } else if (cutout.m_type == CTYPE_TRAPEZOID) {
        TrapezoidCutout t = deserializeTrapezoid(cutout);

        // Check point is within the trapezoid
        dvec2 P = dvec2(x, z);

        // A, B, C, D are the four points on the trapezoid.
        //
        //    A--B    //
        //   /    \   //
        //  C------D  //
        dvec2 A = dvec2(-t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 B = dvec2(t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 C = dvec2(t.m_widthB / 2.0, t.m_length / 2.0);
        dvec2 D = dvec2(-t.m_widthB / 2.0, t.m_length / 2.0);

        dvec2 PmA = P - A;
        dvec2 BmA = B - A;
        dvec2 PmD = P - D;
        dvec2 CmD = C - D;
        dvec2 DmA = D - A;
        dvec2 PmB = P - B;
        dvec2 CmB = C - B;

        double l1 = (PmA.x * BmA.y - PmA.y * BmA.x) * (PmD.x * CmD.y - PmD.y * CmD.x);
        double l2 = (PmA.x * DmA.y - PmA.y * DmA.x) * (PmB.x * CmB.y - PmB.y * CmB.x);
        return l1 < 0 && l2 < 0;
    } else if (cutout.m_type == CTYPE_ELLIPTICAL) {
        EllipticalCutout ell = deserializeElliptical(cutout);

        double radius_x = ell.m_diameter_x / 2.0;
        double radius_z = ell.m_diameter_z / 2.0;

        double val1 = x / radius_x;
        double val2 = z / radius_z;

        double rd2 = val1 * val1 + val2 * val2;
        return rd2 <= 1.0;
    } else {
        _throw("invalid cutout type in inCutout!");
        return false;
    }
}

// returns a matrix M where (M[i].x, M[i].z) are the key points of our cutout.
// The key points are typically points on the boundary of the cutout.
dmat4 RAYX_API keyCutoutPoints(Cutout cutout) {
    dmat4 ret;
    double w = 0;
    double l = 0;
    if (cutout.m_type == CTYPE_UNLIMITED) {
        double inf = 1e100;
        w = inf;
        l = inf;
    } else if (cutout.m_type == CTYPE_RECT) {
        RectCutout rect = deserializeRect(cutout);
        w = rect.m_width/2.0;
        l = rect.m_length/2.0;
    } else if (cutout.m_type == CTYPE_TRAPEZOID) {
        TrapezoidCutout t = deserializeTrapezoid(cutout);

        dvec2 A = dvec2(-t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 B = dvec2(t.m_widthA / 2.0, -t.m_length / 2.0);
        dvec2 C = dvec2(t.m_widthB / 2.0, t.m_length / 2.0);
        dvec2 D = dvec2(-t.m_widthB / 2.0, t.m_length / 2.0);

        ret[0] = dvec4(A[0], 0.0, A[1], 0.0);
        ret[1] = dvec4(B[0], 0.0, B[1], 0.0);
        ret[2] = dvec4(C[0], 0.0, C[1], 0.0);
        ret[3] = dvec4(D[0], 0.0, D[1], 0.0);
        return ret;
    } else if (cutout.m_type == CTYPE_ELLIPTICAL) {
        EllipticalCutout ell = deserializeElliptical(cutout);
        double rx = ell.m_diameter_x / 2.0;
        double rz = ell.m_diameter_z / 2.0;
        ret[0] = dvec4( rx, 0.0, 0.0, 0.0);
        ret[1] = dvec4(0.0, 0.0,  rz, 0.0);
        ret[2] = dvec4(-rx, 0.0, 0.0, 0.0);
        ret[3] = dvec4(0.0, 0.0, -rz, 0.0);
        return ret;
    } else {
        _throw("invalid cutout type in inCutout!");
    }
    ret[0] = dvec4( w, 0.0,  l, 0.0);
    ret[1] = dvec4(-w, 0.0, -l, 0.0);
    ret[2] = dvec4(-w, 0.0,  l, 0.0);
    ret[3] = dvec4( w, 0.0, -l, 0.0);
    return ret;
}

// returns width and length of the bounding box.
dvec2 RAYX_API cutoutBoundingBox(Cutout cutout) {
    dvec2 ret = dvec2(0.0, 0.0);
    dmat4 keypoints = keyCutoutPoints(cutout);
    for (int i = 0; i < 4; i++) {
        double x = abs(keypoints[i][0]) * 2.0;
        double z = abs(keypoints[i][2]) * 2.0;
        if (x > ret[0]) ret[0] = x;
        if (z > ret[1]) ret[1] = z;
    }
    return ret;
}

// checks whether c1 is a subset of c2, and prints an error otherwise.
// might not find all subset-violations, but should find most of them.
// (might not find all Ellipsoid vs Trapezoid violations)
void RAYX_API assertCutoutSubset(Cutout c1, Cutout c2) {
    dmat4 keypoints = keyCutoutPoints(c1);
    for (int i = 0; i < 4; i++) {
        double x = keypoints[i][0];
        double z = keypoints[i][2];
        if (!inCutout(c2, x, z)) {
            _throw("assertCutoutSubset failed!");
        }
    }
}
