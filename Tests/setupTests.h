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

// used for float, int
template <typename T1, typename T2>
void check_eq(std::string filename, int line, std::string l, std::string r, T1 t1, T2 t2, double tolerance) {
	if (abs(t1-t2) <= tolerance) return;

	RAYX::Warn(filename, line) << l << " != " << r << ":";
	RAYX::Warn(filename, line) << "\x1B[36m" << t1 << "|" << t2 << "\x1B[31m";
	EXPECT_TRUE(false);
}

// used for glm::dvec
template <typename T1, typename T2>
void check_eq(std::string filename, int line, std::string l, std::string r, T1 t1, T2 t2, double tolerance, int dim1) {
	bool success = true;
	for (auto i = 0; i < dim1; i++) {
		if (abs(t1[i]-t2[i]) > tolerance) {
			success = false;
			break;
		}
	}
	if (success) return;

	RAYX::Warn(filename, line) << l << " != " << r << ":";
	std::stringstream s;
	for (auto i = 0; i < dim1; i++) {
		if (i != 0) { s << " "; }
		if (abs(t1[i]-t2[i]) <= tolerance) {
			s << t1[i] << "|" << t2[i];
		} else {
			s << "\x1B[36m" << t1[i] << "|" << t2[i] << "\x1B[31m";
		}
	}
	RAYX::Warn(filename, line) << s.str();
	EXPECT_TRUE(false);
}

// used for glm::dmat
template <typename T1, typename T2>
void check_eq(std::string filename, int line, std::string l, std::string r, T1 t1, T2 t2, double tolerance, int dim1, int dim2) {
	bool success = true;
	for (auto i = 0; i < dim1; i++) {
		for (auto j = 0; j < dim2; j++) {
			if (abs(t1[i][j]-t2[i][j]) > tolerance) {
				success = false;
				break;
			}
		}
	}
	if (success) return;

	RAYX::Warn(filename, line) << l << " != " << r << ":";
	for (auto i = 0; i < dim1; i++) {
		std::stringstream s;
		for (auto j = 0; j < dim2; j++) {
			if (j != 0) { s << " "; }
			if (abs(t1[i][j]-t2[i][j]) <= tolerance) {
				s << t1[i][j] << "|" << t2[i][j];
			} else {
				s << "\x1B[36m" << t1[i][j] << "|" << t2[i][j] << "\x1B[31m";
			}
			RAYX::Warn(filename, line) << s.str();
		}
	}
	EXPECT_TRUE(false);
}

// used for arrays, vectors
template <typename T1, typename T2>
void check_eq_iter(std::string filename, int line, std::string l, std::string r, T1 t1, T2 t2, double tolerance, int break_after=-1) {
	{
		bool success = true;

		auto a = t1.begin();
		auto b = t2.begin();
		while (a != t1.end() || b != t2.end()) {
			if ((a == t1.end()) != (b == t2.end())) {
				RAYX::Warn(filename, line) << l << " != " << r << ": different lengths!";
				EXPECT_TRUE(false);
				return;
			}
			if (*a != *b) {
				success = false;
				break;
			}
			a++;
			b++;
		}
		if (success) return;
	}

	auto a = t1.begin();
	auto b = t2.begin();
	int counter = 0;

	RAYX::Warn(filename, line) << l << " != " << r << ":";

	std::stringstream s;
	while (a != t1.end()) {
		if (counter != 0) { s << " "; }
		if (abs(*a-*b) <= tolerance) {
			s << *a << "|" << *b;
		} else {
			s << "\x1B[36m" << *a << "|" << *b << "\x1B[31m";
		}
		counter++;
		if (counter == break_after) {
			counter = 0;
			RAYX::Warn(filename, line) << s.str();
			s = std::stringstream();
		}
		a++;
		b++;
	}
	if (counter > 0) {
		RAYX::Warn(filename, line) << s.str();
	}
	EXPECT_TRUE(false);
}

#define CHECK_EQ(L, R, ...) check_eq(__FILE__, __LINE__, #L, #R, L, R, __VA_ARGS__)
#define CHECK_EQ_ITER(L, R, ...) check_eq_iter(__FILE__, __LINE__, #L, #R, L, R, __VA_ARGS__)
