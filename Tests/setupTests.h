#pragma once

#include <fstream>
#include <functional>
#include <sstream>
#include <type_traits>

#include "Core.h"
#include "Data/Importer.h"
#include "Debug.h"
#include "Material/Material.h"
#include "Model/Beamline/Objects/Objects.h"
#include "PathResolver.h"
#include "Tracer/CpuTracer.h"
#include "Tracer/Ray.h"
#include "Tracer/RayList.h"
#include "Tracer/VulkanTracer.h"
#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"
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

template <typename TL, typename TR>
inline void checkEq(std::string filename, int line, std::string l,
                    std::string r, const TL& tl, const TR& tr,
                    std::vector<double> vl, std::vector<double> vr,
                    double tolerance = 1e-10) {
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
            s << "\x1B[36m" << std::setprecision(PREC) << vl[i] << "|" << vr[i]
              << "\x1B[31m";
        }

        counter++;
        if (counter == 4 &&
            vl.size() == 16) {  // 4x4 things should be written in 4 rows
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

// specialized handling for rays, better prints!
template <>
inline void checkEq(std::string filename, int line, std::string l,
                    std::string r, const RAYX::Ray& tl, const RAYX::Ray& tr,
                    std::vector<double> vl, std::vector<double> vr,
                    double tolerance) {
    std::vector<char*> names = {
        ".m_position.x",  ".m_position.y",  ".m_position.z",  ".m_weight",
        ".m_direction.x", ".m_direction.y", ".m_direction.z", ".m_energy",
        ".m_stokes.x",    ".m_stokes.y",    ".m_stokey.z",    ".m_stokes.w",
        ".m_pathLength",  ".m_order",       ".m_lastElement", ".m_extraParam"};
    for (int i = 0; i < 16; i++) {
        checkEq(filename, line, l + names[i], r + names[i], vl[i], vr[i],
                {vl[i]}, {vr[i]}, tolerance);
    }
}

#define CHECK_EQ(L, R, ...)                                         \
    checkEq(__FILE__, __LINE__, #L, #R, L, R, RAYX::formatAsVec(L), \
            RAYX::formatAsVec(R),                                   \
            ##__VA_ARGS__)  // __VA_ARGS__ = tolerance or nothing

#define CHECK(x)                                      \
    {                                                 \
        if (!(x)) {                                   \
            RAYX_ERR << "CHECK(" << #x << ") failed"; \
        }                                             \
    }

#define CHECK_IN(expr, low, high)                                             \
    {                                                                         \
        auto res = expr;                                                      \
        auto res_low = low;                                                   \
        if (res < res_low) {                                                  \
            RAYX_ERR << "CHECK_IN failed: " << #expr << " (" << res << ") < " \
                     << #low;                                                 \
        }                                                                     \
        auto res_high = high;                                                 \
        if (res > res_high) {                                                 \
            RAYX_ERR << "CHECK_IN failed: " << #expr << " (" << res << ") > " \
                     << #high;                                                \
        }                                                                     \
    }

// used to check equalities, where doubles contain integer values
// i.e. weight == 1, or extraParam == 21.
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
            if (strcmp(GLOBAL_ARGV[i], "-x") == 0 ||
                strcmp(GLOBAL_ARGV[i], "--cpu") == 0) {
                cpu = true;
                break;
            }
        }

        if (cpu) {
            tracer = std::make_unique<RAYX::CpuTracer>();
        } else {
            tracer = std::make_unique<RAYX::VulkanTracer>();
        }
    }
    virtual void SetUp() {}
    static void TearDownTestSuite() { tracer = nullptr; }
};

// helper functions for writing tests

RAYX::Ray parseCSVline(std::string line);

// will look at Tests/input/<filename>.rml
RAYX::Beamline loadBeamline(std::string filename);

// will write to Tests/output/<filename>.csv
void writeToOutputCSV(RAYX::RayList& rays, std::string filename);

enum class Filter { KeepAllRays, OnlySequentialRays };

// returns rays in element coordinates
// weight = 0 rays are filtered out.
// if filter == OnlySequentialRays, then only the sequential rays are returned.
RAYX::RayList traceRML(std::string filename,
                       Filter filter = Filter::KeepAllRays);

// will look at Tests/input/<filename>.csv
// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
// element coordinates of the relevant element!)
RAYX::RayList loadCSVRayUI(std::string filename);

// This only asserts that position, direction, energy are the same
// yet! many parameters are missing in RayUI and hence cannot be compared. but
// for example path length could be compared, but tests fail currently if we do
// so (TODO(rudi)).
void compareRayLists(const RAYX::RayList& rayx, const RAYX::RayList& rayui,
                     double t = 1e-11);

// This function automatcaily filters
// out weight = 0 rays from rayx, as they are automatically missing in rayui.
// This also filters out non-sequential rays to compare to Ray-UI correctly.
void compareAgainstRayUI(std::string filename, double t = 1e-11);

// updates the material tables of the Cpu Tracer to contain exactly the materials given in the std::vector.
void updateCpuTracerMaterialTables(std::vector<Material>);