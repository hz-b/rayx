#include "Core.h"
#include "Data/Importer.h"
#include "Debug.h"
#include "Model/Beamline/Objects/Objects.h"
#include "Model/Geometry/Geometry.h"
#include "Tracer/Ray.h"
#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Run Test Macros
#define RUN_TEST_GRATINGS 1
#define RUN_TEST_MIRRORS 1
#define RUN_TEST_QUADRIC 1
#define RUN_TEST_RML 1
#define RUN_TEST_RZP 1
#define RUN_TEST_SHADER 1
#define RUN_TEST_SLIT 1
#define RUN_TEST_SOURCES 1

// old EXPECT system: (deprecated)

//! Using the google test framework, check all elements of two containers
#define EXPECT_ITERABLE_BASE(PREDICATE, REFTYPE, TARTYPE, ref, target)       \
    {                                                                        \
        const REFTYPE& ref_(ref);                                            \
        const TARTYPE& target_(target);                                      \
        REFTYPE::const_iterator refIter = ref_.begin();                      \
        TARTYPE::const_iterator tarIter = target_.begin();                   \
        unsigned int i = 0;                                                  \
        while (refIter != ref_.end()) {                                      \
            if (tarIter == target_.end()) {                                  \
                ADD_FAILURE() << #target " has a smaller length than " #ref; \
                break;                                                       \
            }                                                                \
            PREDICATE(*refIter, *tarIter)                                    \
                << "Containers " #ref " (refIter) and " #target              \
                   " (tarIter)"                                              \
                   " differ at index "                                       \
                << i;                                                        \
            ++refIter;                                                       \
            ++tarIter;                                                       \
            ++i;                                                             \
        }                                                                    \
        EXPECT_TRUE(tarIter == target_.end())                                \
            << #ref " has a smaller length than " #target;                   \
    }

//! Check that all elements of two same-type containers are equal
#define EXPECT_ITERABLE_EQ(TYPE, ref, target) \
    EXPECT_ITERABLE_BASE(EXPECT_EQ, TYPE, TYPE, ref, target)

//! Check that all elements of two different-type containers are equal
#define EXPECT_ITERABLE_EQ2(REFTYPE, TARTYPE, ref, target) \
    EXPECT_ITERABLE_BASE(EXPECT_EQ, REFTYPE, TARTYPE, ref, target)

//! Check that all elements of two same-type containers of doubles are equal
#define EXPECT_ITERABLE_DOUBLE_EQ(TYPE, ref, target) \
    EXPECT_ITERABLE_BASE(EXPECT_DOUBLE_EQ, TYPE, TYPE, ref, target)

#define ARR(L) std::array<double, L>

#define EXPECT_ITERABLE_DOUBLE_EQ_ARR(L, ref, target) \
    EXPECT_ITERABLE_BASE(EXPECT_DOUBLE_EQ, ARR(L), ARR(L), ref, target)
//

// new EXPECT system:
/* usage:
    CHECK_EQ(A, B, 1e-10); // with explicit tolerance
    CHECK_EQ(A, B);        // without default tolerance
*/

void check_eq(std::string filename, int line, std::string l, std::string r,
              std::vector<double> vl, std::vector<double> vr,
              double tolerance = 1e-10);

#define CHECK_EQ(L, R, ...)                                    \
    check_eq(__FILE__, __LINE__, #L, #R, RAYX::to_vec_impl(L), \
             RAYX::to_vec_impl(R),                             \
             ##__VA_ARGS__)  // __VA_ARGS__ = tolerance or nothing
