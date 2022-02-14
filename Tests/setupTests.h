#include "Core.h"
#include "Data/Importer.h"
#include "Model/Beamline/Objects/Objects.h"
#include "Model/Geometry/Geometry.h"
#include "Tracer/Ray.h"
#include "UserParameter/GeometricUserParams.h"
#include "UserParameter/WorldUserParams.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "Debug.h"

// Run Test Macros
#define RUN_TEST_GRATINGS 1
#define RUN_TEST_MIRRORS 1
#define RUN_TEST_QUADRIC 1
#define RUN_TEST_RML 1
#define RUN_TEST_RZP 1
#define RUN_TEST_SHADER 1
#define RUN_TEST_SLIT 1
#define RUN_TEST_SOURCES 1

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

// used for float, int
template <typename T1, typename T2>
bool check_eq(T1 t1, T2 t2, double tolerance) {
	return abs(t1-t2) <= tolerance;
}

// used for glm::dvec
template <typename T1, typename T2>
bool check_eq(T1 t1, T2 t2, double tolerance, int dim1) {
	for (auto i = 0; i < dim1; i++) {
		if (abs(t1[i]-t2[i]) > tolerance) { return false; }
	}
	return true;
}

// used for glm::dmat
template <typename T1, typename T2>
bool check_eq(T1 t1, T2 t2, double tolerance, int dim1, int dim2) {
	for (auto i = 0; i < dim1; i++) {
		for (auto j = 0; j < dim2; j++) {
			if (abs(t1[i][j]-t2[i][j]) > tolerance) { return false; }
		}
	}
	return true;
}

// used for arrays, vectors
template <typename T1, typename T2>
bool check_eq_iter(T1 t1, T2 t2, double tolerance) {
	auto a = t1.begin();
	auto b = t2.begin();
	while (a != t1.end() || b != t2.end()) {
		if ((a == t1.end()) != (b == t2.end())) { return false; }
		if (*a != *b) { return false; }
		a++;
		b++;
	}
	return true;
}

#define CHECK_EQ(...) if (!check_eq(__VA_ARGS__)) { RAYX_WARN << "CHECK_EQ failed"; EXPECT_TRUE(false); }
#define CHECK_EQ_ITER(...) if (!check_eq_iter(__VA_ARGS__)) { RAYX_WARN << "CHECK_EQ_ITER failed"; EXPECT_TRUE(false); }
