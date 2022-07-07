#pragma once

#include "Core.h"
#include "Data/Importer.h"
#include "Debug.h"
#include "Material/Material.h"
#include "Model/Beamline/Objects/Objects.h"
#include "Model/Geometry/Geometry.h"
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

extern int GLOBAL_ARGC;
extern char** GLOBAL_ARGV;

// CHECK_EQ

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

// ShaderTest

extern std::unique_ptr<RAYX::Tracer> tracer;

class ShaderTest : public testing::Test {
  protected:
    static void SetUpTestSuite() {
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

// if convertToElementCoords = true, all rays are converted to element
// coordinates of beamline->back()
RAYX::RayList traceRML(std::string filename,
                       bool convertToElementCoords = true);

// will look at Tests/input/<filename>.csv
// the Ray-UI files are to be obtained by Export > RawRaysOutgoing (which are in
// element coordinates of the relevant element!)
RAYX::RayList loadCSVRayUI(std::string filename);

void compareRayLists(RAYX::RayList& rayx, RAYX::RayList& rayui,
                     double t = 1e-11);

void compareAgainstRayUI(std::string filename);

// converts global coordinates to element coordinates.
// to be used in conjunction with runTracerRaw
std::vector<RAYX::Ray> mapGlobalToElement(
    std::vector<RAYX::Ray> global, std::shared_ptr<RAYX::OpticalElement> o);

RAYX::RayList mapGlobalToElementRayList(
    RAYX::RayList& global, std::shared_ptr<RAYX::OpticalElement> o);