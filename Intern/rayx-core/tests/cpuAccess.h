// This file allows you to access the inner functions of the CpuTracer in order to test them.

namespace RAYX {
namespace CPU_TRACER {

#include "../src/Shared/NffPalik.h"

using uint = unsigned int;

extern PushConstants pushConstants;

extern int gl_GlobalInvocationID;
extern ShaderArray<Ray> rayData;
extern ShaderArray<Ray> outputData;

extern ShaderArray<Element> quadricData;
extern ShaderArray<vec4> xyznull;
extern RAYX_API ShaderArray<int> matIdx;
extern RAYX_API ShaderArray<float> mat;

// forward declarations for shader functions

float RAYX_API r8_exp(float);
float RAYX_API r8_log(float);
float RAYX_API squaresDoubleRNG(uint64_t&);
float RAYX_API squaresNormalRNG(uint64_t&, float, float);
Ray RAYX_API refrac2D(Ray, glm::vec4, float, float);
Ray RAYX_API refrac(Ray, glm::vec4, float);
glm::vec4 RAYX_API normal_cartesian(glm::vec4, float, float);
glm::vec4 RAYX_API normal_cylindrical(glm::vec4, float, float);
void RAYX_API RZPLineDensity(Ray r, glm::vec4 normal, RZPBehaviour b, float& DX, float& DZ);
Ray RAYX_API rayMatrixMult(Ray, glm::mat4);
void RAYX_API cosini(Ray&, float, float);
float RAYX_API dpow(float, int);
float RAYX_API fact(int);
float RAYX_API bessel1(float);
float RAYX_API r8_sin(float);
float RAYX_API r8_cos(float);
float RAYX_API r8_atan(float);
float RAYX_API vlsGrating(float, float, float[6]);
Ray RAYX_API refrac_plane(Ray, glm::vec4, float);
float RAYX_API getIncidenceAngle(Ray r, glm::vec4 normal);
void RAYX_API reflectance(float energy, float incidence_angle, glm::vec2& complex_S, glm::vec2& complex_P, int material);
glm::vec2 RAYX_API snell(glm::vec2 cos_incidence, glm::vec2 cn1, glm::vec2 cn2);
void RAYX_API fresnel(glm::vec2 cn1, glm::vec2 cn2, glm::vec2 cos_incidence, glm::vec2 cos_transmittance, glm::vec2& complex_S,
                      glm::vec2& complex_P);
glm::vec2 RAYX_API cartesian_to_euler(glm::vec2 complex);
float RAYX_API hvlam(float);

vec2 RAYX_API getAtomicMassAndRho(int material);
int RAYX_API getPalikEntryCount(int material);
PalikEntry RAYX_API getPalikEntry(int index, int material);
int RAYX_API getNffEntryCount(int material);
NffEntry RAYX_API getNffEntry(int index, int material);
int RAYX_API getPalikEntryCount(int material);
PalikEntry RAYX_API getPalikEntry(int index, int material);
vec2 RAYX_API getRefractiveIndex(float energy, int material);
}  // namespace CPU_TRACER
}  // namespace RAYX
