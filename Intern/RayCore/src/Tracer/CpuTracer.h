#pragma once

#include "Core.h"
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

    std::vector<Ray> traceRaw(const TraceRawConfig&) override;
};

namespace CPU_TRACER {
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using dmat3 = glm::dmat3;
using dmat4 = glm::dmat4;
using uint = unsigned int;

struct Element {
    dmat4 surfaceParams;
    dmat4 inTrans;
    dmat4 outTrans;
    dmat4 objectParameters;
    dmat4 elementParameters;
};

struct PalikEntry {
    double energy;
    double n;
    double k;
};

struct NffEntry {
    double energy;
    double f1;
    double f2;
};

// this type intends to mimic the GLSL type T[], this is used for layouts.
template <typename T>
struct ShaderArray {
  public:
    std::vector<T> data;

    inline int length() { return data.size(); }
    inline T& operator[](int i) { return data[i]; }
};

extern int gl_GlobalInvocationID;
extern ShaderArray<Ray> rayData;
extern ShaderArray<Ray> outputData;
extern double rayIdStart;
extern double numRays;
extern double randomSeed;
extern double maxSnapshots;
extern ShaderArray<Element> quadricData;
extern ShaderArray<dvec4> xyznull;
extern RAYX_API ShaderArray<int> matIdx;
extern RAYX_API ShaderArray<double> mat;

inline double length(dvec2 v) { return sqrt(v.x * v.x + v.y * v.y); }
inline double mod(double x, double y) { return glm::mod(x, y); }
inline double sign(double x) { return glm::sign(x); }

// forward declarations for shader functions

double RAYX_API r8_exp(double);
double RAYX_API r8_log(double);
double RAYX_API squaresDoubleRNG(uint64_t&);
Ray RAYX_API refrac2D(Ray, glm::dvec4, double, double);
Ray RAYX_API refrac(Ray, glm::dvec4, double);
glm::dvec4 RAYX_API normal_cartesian(glm::dvec4, double, double);
glm::dvec4 RAYX_API normal_cylindrical(glm::dvec4, double, double);
bool RAYX_API wasteBox(double, double, double, double);
void RAYX_API RZPLineDensity(Ray r, glm::dvec4 normal, int IMAGE_TYPE, int RZP_TYPE, int DERIVATION_METHOD, double zOffsetCenter, double risag,
                             double rosag, double rimer, double romer, double alpha, double beta, double Ord, double WL, double& DX, double& DZ);
Ray RAYX_API rayMatrixMult(Ray, glm::dmat4);
void RAYX_API cosini(Ray&, double, double);
double RAYX_API dpow(double, int);
double RAYX_API fact(int);
double RAYX_API bessel1(double);
double RAYX_API r8_sin(double);
double RAYX_API r8_cos(double);
double RAYX_API r8_atan(double);
double RAYX_API vlsGrating(double, double, double[6]);
void RAYX_API diffraction(int iopt, double xLength, double yLength, double wl, double& dPhi, double& dPsi, uint64_t& ctr);
Ray RAYX_API refrac_plane(Ray, glm::dvec4, double);
glm::dvec4 RAYX_API iteratTo(Ray& r, double longRadius, double shortRadius);
double RAYX_API getIncidenceAngle(Ray r, glm::dvec4 normal);
void RAYX_API reflectance(double energy, double incidence_angle, glm::dvec2& complex_S, glm::dvec2& complex_P, int material);
glm::dvec2 RAYX_API snell(glm::dvec2 cos_incidence, glm::dvec2 cn1, glm::dvec2 cn2);
void RAYX_API fresnel(glm::dvec2 cn1, glm::dvec2 cn2, glm::dvec2 cos_incidence, glm::dvec2 cos_transmittance, glm::dvec2& complex_S,
                      glm::dvec2& complex_P);
glm::dvec2 RAYX_API cartesian_to_euler(glm::dvec2 complex);
double RAYX_API hvlam(double);

dvec2 RAYX_API getAtomicMassAndRho(int material);
int RAYX_API getPalikEntryCount(int material);
PalikEntry RAYX_API getPalikEntry(int index, int material);
int RAYX_API getNffEntryCount(int material);
NffEntry RAYX_API getNffEntry(int index, int material);
int RAYX_API getPalikEntryCount(int material);
PalikEntry RAYX_API getPalikEntry(int index, int material);
dvec2 RAYX_API getRefractiveIndex(double energy, int material);
}  // namespace CPU_TRACER

}  // namespace RAYX
