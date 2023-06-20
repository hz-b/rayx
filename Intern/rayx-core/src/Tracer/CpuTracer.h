#pragma once

#include "../Shared/NffPalik.h"
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
    void setPushConstants(const PushConstants*) override;
};

namespace CPU_TRACER {
// TODO can this be merged with adapt.h somehow?
using uint = unsigned int;

using pushConstants_t = CpuTracer::PushConstants;
extern pushConstants_t pushConstants;

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
Ray RAYX_API refrac2D(Ray, glm::dvec4, double, double);
Ray RAYX_API refrac(Ray, glm::dvec4, double);
glm::dvec4 RAYX_API normal_cartesian(glm::dvec4, double, double);
glm::dvec4 RAYX_API normal_cylindrical(glm::dvec4, double, double);
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
Ray RAYX_API refrac_plane(Ray, glm::dvec4, double);
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
