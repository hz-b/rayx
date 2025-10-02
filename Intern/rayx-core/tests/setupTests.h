#pragma once

#include <fstream>
#include <functional>
#include <sstream>
#include <type_traits>

#include "CanonicalizePath.h"
#include "Core.h"
#include "Debug/Debug.h"
#include "Design/DesignElement.h"
#include "Design/DesignSource.h"
#include "Material/Material.h"
#include "Random.h"
#include "Rml/Importer.h"
#include "Shader/Constants.h"
#include "Shader/Diffraction.h"
#include "Shader/Efficiency.h"
#include "Shader/Ray.h"
#include "Shader/RefractiveIndex.h"
#include "Writer/CsvWriter.h"
#include "Writer/H5Writer.h"
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

constexpr int PREC                 = 17;
constexpr double DEFAULT_TOLERANCE = 1e-10;

/// this is the underlying implementation of the CHECK_EQ macro.
/// asserts that tl and tr are the same up to a given tolerance, and give a fancy print if they mismatch.
/// filename, line represents where CHECK_EQ is called.
/// l and r are the printable strings representing tl and tr.
/// vl and vr represent the doubles contained in tl and tr, obtained with RAYX::formatAsVec
template <typename TL, typename TR>
inline void checkEq(std::string filename, int line, std::string l, std::string r, const TL& tl, const TR& tr, std::vector<double> vl,
                    std::vector<double> vr, double tolerance = DEFAULT_TOLERANCE) {
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
        if (counter != 0) { s << " "; }
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
    if (counter > 0) { RAYX::Warn(filename, line) << s.str(); }
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
        if (i == 3 /*eventType*/ || i == 14 /*lastElem*/) { t = 0.5; }

        checkEq(filename, line, l + names[i], r + names[i], vl[i], vr[i], {vl[i]}, {vr[i]}, t);
    }
}

/// check that L and R contain the same doubles.
// within CHECK_EQ: the __VA_ARGS__ argument is either `double tolerance` or nothing.
// all variables declared within CHECK_EQ end with `_check_eq` distinguish them from the variables that the user might write.
#define CHECK_EQ(L, R, ...)                                                                                                                       \
    {                                                                                                                                             \
        const auto& l_check_eq = L;                                                                                                               \
        const auto& r_check_eq = R;                                                                                                               \
        checkEq(__FILE__, __LINE__, #L, #R, l_check_eq, r_check_eq, RAYX::formatAsVec(l_check_eq), RAYX::formatAsVec(r_check_eq), ##__VA_ARGS__); \
    }

/// assert that x holds, and give a fancy print otherwise.
#define CHECK(x)                                                 \
    {                                                            \
        if (!(x)) { RAYX_EXIT << "CHECK(" << #x << ") failed"; } \
    }

/// check whether low <= expr <= high
#define CHECK_IN(expr, low, high)                                                                                                     \
    {                                                                                                                                 \
        auto expr_check_in = expr;                                                                                                    \
        auto low_check_in  = low;                                                                                                     \
        if (expr_check_in < low_check_in) { RAYX_EXIT << "CHECK_IN failed: " << #expr << " (" << expr_check_in << ") < " << #low; }   \
        auto high_check_in = high;                                                                                                    \
        if (expr_check_in > high_check_in) { RAYX_EXIT << "CHECK_IN failed: " << #expr << " (" << expr_check_in << ") > " << #high; } \
    }

/**
 * @brief Checks if the given values are present at least once in the vector of expected values.
 * If any value from expected is not found in values, a test failure is added.
 * @tparam T The type of the elements in the vectors.
 * @param values The vector of values to check.
 * @param expected The vector of expected values to look for.
 */
template <typename T>
inline void expectAtLeastOnce(const std::vector<T>& values, const std::vector<T>& expected) {
    for (const auto& v : expected) {
        const auto it = std::find(values.begin(), values.end(), v);
        if (it == values.end()) ADD_FAILURE();
    }
}

template <typename T>
inline void expectInRange(const std::vector<T>& values, const T min, const T max) {
    for (const auto& v : values) {
        if (v != std::clamp(v, min, max)) ADD_FAILURE();
    }
}

template <typename T>
inline void expectDifferentValues(const std::vector<T>& values) {
    if (values.size() <= 1) ADD_FAILURE();

    bool f = false;
    for (const auto a : values) {
        for (const auto b : values) {
            f = a != b;
            if (f) break;
        }

        if (f) break;
    }

    CHECK(f);
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
        using DeviceType      = RAYX::DeviceConfig::DeviceType;
        const auto deviceType = cpu ? DeviceType::Cpu : DeviceType::Gpu;
        tracer                = std::make_unique<RAYX::Tracer>(RAYX::DeviceConfig(deviceType).enableBestDevice());
    }

    // called before every test invocation.
    void SetUp() override { RAYX::fixSeed(RAYX::FIXED_SEED); }

    static void TearDownTestSuite() { tracer = nullptr; }
};

constexpr RayAttrMask attrMaskCompatibleWithRayUi =
    RayAttrMask::Position | RayAttrMask::Direction | RayAttrMask::Energy | RayAttrMask::ElectricField | RayAttrMask::OpticalPathLength;

void compare(const Rays& a, const Rays& b, double t = DEFAULT_TOLERANCE, const RayAttrMask attrMask = RayAttrMask::All);
void compareRayUiCompatible(const Rays& a, const Rays& b, double t = DEFAULT_TOLERANCE);

std::filesystem::path getBeamlineFilepath(std::string filename);
Beamline loadBeamline(std::string filename);
Rays traceRml(std::string filename, RayAttrMask attrMask = RayAttrMask::All);
std::pair<Beamline, Rays> loadBeamlineAndTrace(std::string filename, RayAttrMask attrMask = RayAttrMask::All);
Rays loadCsvRayUi(std::string filename);

/**
 * @brief Trace a beamline and make the results compatible with RayUi.
 * Only ray RAY-UI compatible ray attributes are recorded.
 * Also, y and z coordinates are swapped for Position and Direction.
 */
Rays traceRmlAndMakeCompatibleWithRayUi(std::string filename, Sequential seq);

/// Trace a beamline and compare the results against the RayUi results.
void traceRmlAndCompareAgainstRayUi(std::string filename, double tolerance = DEFAULT_TOLERANCE, Sequential seq = Sequential::No);

/**
 * @brief Trace a beamline and compare the results against correct results.
 * @note Correct results are cached and can be overriden, using one of the scripts in the scripts folder.
 */
void traceRmlAndCompareAgainstCorrectResults(std::string filename, double tolerance = DEFAULT_TOLERANCE);

MaterialTables createMaterialTables(std::vector<Material> mats_vec);
