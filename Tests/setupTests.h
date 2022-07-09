#pragma once

#include "Core.h"
#include "Data/Importer.h"
#include "Debug.h"
#include "Material/Material.h"
#include "Model/Beamline/Objects/Objects.h"

#include "PathResolver.h"
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

extern int GLOBAL_ARGC;
extern char** GLOBAL_ARGV;

/* usage:
    CHECK_EQ(A, B, 1e-10); // with explicit tolerance
    CHECK_EQ(A, B);        // without default tolerance
*/

void checkEq(std::string filename, int line, std::string l, std::string r,
             std::vector<double> vl, std::vector<double> vr,
             double tolerance = 1e-10);

#define CHECK_EQ(L, R, ...)                                   \
    checkEq(__FILE__, __LINE__, #L, #R, RAYX::formatAsVec(L), \
            RAYX::formatAsVec(R),                             \
            ##__VA_ARGS__)  // __VA_ARGS__ = tolerance or nothing
