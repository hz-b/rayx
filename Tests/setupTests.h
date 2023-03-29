#pragma once

#include <fstream>
#include <functional>
#include <sstream>
#include <type_traits>

#include "CanonicalizePath.h"
#include "Core.h"
#include "Data/Importer.h"
#include "Debug/Debug.h"
#include "Material/Material.h"
#include "Beamline/Objects/Objects.h"
#include "Shared/Constants.h"
#include "Tracer/CpuTracer.h"
#include "Tracer/Ray.h"
#include "Tracer/VulkanTracer.h"
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

/// this is the underlying implementation of the CHECK_EQ macro.
/// asserts that tl and tr are the same up to a given tolerance, and give a fancy print if they mismatch.
/// filename, line represents where CHECK_EQ is called.
/// l and r are the printable strings representing tl and tr.
/// vl and vr represent the doubles contained in tl and tr, obtained with RAYX::formatAsVec
template <typename TL, typename TR>
inline void checkEq(std::string filename, int line, std::string l, std::string r, const TL& tl, const TR& tr, std::vector<double> vl,
                    std::vector<double> vr, double tolerance = 1e-10) {
    if (vl.size() != vr.size()) {
        RAYX::Err(filename, line) << l << " != " << r << ": different lengths!";
        return;
    }

    bool success = true;
    for (size_t i = 0; i < vl.size(); i++) {
        if (abs(vl[i] - vr[i]) > tolerance) {
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
    std::vector<std::string> names = {".m_position.x",  ".m_position.y", ".m_position.z",  ".m_weight",    ".m_direction.x", ".m_direction.y",
                                      ".m_direction.z", ".m_energy",     ".m_stokes.x",    ".m_stokes.y",  ".m_stokes.z",    ".m_stokes.w",
                                      ".m_pathLength",  ".m_order",      ".m_lastElement", ".m_extraParam"};
    for (int i = 0; i < 14; i++) {
        auto t = tolerance;

        // integer tolerance for integer values.
        if (i == 3 /*weight*/ || i == 14 /*lastElem*/ || i == 15 /*extraParam*/) {
            t = 0.5;
        }

        checkEq(filename, line, l + names[i], r + names[i], vl[i], vr[i], {vl[i]}, {vr[i]}, t);
    }
}

/// check that L and R contain the same doubles.
// TODO check_eq calls L and R multiple times if they are expressions with side-effects!
#define CHECK_EQ(L, R, ...) \
    checkEq(__FILE__, __LINE__, #L, #R, L, R, RAYX::formatAsVec(L), RAYX::formatAsVec(R), ##__VA_ARGS__)  // __VA_ARGS__ = tolerance or nothing

/// assert that x holds, and give a fancy print otherwise.
#define CHECK(x)                                      \
    {                                                 \
        if (!(x)) {                                   \
            RAYX_ERR << "CHECK(" << #x << ") failed"; \
        }                                             \
    }

/// check whether low <= expr <= high
#define CHECK_IN(expr, low, high)                                                       \
    {                                                                                   \
        auto res = expr;                                                                \
        auto res_low = low;                                                             \
        if (res < res_low) {                                                            \
            RAYX_ERR << "CHECK_IN failed: " << #expr << " (" << res << ") < " << #low;  \
        }                                                                               \
        auto res_high = high;                                                           \
        if (res > res_high) {                                                           \
            RAYX_ERR << "CHECK_IN failed: " << #expr << " (" << res << ") > " << #high; \
        }                                                                               \
    }

/// used to check equalities, where doubles contain integer values
/// i.e. weight == 1, or extraParam == 21.
inline bool intclose(double x, double y) { return abs(x - y) < 0.5; }

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

        if (cpu) {
            tracer = std::make_unique<RAYX::CpuTracer>();
        } else {
#ifdef NO_VULKAN
            RAYX_ERR << "can't create VulkanTracer due to NO_VULKAN";
#else
            tracer = std::make_unique<RAYX::VulkanTracer>();
#endif
        }
    }
    virtual void SetUp() {}
    static void TearDownTestSuite() { tracer = nullptr; }
};

// helper functions for writing tests

/// will look at Tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename);

/// will write to Tests/output/<filename>.csv
void writeToOutputCSV(const RAYX::Rays& rays, std::string filename);

/// Returns all traced rays
RAYX::Rays traceRML(std::string filename);

// extracts the last W_JUST_HIT_ELEM for each ray.
std::vector<RAYX::Ray> extractLastHit(const RAYX::Rays&);

/// will look at Tests/input/<filename>.csv
/// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
/// element coordinates of the relevant element!)
std::vector<RAYX::Ray> loadCSVRayUI(std::string filename);

/// Checks for equality up to the tolerance `t`.
void compareRays(const RAYX::Rays& r1, const RAYX::Rays& r2, double t = 1e-11);

/// Only cares for the rays hitting the last object of the beamline, and check whether they are the same as their RayUI counter part.
/// Ray UI rays are obtained Export > RawRaysOutgoing.
/// This also filters out non-sequential rays to compare to Ray-UI correctly.
void compareLastAgainstRayUI(std::string filename, double t = 1e-11);

// compares input/<filename>.correct.csv with the trace output.
void compareAgainstCorrect(std::string filename, double t = 1e-11);

/// updates the material tables of the Cpu Tracer to contain exactly the
/// materials given in the std::vector.
void updateCpuTracerMaterialTables(std::vector<Material>);
