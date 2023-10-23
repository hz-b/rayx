// This file allows you to access the inner functions of the CpuTracer in order to test them.

namespace RAYX::CPU_TRACER {

#include "../src/Shared/NffPalik.h"

using uint = unsigned int;

extern PushConstants pushConstants;

extern int gl_GlobalInvocationID;
extern ShaderArray<Ray> rayData;
extern ShaderArray<Ray> outputData;

extern ShaderArray<Element> quadricData;
extern ShaderArray<dvec4> xyznull;
extern RAYX_API ShaderArray<int> matIdx;
extern RAYX_API ShaderArray<double> mat;

// forward declarations for shader functions

double RAYX_API r8_exp(double);
double RAYX_API r8_log(double);
double RAYX_API squaresDoubleRNG(uint64_t&);
double RAYX_API squaresNormalRNG(uint64_t&, double, double);
Ray RAYX_API refrac2D(Ray, glm::dvec3, double, double);
Ray RAYX_API refrac(Ray, glm::dvec3, double);
glm::dvec3 RAYX_API normal_cartesian(glm::dvec3, double, double);
glm::dvec3 RAYX_API normal_cylindrical(glm::dvec3, double, double);
void RAYX_API RZPLineDensity(Ray r, glm::dvec3 normal, RZPBehaviour b, double& DX, double& DZ);
Ray RAYX_API rayMatrixMult(Ray, glm::dmat4);
double RAYX_API dpow(double, int);
double RAYX_API fact(int);
double RAYX_API bessel1(double);
double RAYX_API r8_sin(double);
double RAYX_API r8_cos(double);
double RAYX_API r8_atan(double);
double RAYX_API vlsGrating(double, double, double[6]);
Ray RAYX_API refrac_plane(Ray, glm::dvec3, double);
double RAYX_API getIncidenceAngle(Ray r, glm::dvec3 normal);
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
dvec2 RAYX_API getRefractiveIndex(double energy, int material);
}  // namespace RAYX::CPU_TRACER
