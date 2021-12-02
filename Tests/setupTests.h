#include "Core.h"
#include "Data/Importer.h"
#include "Model/Beamline/Objects/Objects.h"
#include "Model/Geometry/Geometry.h"
#include "Tracer/Ray.h"
#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Run Test Macros
#define RUN_TEST_GRATINGS 0
#define RUN_TEST_MIRRORS 0
#define RUN_TEST_QUADRIC 0
#define RUN_TEST_RML 0
#define RUN_TEST_RZP 0
#define RUN_TEST_SHADER 1
#define RUN_TEST_SLIT 0
#define RUN_TEST_SOURCES 0

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
