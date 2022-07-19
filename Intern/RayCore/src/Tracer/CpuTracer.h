#pragma once

#include "Core.h"
#include "Tracer/RayList.h"
#include "Tracer/Tracer.h"

namespace RAYX {
/**
 * @brief The CPU tracer can replace the Vulkan tracer to run all shader compute
 * locally on CPU and RAM. |CPU| --> |SHADER(OnCPU)|--> |CPU|
 *
 */
class RAYX_API CpuTracer : public Tracer {
  public:
    /**
     * @brief Constructs a new *CPU* Tracer object that utlizes the compute
     * shader code.
     *
     */
    CpuTracer();
    ~CpuTracer();

    RayList trace(const Beamline&) override;
    void* getDebugList() override;
};

namespace CPU_TRACER {
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using dmat3 = glm::dmat3;
using dmat4 = glm::dmat4;
using uint = unsigned int;

#ifdef RAYX_DEBUG_MODE
struct _debug_struct {
    dmat4 _dMat;  // Can also be used as vectors or scalar
};
#endif

struct Element {
    dmat4 surfaceParams;
    dmat4 inTrans;
    dmat4 outTrans;
    dmat4 objectParameters;
    dmat4 elementParameters;
};

// this type intends to mimic the GLSL type T[], this is used for layouts.
template <typename T>
struct ShaderArray {
    std::vector<T> data;

    inline int length() { return data.size(); }
    inline T& operator[](int i) { return data[i]; }
};

extern int gl_GlobalInvocationID;
extern ShaderArray<Ray> rayData;
extern ShaderArray<Ray> outputData;
extern double numberOfBeamlines;
extern double numberOfElementsPerBeamline;
extern double numberOfRays;
extern double numberOfRaysPerBeamLine;
extern ShaderArray<Element> quadricData;
extern ShaderArray<dvec4> xyznull;
extern ShaderArray<int> matIdx;
extern ShaderArray<double> mat;
#ifdef RAYX_DEBUG_MODE
extern ShaderArray<_debug_struct> d_struct;
#endif

inline double length(dvec2 v) { return sqrt(v.x * v.x + v.y * v.y); }
inline double mod(double x, double y) { return glm::mod(x, y); }
inline double sign(double x) { return glm::sign(x); }

// forward declarations for shader functions

double r8_exp(double);
double r8_log(double);
double squaresDoubleRNG(uint64_t&);
Ray refrac2D(Ray, glm::dvec4, double, double);
Ray refrac(Ray, glm::dvec4, double);
glm::dvec4 normal_cartesian(glm::dvec4, double, double);
glm::dvec4 normal_cylindrical(glm::dvec4, double, double);
double wasteBox(double, double, double, double, double);
void RZPLineDensity(Ray r, glm::dvec4 normal, int IMAGE_TYPE, int RZP_TYPE,
                    int DERIVATION_METHOD, double zOffsetCenter, double risag,
                    double rosag, double rimer, double romer, double alpha,
                    double beta, double Ord, double WL, double& DX, double& DZ);
Ray rayMatrixMult(Ray, glm::dmat4);
void cosini(Ray&, double, double);
double dpow(double, int);
double fact(int);
double bessel1(double);
double r8_sin(double);
double r8_cos(double);
double r8_atan(double);
double vlsGrating(double, double, double[6]);
void diffraction(int iopt, double xLength, double yLength, double wl,
                 double& dPhi, double& dPsi, uint64_t& ctr);
Ray refrac_plane(Ray, glm::dvec4, double);
glm::dvec4 iteratTo(Ray& r, double longRadius, double shortRadius);
double getIncidenceAngle(Ray r, glm::dvec4 normal);
void reflectance(double energy, double incidence_angle, glm::dvec2& complex_S,
                 glm::dvec2& complex_P, int material);
glm::dvec2 snell(glm::dvec2 cos_incidence, glm::dvec2 cn1, glm::dvec2 cn2);
void fresnel(glm::dvec2 cn1, glm::dvec2 cn2, glm::dvec2 cos_incidence,
             glm::dvec2 cos_transmittance, glm::dvec2& complex_S,
             glm::dvec2& complex_P);
glm::dvec2 cartesian_to_euler(glm::dvec2 complex);
}  // namespace CPU_TRACER

}  // namespace RAYX
