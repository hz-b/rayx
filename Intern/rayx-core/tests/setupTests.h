#pragma once

#include <fstream>
#include <functional>
#include <sstream>
#include <type_traits>

#include "Beamline/Objects/Objects.h"
#include "CanonicalizePath.h"
#include "Core.h"
#include "Data/Importer.h"
#include "Debug/Debug.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Material/Material.h"
#include "Random.h"
#include "Shader/Constants.h"
#include "Shader/Diffraction.h"
#include "Shader/Efficiency.h"
#include "Shader/Ray.h"
#include "Shader/RefractiveIndex.h"
#include "Writer/CSVWriter.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace RAYX;

extern int GLOBAL_ARGC;
extern char** GLOBAL_ARGV;

// CHECK_EQ

/* usage:
    CHECK_EQ(A, B, 1e-10); // with explicit tolerance
    CHECK_EQ(A, B);        // with default tolerance
*/

const int PREC = 17;

// declare invocation state globally
// TODO(Sven): do we really need invocation state here, or just material tables individually?
extern InvState inv;

/// this is the underlying implementation of the CHECK_EQ macro.
/// asserts that tl and tr are the same up to a given tolerance, and give a fancy print if they mismatch.
/// filename, line represents where CHECK_EQ is called.
/// l and r are the printable strings representing tl and tr.
/// vl and vr represent the doubles contained in tl and tr, obtained with RAYX::formatAsVec
template <typename TL, typename TR>
inline void checkEq(std::string filename, int line, std::string l, std::string r, const TL& tl, const TR& tr, std::vector<double> vl,
                    std::vector<double> vr, double tolerance = 1e-10) {
    if (vl.size() != vr.size()) {
        RAYX::Exit(filename, line) << l << " != " << r << ": different lengths!";
        return;
    }

    bool success = true;
    for (size_t i = 0; i < vl.size(); i++) {
        const bool xor_nan = std::isnan(vl[i]) != std::isnan(vr[i]);
        if (abs(vl[i] - vr[i]) > tolerance || xor_nan) {
            success = false;
            break;
        }
    }
    if (success) return;

    RAYX::Warn(filename, line) << l << " != " << r << ":";

    int counter = 0;  // stores the number of elements in the stringstream
    std::stringstream s;
    for (size_t i = 0; i < vl.size(); i++) {
        if (counter != 0) {
            s << " ";
        }
        if (abs(vl[i] - vr[i]) <= tolerance) {
            s << std::setprecision(PREC) << vl[i] << "|" << vr[i];
        } else {
            s << "\x1B[36m" << std::setprecision(PREC) << vl[i] << "|" << vr[i] << "\x1B[31m";
        }

        counter++;
        if (counter == 4 && vl.size() == 16) {  // 4x4 things should be written in 4 rows
            counter = 0;
            RAYX::Warn(filename, line) << s.str();
            s = std::stringstream();
        }
    }
    if (counter > 0) {
        RAYX::Warn(filename, line) << s.str();
    }
    ADD_FAILURE();
}

/// specialized handling for rays, better prints!
template <>
inline void checkEq(std::string filename, int line, std::string l, std::string r, const RAYX::Ray& tl, const RAYX::Ray& tr, std::vector<double> vl,
                    std::vector<double> vr, double tolerance) {
    std::vector<std::string> names = {".m_position.x",   ".m_position.y",   ".m_position.z",   ".m_eventType",    ".m_direction.x",
                                      ".m_direction.y",  ".m_direction.z",  ".m_energy",       ".m_field.x.real", ".m_field.x.imag",
                                      ".m_field.y.real", ".m_field.y.imag", ".m_field.z.real", ".m_field.z.imag", ".m_pathLength",
                                      ".m_order",        ".m_lastElement",  ".m_sourceID"};
    for (int i = 0; i < names.size(); i++) {
        auto t = tolerance;

        // integer tolerance for integer values.
        if (i == 3 /*eventType*/ || i == 14 /*lastElem*/) {
            t = 0.5;
        }

        checkEq(filename, line, l + names[i], r + names[i], vl[i], vr[i], {vl[i]}, {vr[i]}, t);
    }
}

/// check that L and R contain the same doubles.
// within CHECK_EQ: the __VA_ARGS__ argument is either `double tolerance` or nothing.
// all variables declared within CHECK_EQ end with `_check_eq` distinguish them from the variables that the user might write.
#define CHECK_EQ(L, R, ...)                                                                                                                       \
    {                                                                                                                                             \
        auto l_check_eq = L;                                                                                                                      \
        auto r_check_eq = R;                                                                                                                      \
        checkEq(__FILE__, __LINE__, #L, #R, l_check_eq, r_check_eq, RAYX::formatAsVec(l_check_eq), RAYX::formatAsVec(r_check_eq), ##__VA_ARGS__); \
    }

/// assert that x holds, and give a fancy print otherwise.
#define CHECK(x)                                       \
    {                                                  \
        if (!(x)) {                                    \
            RAYX_EXIT << "CHECK(" << #x << ") failed"; \
        }                                              \
    }

/// check whether low <= expr <= high
#define CHECK_IN(expr, low, high)                                                                  \
    {                                                                                              \
        auto expr_check_in = expr;                                                                 \
        auto low_check_in = low;                                                                   \
        if (expr_check_in < low_check_in) {                                                        \
            RAYX_EXIT << "CHECK_IN failed: " << #expr << " (" << expr_check_in << ") < " << #low;  \
        }                                                                                          \
        auto high_check_in = high;                                                                 \
        if (expr_check_in > high_check_in) {                                                       \
            RAYX_EXIT << "CHECK_IN failed: " << #expr << " (" << expr_check_in << ") > " << #high; \
        }                                                                                          \
    }

// ShaderTest

extern std::unique_ptr<RAYX::Tracer> tracer;

inline void add_failure() { ADD_FAILURE(); }

class TestSuite : public testing::Test {
  protected:
    static void SetUpTestSuite() {
        RAYX::error_fn = add_failure;

        bool cpu = false;
        for (int i = 1; i < GLOBAL_ARGC; i++) {
            if (strcmp(GLOBAL_ARGV[i], "-x") == 0 || strcmp(GLOBAL_ARGV[i], "--cpu") == 0) {
                cpu = true;
                break;
            }
        }

        // Choose Hardware
        using DeviceType = RAYX::DeviceConfig::DeviceType;
        const auto deviceType = cpu ? DeviceType::Cpu : DeviceType::Gpu;
        tracer = std::make_unique<RAYX::Tracer>(RAYX::DeviceConfig(deviceType).enableBestDevice());
    }

    // called before every test invocation.
    void SetUp() override { RAYX::fixSeed(RAYX::FIXED_SEED); }

    static void TearDownTestSuite() { tracer = nullptr; }
};

// helper functions for writing tests

/// will look at Tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename);

/// will write to Tests/output/<filename>.csv
void writeToOutputCSV(const RAYX::BundleHistory& hist, std::string filename);

/// Returns all traced rays
RAYX::BundleHistory traceRML(std::string filename);

// extracts the last EventType::HitElement for each ray.
std::vector<RAYX::Ray> extractLastHit(const RAYX::BundleHistory&);

/// will look at Tests/input/<filename>.csv
/// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
/// element coordinates of the relevant element!)
std::vector<RAYX::Ray> loadCSVRayUI(std::string filename);

/// Checks for equality up to the tolerance `t`.
void compareBundleHistories(const RAYX::BundleHistory& r1, const RAYX::BundleHistory& r2, double t = 1e-11);

// If the ray from `ray_hist` went through the whole beamline sequentially, we return its last hit event.
// Otherwise, we return `{}`, aka None.
std::optional<RAYX::Ray> lastSequentialHit(RayHistory ray_hist, uint32_t beamline_len);

/// Only cares for the rays hitting the last object of the beamline, and check whether they are the same as their RayUI counter part.
/// Ray UI rays are obtained Export > RawRaysOutgoing.
/// This also filters out non-sequential rays to compare to Ray-UI correctly.
void compareLastAgainstRayUI(std::string filename, double tolerance = 1e-4, Sequential seq = Sequential::No);

// compares input/<filename>.correct.csv with the trace output.
void compareAgainstCorrect(std::string filename, double tolerance = 1e-11);

/// updates the material tables of the Cpu Tracer to contain exactly the
/// materials given in the std::vector.
void updateCpuTracerMaterialTables(std::vector<Material>);
