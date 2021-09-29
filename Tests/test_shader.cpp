#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Beamline/Beamline.h"
#include "Surface/Quadric.h"
#include "Beamline/Objects/ReflectionZonePlate.h"
#include "Beamline/Objects/PlaneMirror.h"
#include "Beamline/Objects/SphereMirror.h"
#include "Beamline/Objects/MatrixSource.h"
#include "Tracer/TracerInterface.h"
#include "GeometricUserParams.h"
#include "Core.h"
#include "Ray.h"
// #include "Tracer/TracerInterface.h"
#include "VulkanTracer.h"
#include <fstream>
#include <sstream>
#include <functional>
#include <type_traits>
//std::vector<double> zeros = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

using ::testing::ElementsAre;
std::vector<double> zeros = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
std::vector<double> zeros7 = { 0,0,0,0,0, 0,0 }; // for slope error

#define RAY_DOUBLE_COUNT 12

//! Using the google test framework, check all elements of two containers
#define EXPECT_ITERABLE_BASE( PREDICATE, REFTYPE, TARTYPE, ref, target) \
    { \
    const REFTYPE& ref_(ref); \
    const TARTYPE& target_(target); \
    REFTYPE::const_iterator refIter = ref_.begin(); \
    TARTYPE::const_iterator tarIter = target_.begin(); \
    unsigned int i = 0; \
    while(refIter != ref_.end()) { \
        if ( tarIter == target_.end() ) { \
            ADD_FAILURE() << #target " has a smaller length than " #ref ; \
            break; \
        } \
        PREDICATE(* refIter, * tarIter) \
            << "Containers " #ref  " (refIter) and " #target " (tarIter)" \
               " differ at index " << i; \
        ++refIter; ++tarIter; ++i; \
    } \
    EXPECT_TRUE( tarIter == target_.end() ) \
        << #ref " has a smaller length than " #target ; \
    }

//! Check that all elements of two same-type containers are equal
#define EXPECT_ITERABLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, TYPE, TYPE, ref, target )

//! Check that all elements of two different-type containers are equal
#define EXPECT_ITERABLE_EQ2( REFTYPE, TARTYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_EQ, REFTYPE, TARTYPE, ref, target )

//! Check that all elements of two same-type containers of doubles are equal
#define EXPECT_ITERABLE_DOUBLE_EQ( TYPE, ref, target) \
    EXPECT_ITERABLE_BASE( EXPECT_DOUBLE_EQ, TYPE, TYPE, ref, target )


std::list<double> runTracer(std::vector<RAYX::Ray> testValues, std::vector<std::shared_ptr<RAYX::OpticalElement>> elements) {
    for (int i = 0;i < 16;i++) {
        std::cout << "elements[0]: " << elements[0]->getSurfaceParams()[i] << std::endl;
    }
    RAYX::TracerInterface ti;
    VulkanTracer tracer;

    std::list<std::vector<RAYX::Ray>> rayList;
    tracer.setBeamlineParameters(1, elements.size(), testValues.size());
    std::cout << "testValues.size(): " << testValues.size() << std::endl;
    (tracer).addRayVector(testValues.data(), testValues.size());
    std::cout << "add rays to tracer done" << std::endl;

    for (std::shared_ptr<RAYX::OpticalElement> e : elements) {
        ti.addOpticalElementToTracer(tracer, e);
    }
    tracer.run(); //run tracer
    std::list<double> outputRays;
    std::vector<Ray> outputRayVector = *(tracer.getOutputIteratorBegin());
    for (auto iter = outputRayVector.begin();iter != outputRayVector.end();iter++) {
        outputRays.push_back((*iter).getxPos());
        outputRays.push_back((*iter).getyPos());
        outputRays.push_back((*iter).getzPos());
        outputRays.push_back((*iter).getWeight());
        outputRays.push_back((*iter).getxDir());
        outputRays.push_back((*iter).getyDir());
        outputRays.push_back((*iter).getzDir());
        outputRays.push_back((*iter).getEnergy());
        outputRays.push_back((*iter).getS0());
        outputRays.push_back((*iter).getS1());
        outputRays.push_back((*iter).getS2());
        outputRays.push_back((*iter).getS3());
    }
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    tracer.cleanup();
    return outputRays;
}

void writeToFile(std::list<double> outputRays, std::string name)
{
    std::cout << "writing to file..." << name << std::endl;
    std::ofstream outputFile;
    outputFile.precision(17);
    std::cout.precision(17);
    std::string filename = "../../Tests/output/";
    filename.append(name);
    filename.append(".csv");
    outputFile.open(filename);
    char sep = ';'; // file is saved in .csv (comma seperated value), excel compatibility is manual right now
    outputFile << "Index" << sep << "Xloc" << sep << "Yloc" << sep << "Zloc" << sep << "Weight" << sep << "Xdir" << sep << "Ydir" << sep << "Zdir" << sep << "Energy" << sep << "S0" << sep << "S1" << sep << "S2" << sep << "S3" << std::endl;
    // outputFile << "Index,Xloc,Yloc,Zloc,Weight,Xdir,Ydir,Zdir" << std::endl;

    size_t counter = 0;
    int print = 0; // whether to print on std::out (0=no, 1=yes)
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end(); i++) {
        if (counter % RAY_DOUBLE_COUNT == 0) {
            outputFile << counter / VULKANTRACER_RAY_DOUBLE_AMOUNT;
            if (print == 1) std::cout << ")" << std::endl;
            if (print == 1) std::cout << "(";
        }
        outputFile << sep << *i;
        if (counter % RAY_DOUBLE_COUNT == RAY_DOUBLE_COUNT - 1) {
            outputFile << std::endl;
            counter++;
            continue;
        }
        if (counter % RAY_DOUBLE_COUNT == 3) {
            if (print == 1) std::cout << ") ";
        }
        else if (counter % RAY_DOUBLE_COUNT == 4) {
            if (print == 1) std::cout << " (";
        }
        else if (counter % RAY_DOUBLE_COUNT != 0) {
            if (print == 1) std::cout << ", ";
        }
        if (print == 1) std::cout << *i;
        counter++;
    }
    if (print == 1) std::cout << ")" << std::endl;
    outputFile.close();
    std::cout << "done!" << std::endl;
}

void compareFromCorrect(std::vector<RAYX::Ray> correct, std::list<double> outputRays, double tolerance) {
    int counter = 0;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        if (counter % RAY_DOUBLE_COUNT == 0) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_position.x, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 1) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_position.y, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 2) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_position.z, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 3) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_weight, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 4) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_direction.x, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 5) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_direction.y, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 6) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_direction.z, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 7) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_energy, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 8) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_stokes.x, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 9) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_stokes.y, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 10) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_stokes.z, tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 11) {
            EXPECT_NEAR(*i, correct[int(counter / RAY_DOUBLE_COUNT)].m_stokes.w, tolerance);
        }
        counter++;
        i++;
    }
}

template<typename ret, typename par>
using fn = std::function<ret(par)>;

template<typename ret, typename par>
void compareFromFunction(fn<ret, par> func, std::vector<RAYX::Ray> testValues, std::list<double> outputRays, double tolerance) {
    int counter = 0;
    typename std::remove_reference<par>::type p;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        if (counter % RAY_DOUBLE_COUNT == 0) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.x), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 1) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.y), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 2) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.z), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 3) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_weight), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 4) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.x), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 5) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.y), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 6) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.z), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 7) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_energy), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 8) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.x), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 9) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.y), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 10) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.z), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 11) {
            EXPECT_NEAR(*i, func(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.w), tolerance);
        }
        counter++;
        i++;
    }
}

/*
TEST(Tracer, testUniformRandom) {
    double settings = 17;

    RAYX::MatrixSource m = RAYX::MatrixSource(0, "Matrix source 1", 2000, 0, 0.065, 0.04, 0.0, 0.001, 0.001, 100, 0, 1, 0, 0, { 0,0,0,0 });
    std::vector<RAYX::Ray> testValues = m.getRays();

    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("testRandomNumbers", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);
    std::list<double> outputRays = runTracer(testValues, { q });

    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end(); i++) {
        ASSERT_TRUE(*i <= 1.0);
        ASSERT_TRUE(*i >= 0.0);
    }
    std::string filename = "testFile_randomUniform";
    writeToFile(outputRays, filename);
}


TEST(Tracer, ExpTest) {
    std::list<std::vector<RAYX::Ray>> rayList;
    int n = 10;
    int low = -4;
    int high = 4;
    double settings = 18;
    RAYX::RandomRays random = RAYX::RandomRays(n, low, high);

    std::vector<RAYX::Ray> testValues = random.getRays();
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, -3), glm::dvec3(PI, 2, 3), glm::dvec4(10,-4.41234,0,1.224), 4, 5);
    testValues.push_back(r);

    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("ExpTest", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    double tolerance = 1e-13;
    auto expfun = fn<double, double>([] (double x) {return exp(x);});
    compareFromFunction(expfun, testValues, outputRays, tolerance);

}

TEST(Tracer, LogTest) {
    std::list<std::vector<RAYX::Ray>> rayList;
    int n = 10;
    int low = 1;
    int high = 4;
    double settings = 19;
    RAYX::RandomRays random = RAYX::RandomRays(n, low, high);

    std::vector<RAYX::Ray> testValues = random.getRays();
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0.1, 1, 0.3), glm::dvec3(PI, 2, 3), glm::dvec4(0.2345,100, 3.423453, 0.00000001), 2.1 , 5);
    testValues.push_back(r);

    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("LogTest", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    double tolerance = 1e-13;
    auto logfun = fn<double, double>([] (double x) {return log(x);});
    compareFromFunction(logfun, testValues, outputRays, tolerance);

}


TEST(Tracer, testRefrac2D) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    std::vector<std::shared_ptr<RAYX::OpticalElement>> quadrics;
    double settings = 16;

    // ray.position = normal at intersection point, ray.direction = direction of ray, ray.weight = weight of ray before refraction
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0.0001666666635802469, -0.017285764670739875, 0.99985057611723738), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.012664171360811521, 0.021648721107426414, 0.99968542634078494), glm::dvec4(1,1,0,0), 0, 1.0);
    correct.push_back(c);
    // one quadric for each ray to transport ax and az for that test ray to the shader
    double az = 0.00016514977645243345;
    double ax = 0.012830838024391771;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("testRefrac2D", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ az, ax,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);
    quadrics.push_back(q);

    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0.00049999999722222275, -0.017285762731583675, 0.99985046502305308), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0.00049999999722222275, -0.017285762731583675, 0.99985046502305308), glm::dvec4(1,1,0,0), 0, 0.0);
    correct.push_back(c);
    az = -6.2949352042540596e-05;
    ax = 0.038483898782123105;
    q = std::make_shared<RAYX::OpticalElement>("testRefrac2D", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ az, ax,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);
    quadrics.push_back(q);

    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0.0001666666635802469, -0.017619047234249029, 0.99984475864845179), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0.0001666666635802469, -0.017619047234249029, 0.99984475864845179), glm::dvec4(1,1,0,0), 0, 0.0);
    correct.push_back(c);
    az = -0.077169530850327184;
    ax = 0.2686127340088395;
    q = std::make_shared<RAYX::OpticalElement>("testRefrac2D", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ az, ax,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);
    quadrics.push_back(q);

    // normal != 0 (spherical RZP)
    r = RAYX::Ray(glm::dvec3(0.050470500672820856, 0.95514062789960541, -0.29182033770349547), glm::dvec3(-0.000499999916666667084, -0.016952478247434233, 0.99985617139734351), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0.050470500672820856, 0.95514062789960541, -0.29182033770349547), glm::dvec3(0.080765992839840872, 0.57052382524991363, 0.81730007905468893), glm::dvec4(1,1,0,0), 0, 1.0);
    correct.push_back(c);
    az = 0.0021599283476277926;
    ax = -0.050153240660177005;
    q = std::make_shared<RAYX::OpticalElement>("testRefrac2D", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ az, ax,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);
    quadrics.push_back(q);

    std::list<double> outputRays = runTracer(testValues, quadrics);

    double tolerance = 1e-12;
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testNormalCartesian) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    // encode: ray.position.x = slopeX, ray.position.z = slopeZ. ray.direction = normal at intersection point from eg quad fct.
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    // store correct resulting normal[0:3] in ray.direction and fourth component (normal[3]) in weight
    // case: normal unchanged bc slope = 0
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    // normal != (0,1,0), slope still = 0
    r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    // normal unchanged
    c = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0.0);
    correct.push_back(c);

    // normal = (0,1,0), slopeX = 2, slopeZ = 3
    r = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(-0.90019762973551742, 0.41198224566568298, -0.14112000805986721), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(-9431.2371568647086, 4310.7269916467494, -1449.3435640204684), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    double settings = 13;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testNormalCartesian", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-12; // smallest possible
    // return format = pos (0=x,1=y,2=z), 3=weight, dir (4=x,5=y,6=z), 7=0
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testNormalCylindrical) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    // encode: ray.position.x = slopeX, ray.position.z = slopeZ. ray.direction = normal at intersection point from eg quad fct.
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    // store correct resulting normal[0:3] in ray.direction and fourth component (normal[3]) in weight
    // case: normal unchanged bc slope = 0
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    // normal != (0,1,0), slope still = 0
    r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    // normal slightly unchanged in x (due to limited precision?!)
    c = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(5.0465463027115769, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0.0);
    correct.push_back(c);

    // normal = (0,1,0), slopeX = 2, slopeZ = 3
    r = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(0.90019762973551742, 0.41198224566568292, -0.14112000805986721), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(5.0465463027123736, 10470.451695989539, -28.532199794465537), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(2, 0, 3), glm::dvec3(9431.2169472441783, 4310.7711493493844, -1449.3437356459144), glm::dvec4(1,1,0,0), 0, 0);
    correct.push_back(c);

    double settings = 14;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testNormalCylindrical", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-11; // smallest possible
    // return format = pos (0=x,1=y,2=z), 3=weight, dir (4=x,5=y,6=z), 7=0
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testRefrac) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    double a = 0.01239852;
    // encode: ray.position = normal at intersection point. ray.direction = direction of ray, ray.weigth = weight of ray
    // plane surface
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.00049999991666667084, -0.99558611855684065, 0.09385110834192622), glm::dvec4(1,1,0,0), 0, 1);
    testValues.push_back(r);
    // store correct resulting weight in c.weight and calculated direction in c.direction
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.00049999991666667084, 0.99667709206767885, 0.08145258834192623), glm::dvec4(1,1,0,0), 0, 1);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.000016666664506172893, -0.995586229182718, 0.093851118714515264), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.000016666664506160693, 0.9966772027014974, 0.081452598714515267), glm::dvec4(1,1,0,0), 0, 1.0);
    correct.push_back(c);

    // spherical grating, same a
    r = RAYX::Ray(glm::dvec3(0.0027574667592826954, 0.99999244446428082, -0.0027399619384214182), glm::dvec3(-0.00049999991666667084, -0.99558611855684065, 0.093851108341926226), glm::dvec4(1,1,0,0), 0, 1);
    testValues.push_back(r);
    // pos does not matter
    c = RAYX::Ray(glm::dvec3(0.0027574667592826954, 0.99999244446428082, -0.0027399619384214182), glm::dvec3(0.0049947959329671825, 0.99709586573547515, 0.07599267429701162), glm::dvec4(1,1,0,0), 0, 1);
    correct.push_back(c);


    double settings = 15;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testRefrac", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ a,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-12;
    // return format = pos (x,y,z), weight, dir (x,y,z), 0
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testRefracBeyondHor) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    // encode: ray.position = normal at intersection point. ray.direction = direction of ray, ray.weigth = weight of ray
    // plane surface
    // beyond horizon
    double a = -0.038483898782123105;
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(-0.99991341437509562, 0.013149667401360443, -0.00049999997222215965), glm::dvec4(1,1,0,0), 0, 1.0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(-0.99991341437509562, 0.013149667401360443, -0.00049999997222215965), glm::dvec4(1,1,0,0), 0, 0.0);
    correct.push_back(c);


    double settings = 15;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testRefrac", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, std::vector<double>{ a,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 }, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-12;
    // return format = pos (x,y,z), weight, dir (x,y,z), 0
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testWasteBox) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    // encode: ray.position = position of intersection point. ray.direction.x = xLength of opt. element, ray.direction.z = zLength of optical element, ray.weigth = weight of ray before calling wastebox
    // case: intersection point on surface
    RAYX::Ray r = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(50, 0, 200), glm::dvec4(0,0,0,0), 0, 1);
    testValues.push_back(r);
    // store correct resulting weight in weight of c
    RAYX::Ray c = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(50, 0, 200), glm::dvec4(0,0,0,0), 0, 1);
    correct.push_back(c);

    // intersection point not on surface
    r = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(5, 0, 20), glm::dvec4(0,0,0,0), 0, 1.0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(5, 0, 20), glm::dvec4(0,0,0,0), 0, 0.0);
    correct.push_back(c);

    // intersection point not on surface
    r = RAYX::Ray(glm::dvec3(-1.6822205656320104, 0, 28.760233508097873), glm::dvec3(5, 0, 20), glm::dvec4(0,0,0,0), 0, 1);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(-1.6822205656320104, 0, 28.760233508097873), glm::dvec3(5, 0, 20), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    // ray already had weight 0
    r = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(50, 0, 200), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(-5.0466620698997637, 0, 28.760236725599515), glm::dvec3(50, 0, 200), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    double settings = 11;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testWasteBox", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-10;
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testRZPLineDensityDefaulParams) { // point to point
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    // {1st column, 2nd column, 3rd column, 4th column}
    // {image_type, rzp_type, derivation_method, zOffsetCenter}, -> point2point(0), elliptical(0), formulas(0), 0
    // {risag, rosag, rimer, romer},
    // {d_alpha, d_beta, d_ord, wl}, {0,0,0,0}
    std::vector<double> inputValues = { 0,0,0,0, 100,500,100,500, 0.017453292519943295,0.017453292519943295,-1,12.39852 * 1e-06, 0,0,0,0 };

    // encode: ray.position = position of test ray. ray.direction = normal at intersection point.
    RAYX::Ray r = RAYX::Ray(glm::dvec3(-5.0805095016939532, 0, 96.032788311782269), glm::dvec3(0, 1, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(3103.9106911246745, 0, 5.0771666329965663), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.6935030407867075, 0, 96.032777495754004), glm::dvec3(0, 1, 0), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1034.8685185321933, 0, -13.320120179862874), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    // spherical (normal != (0,1,0))
    r = RAYX::Ray(glm::dvec3(-5.047050067282087, 4.4859372100394515, 29.182033770349552), glm::dvec3(0.05047050067282087, 0.95514062789960552, -0.29182033770349552), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(4045.0989844091873, 0, -174.20856260487483), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.6802365843267262, 1.3759250917712356, 16.445931214643075), glm::dvec3(0.016802365843267261, 0.98624074908228765, -0.16445931214643075), glm::dvec4(1,1,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1418.1004208892471, 0, 253.09836635775156), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    double settings = 12;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testRZPpoint2point", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, inputValues, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-10;
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, testRZPLineDensityAstigmatic) { // astigmatic 2 astigmatic
    VulkanTracer tracer;
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    // {1st column, 2nd column, 3rd column, 4th column} -> astigmatic2astigmatic(1), elliptical(0), formulas(0), 0
    // {image_type, rzp_type, derivation_method, zOffsetCenter},
    // {risag, rosag, rimer, romer},
    // {d_alpha, d_beta, d_ord, wl}, {0,0,0,0}
    std::vector<double> inputValues = { 1,0,0,0, 100,500,100,500, 0.017453292519943295,0.017453292519943295,-1,12.39852 * 1e-06, 0,0,0,0 };

    // encode: ray.position = position of test ray. ray.direction = normal at intersection point.
    RAYX::Ray r = RAYX::Ray(glm::dvec3(-5.0805095016939532, 0, 96.032788311782269), glm::dvec3(0, 1, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(3103.9106911246745, 0, 5.0771666329965663), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.6935030407867075, 0, 96.032777495754004), glm::dvec3(0, 1, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1034.8685185321933, 0, -13.320120179862874), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    // spherical (normal != (0,1,0))
    r = RAYX::Ray(glm::dvec3(-5.047050067282087, 4.4859372100394515, 29.182033770349552), glm::dvec3(0.05047050067282087, 0.95514062789960552, -0.29182033770349552), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(4045.0989844091873, 0, -174.20856260487483), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.6802365843267262, 1.3759250917712356, 16.445931214643075), glm::dvec3(0.016802365843267261, 0.98624074908228765, -0.16445931214643075), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1418.1004208892471, 0, 253.09836635775156), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    double settings = 12;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testRZPAstigmatic", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, inputValues, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-10;
    compareFromCorrect(correct, outputRays, tolerance);

}


// test pow(a,b) = a^b function. ray position[i] ^ ray direction[i] for i in {0,1,2}
TEST(Tracer, testRayMatrixMult) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    // {1st column, 2nd column, 3rd column, 4th column}
    std::vector <double> matrix = { 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16 };

    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(13, 14, 15), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(1, 1, 0), glm::dvec3(0, 1, 1), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1 + 5 + 13, 2 + 6 + 14, 3 + 7 + 15), glm::dvec3(5 + 9, 6 + 10, 7 + 11), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(1, 2, 3), glm::dvec3(4, 5, 6), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1 * 1 + 2 * 5 + 3 * 9 + 13, 1 * 2 + 2 * 6 + 3 * 10 + 14, 1 * 3 + 2 * 7 + 3 * 11 + 15), glm::dvec3(4 * 1 + 5 * 5 + 6 * 9, 4 * 2 + 5 * 6 + 6 * 10, 4 * 3 + 5 * 7 + 6 * 11), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    double settings = 10;
    std::shared_ptr<RAYX::OpticalElement> q1 = std::make_shared<RAYX::OpticalElement>("testRayMatrixMult", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, matrix, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q1 });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-12;
    compareFromCorrect(correct, outputRays, tolerance);
}

// test pow(a,b) = a^b function. ray position[i] ^ ray direction[i] for i in {0,1,2}
TEST(Tracer, testDPow) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    RAYX::Ray r = RAYX::Ray(glm::dvec3(0.0, 0, 0), glm::dvec3(0, 1, -1), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(1,0,1), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(2, 2, 3), glm::dvec3(0, 1, 7), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1,2,2187), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(4, -4, 2), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0,1,0), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0.2, 19.99 / 2, PI), glm::dvec3(4, 3, 6), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0.0016,998.50074987499977,961.38919357530415), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.0, -1.0, -1.0), glm::dvec3(-4, 3, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1, -1, 1), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(-1.0, -1.0, -1.0), glm::dvec3(4, 5, 6), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1, -1, 1), glm::dvec3(0,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    double settings = 7;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("testDoublePow", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-12;
    compareFromCorrect(correct, outputRays, tolerance);
}

// test pow(a,b) = a^b function. ray position[i] ^ ray direction[i] for i in {0,1,2}
TEST(Tracer, testCosini) {
    VulkanTracer tracer;
    std::vector<RAYX::Ray> testValues;
    // phi, psi given in position.x, position.y
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 0, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    r = RAYX::Ray(glm::dvec3(1, 1, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    r = RAYX::Ray(glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);
    r = RAYX::Ray(glm::dvec3(PI, PI, 0), glm::dvec3(0, 0, 0), glm::dvec4(0,0,0,0), 0, 0);
    testValues.push_back(r);


    double settings = 9;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("testCosini", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    int counter = 0;
    int corr_counter = 0;
    double tolerance = 1e-12;
    // return format = pos (x,y,z), weight, dir (x,y,z), 0
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        if (counter % RAY_DOUBLE_COUNT == 4) { // xdir = sin(psi) * sin(phi)
            EXPECT_NEAR(*i, cos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.y) * sin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.x), tolerance);
            std::cout << *i << ", ";
        }
        else if (counter % RAY_DOUBLE_COUNT == 5) { // ydir = -sin(psi)
            EXPECT_NEAR(*i, -sin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.y), tolerance);
            std::cout << *i << ", ";
        }
        else if (counter % RAY_DOUBLE_COUNT == 6) { // zdir = cos(psi) * cos(phi)
            EXPECT_NEAR(*i, cos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.y) * cos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.x), tolerance);
            std::cout << *i;
        }
        else if (counter % RAY_DOUBLE_COUNT == RAY_DOUBLE_COUNT-1) {
            std::cout << std::endl;
        }
        counter++;
        i++;

    }
}

// test factorial f(a) = a!
TEST(Tracer, factTest) {
    VulkanTracer tracer;
    std::vector<RAYX::Ray> testValues;
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, 2), glm::dvec3(-1, 4, 17), glm::dvec4(10, -4, 12.2, -0), 0, -2); // glm::dvec4(100, -4, 12.2, -0)
    testValues.push_back(r);

    // pos, weight, dir, energy, stokes
    std::vector<double> correct = { 1,1,2, -2, -1,24,355687428096000, 1, 3628800,-4,479001600,1};

    double settings = 8;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("testPow", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    int counter = 0;
    double tolerance = 1e-12;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        EXPECT_NEAR(*i, correct[counter], tolerance);
        counter++;
        i++;

    }
}

TEST(Tracer, bessel1Test) {
    VulkanTracer tracer;
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    //RAYX::Ray r = RAYX::Ray(glm::dvec3(-12.123,20.1,100), glm::dvec3(20.0,0,23.1), 0);
    RAYX::Ray r = RAYX::Ray(glm::dvec3(-12.123, 20.1, 100), glm::dvec3(20.0, 0, 23.1), glm::dvec4(0,0,0,0), 0, -0.1);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0,0,0), glm::dvec3(0.066833545658411195,0,0), glm::dvec4(0,0,0,0), 0, 0);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(12.123, 2, 0.00000001), glm::dvec3(19.99, 10.2, PI), glm::dvec4(0,0,0,0), 0, 4);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(-0.21368198451302897,0.57672480775687363,5e-09), glm::dvec3(0.065192988349741882,-0.0066157432977083167,0.28461534317975273), glm::dvec4(0,0,0,0), 0, -0.06604332802354923);
    correct.push_back(c);

    double settings = 6;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("TestBessel1", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;
    double tolerance = 1e-08;
    compareFromCorrect(correct, outputRays, tolerance);
}


TEST(Tracer, diffractionTest) {
    VulkanTracer tracer;
    std::vector<RAYX::Ray> testValues;
    RAYX::Ray r;
    // pos = (iopt,  xlenght, ylength) weight = wavelength
    //r = RAYX::Ray(glm::dvec3(1, 50,100), glm::dvec3(0.0,0.0,0.0), 0.1);
    //testValues.push_back(r);
    int n = 1;
    for (int i = 0; i < n; i++) {
        r = RAYX::Ray(glm::dvec3(1, 20, 2), glm::dvec3(0.0, 0.0, 0.0), glm::dvec4(0,0,0,0), 0, 12.39852);
        testValues.push_back(r);
    }
    double lowerDphi = 1e-10;
    double upperDphi = 1e-06;
    double lowerDpsi = 1e-08;
    double upperDpsi = 1e-05;

    double settings = 5;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("TestDiffraction", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    int counter = 0;
    double tolerance = 1e-12;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        std::cout << *i << ", ";
        if (counter % RAY_DOUBLE_COUNT == 0) {
            EXPECT_TRUE(abs(*i) < upperDphi);
            EXPECT_TRUE(abs(*i) > lowerDphi);
        }
        else if (counter % RAY_DOUBLE_COUNT == 1) {
            EXPECT_TRUE(abs(*i) < upperDpsi);
            EXPECT_TRUE(abs(*i) > lowerDpsi);
        }
        counter++;
        i++;
    }
}

TEST(Tracer, TrigTest) {
    std::list<std::vector<RAYX::Ray>> rayList;
    int n = 10;
    int low = -1;
    int high = 1;
    RAYX::RandomRays random = RAYX::RandomRays(n, low, high);

    std::vector<RAYX::Ray> testValues = random.getRays();
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(1, 0, 1), glm::dvec4(1,-1,-1,0), -1, 1);
    testValues.push_back(r);
    r = RAYX::Ray(glm::dvec3(PI, PI, PI), glm::dvec3(PI, PI, PI), glm::dvec4(1,1,0,0), 0, PI);
    testValues.push_back(r);
    double settings = 1;

    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("qq", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    int counter = 0;
    double tolerance = 1e-12;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        if (counter % RAY_DOUBLE_COUNT == 0) {
            EXPECT_NEAR(*i, cos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.x), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 1) {
            EXPECT_NEAR(*i, cos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.y), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 2) {
            EXPECT_NEAR(*i, sin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_position.z), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 3) {
            EXPECT_NEAR(*i, sin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_weight), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 4) {
            EXPECT_NEAR(*i, atan(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.x), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 5) {
            EXPECT_NEAR(*i, atan(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.y), tolerance);
        }
        else if (counter % RAY_DOUBLE_COUNT == 6) {
            if (testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.z >= -1 && testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.z <= 1) {
                EXPECT_NEAR(*i, acos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_direction.z), tolerance);
            }
        }
        else if (counter % RAY_DOUBLE_COUNT == 7) {
            if (testValues[int(counter / RAY_DOUBLE_COUNT)].m_energy >= -1 && testValues[int(counter / RAY_DOUBLE_COUNT)].m_energy <= 1) {
                EXPECT_NEAR(*i, acos(testValues[int(counter / RAY_DOUBLE_COUNT)].m_energy), tolerance);
            }
        }else if(counter % RAY_DOUBLE_COUNT == 8) {
            if (testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.x >= -1 && testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.x <= 1) {
                EXPECT_NEAR(*i, asin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.x), tolerance);
            }
        }else if(counter % RAY_DOUBLE_COUNT == 9) {
            if (testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.y >= -1 && testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.y <= 1) {
                EXPECT_NEAR(*i, asin(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.y), tolerance);
            }
        }else if(counter % RAY_DOUBLE_COUNT == 10) {
            EXPECT_NEAR(*i, atan(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.z), tolerance);
        }else if(counter % RAY_DOUBLE_COUNT == 11) {
            EXPECT_NEAR(*i, atan(testValues[int(counter / RAY_DOUBLE_COUNT)].m_stokes.w), tolerance);
        }
        counter++;
        i++;
    }
}


// test VLS function that calculates new a from given a, z-position and 6 vls parameters
TEST(Tracer, vlsGratingTest) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;

    double z = 5.0020783775947848;
    double a = 0.01239852;
    double settings = 4;

    // encode vls parameters in ray direction and position, a = wl*linedensity*ord*1.e-6 is given as well (in weight of ray)
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 0.0), glm::dvec4(1,1,0,0), 0, a);
    testValues.push_back(r);
    // a should remain unchanged if all vls parameters are 0
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 0.0), glm::dvec4(1,1,0,0), 0, a);
    correct.push_back(c);

    // use some vls values and compare with A calculated by old ray UI
    r = RAYX::Ray(glm::dvec3(1, 2, 3), glm::dvec3(4, 5, 6), glm::dvec4(1,1,0,0), 0, a);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(1, 2, 3), glm::dvec3(4, 5, 6), glm::dvec4(1,1,0,0), 0, 9497.479959611925);
    correct.push_back(c);

    // give z position and setting=4 to start vls test on shader
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("TestVLS", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,z,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });

    double tolerance = 1e-15;
    compareFromCorrect(correct, outputRays, tolerance);

}

TEST(Tracer, planeRefracTest) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    double settings = 3;

    // normal (always 0,1,0) encoded in ray position, a encoded in direction.x, direction.y and direction.z are actual ray directions
    RAYX::Ray r = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0, -0.99558611855684065, 0.09385110834192662), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0, 0.99667709206767885, 0.08145258834192623), glm::dvec4(1,1,0,0), 0, 0.01239852);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, -0.99558611855684065, 0.09385110834192662), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, 0.99667709206767885, 0.08145258834192623), glm::dvec4(1,1,0,0), 0, 0.01239852);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, -0.99567947186812988, 0.0928554753392902), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, 0.99675795875308415, 0.080456955339290204), glm::dvec4(1,1,0,0), 0, 0.01239852);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, -0.99567947186812988, 0.0928554753392902), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    c = RAYX::Ray(glm::dvec3(0.0, 1.0, 0.0), glm::dvec3(0.01239852, 0.99675795875308415, 0.080456955339290204), glm::dvec4(1,1,0,0), 0, 0.01239852);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.0004999999166666, -0.99558611855684065, 0.093851108341926226), glm::dvec4(1,1,0,0), 0, 0.01239852);
    c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.0004999999166666, 0.99667709206767885, 0.08145258834192623), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    correct.push_back(c);

    r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.0004999999166666, -0.995586229182718, 0.093851118714515264), glm::dvec4(1,1,0,0), 0, 0.01239852);
    c = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.0004999999166666, 0.9966772027014974, 0.081452598714515267), glm::dvec4(1,1,0,0), 0, 0.01239852);
    testValues.push_back(r);
    correct.push_back(c);
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("TestPlaneRefrac", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    double tolerance = 1e-12;
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, iteratToTest) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    double settings = 20;

    // normal (always 0,1,0) encoded in ray position, a encoded in direction.x, direction.y and direction.z are actual ray directions
    RAYX::Ray r = RAYX::Ray(glm::dvec3(-0.0175, 1736.4751598838836, -9848.1551798768887), glm::dvec3(-0.00026923073232438285, -0.17315574581145807, 0.984894418304465), glm::dvec4(1,1,0,0), 1.0, 100);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(-2.7173752216893443, 0.050407875158271054, 28.473736158432885), glm::dvec3(-0.00026923073232438285, -0.17315574581145807, 0.984894418304465), glm::dvec4(1,1,0,0), 1.0, 100);
    testValues.push_back(r);
    correct.push_back(c);

    double longRadius = 10470.491787499999;
    double shortRadius = 315.72395939400002;
    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("TestPlaneRefrac", std::vector<double>{ longRadius,shortRadius,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    double tolerance = 1e-09;
    compareFromCorrect(correct, outputRays, tolerance);
}

TEST(Tracer, getThetaTest) {
    std::vector<RAYX::Ray> testValues;
    std::vector<RAYX::Ray> correct;
    double settings = 21;

    RAYX::Ray r = RAYX::Ray(glm::dvec3(0, 1, 0), glm::dvec3(-0.00049999997222222275, -0.17381228817387082, 0.98477867487054738), glm::dvec4(1,1,0,0), 1.0, 100);
    RAYX::Ray c = RAYX::Ray(glm::dvec3(0,1,0), glm::dvec3(-0.00049999997222222275, -0.17381228817387082, 0.98477867487054738), glm::dvec4(1,1,0,0), 1.0, 1.3960967569703167);
    testValues.push_back(r);
    correct.push_back(c);

    std::shared_ptr<RAYX::OpticalElement> q = std::make_shared<RAYX::OpticalElement>("getThetaTest", std::vector<double>{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,settings,0,0 }, zeros, zeros, zeros, zeros, zeros, zeros);

    std::list<double> outputRays = runTracer(testValues, { q });
    std::cout << "got " << outputRays.size() << " values from shader" << std::endl;

    double tolerance = 1e-09;
    compareFromCorrect(correct, outputRays, tolerance);

}


void testOpticalElement(std::vector<std::shared_ptr<RAYX::OpticalElement>> elements, int n) {
    std::shared_ptr<RAYX::MatrixSource> m = std::make_shared<RAYX::MatrixSource>(0, "Matrix source 1", n, 0, 0.065, 0.04, 0.0, 0.001, 0.001, 100, 0, 1, 0, 0, std::vector<double>{ 0,0,0,0 });

    std::list<double> outputRays = runTracer(m->getRays(), elements);
    std::string filename = "testFile_";
    std::cout << elements[0]->getName();
    filename.append(elements[0]->getName());
    writeToFile(outputRays, filename);
}
// test complete optical elements instead of single functions
// uses deterministic source (matrix source with source depth = 0)
// use name of optical element as file name

TEST(opticalElements, planeMirrorDefault) {
    GeometricUserParams pm_param = GeometricUserParams(10, 10, 7.5, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 pos_mirror = calcPosition(pm_param);
    glm::dmat4x4 or_mirror = calcOrientation(pm_param);
    std::shared_ptr<RAYX::PlaneMirror> plM = std::make_shared<RAYX::PlaneMirror>("PlaneMirrorDef", 50, 200, pos_mirror, or_mirror, zeros7); // {1,2,3,0.01,0.02,0.03}
    
    plM->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pm_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plM }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, planeMirrorMis) {
    GeometricUserParams pm_param = GeometricUserParams(10, 10, 0, 10000, std::vector<double>{ 1,2,3,0.001,0.002,0.003 });
    glm::dvec4 pos_mirror = calcPosition(pm_param);
    glm::dmat4x4 or_mirror = calcOrientation(pm_param);
    
    std::shared_ptr<RAYX::PlaneMirror> plM = std::make_shared<RAYX::PlaneMirror>("PlaneMirrorMis", 50, 200, pos_mirror, or_mirror, zeros7); // {1,2,3,0.01,0.02,0.03}
    plM->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pm_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plM }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, sphereMirror) {
    double grazingIncidence = 10;
    GeometricUserParams sm_param = GeometricUserParams(grazingIncidence, grazingIncidence, 0, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 pos_mirror = calcPosition(sm_param);
    glm::dmat4x4 or_mirror = calcOrientation(sm_param);
    
    std::shared_ptr<RAYX::SphereMirror> s = std::make_shared<RAYX::SphereMirror>("SphereMirrorDefault", 50, 200, sm_param.alpha, pos_mirror, or_mirror, 10000, 1000, zeros7);
    s->setOutMatrix(glmToVector16(glm::transpose(calcE2B(sm_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ s }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, planeGratingDevDefault) {
    double incidenceAngle = degree(1.4773068838645145);
    double exitAngle = degree(1.4897528445258457);
    GeometricUserParams pg_param = GeometricUserParams(incidenceAngle, exitAngle, 0, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(pg_param);
    glm::dmat4x4 orientation = calcOrientation(pg_param);
    
    std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationDefault", 50, 200, position, orientation, 100, 1000, 1, 0, std::vector<double>{ 0,0,0,0,0,0 }, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    //std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationDefault", 0, 50, 200, 10, 0.0, 0.0, 10000, 100, 1000, 1, 2, 0, std::vector<double>{ 0,0,0,0,0,0 }, std::vector<double>{ 0,0,0,0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    plG->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pg_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plG }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, planeGratingDevAzimuthal) {
    double incidenceAngle = degree(1.4773068838645145);
    double exitAngle = degree(1.4897528445258457);
    GeometricUserParams pg_param = GeometricUserParams(incidenceAngle, exitAngle, 7.5, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(pg_param);
    glm::dmat4x4 orientation = calcOrientation(pg_param);

    std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationAz", 50, 200, position, orientation, 100, 1000, 1, 0, std::vector<double>{ 0,0,0,0,0,0 }, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    //std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationAz", 0, 50, 200, 10, 0.0, 7.5, 10000, 100, 1000, 1, 2, 0, std::vector<double>{ 0,0,0,0,0,0 }, std::vector<double>{ 0,0,0,0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    plG->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pg_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plG }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, planeGratingDevAzMis) {
    double incidenceAngle = degree(1.4773068838645145);
    double exitAngle = degree(1.4897528445258457);
    GeometricUserParams pg_param = GeometricUserParams(incidenceAngle, exitAngle, 7.5, 10000, std::vector<double>{ 1,2,3,0.001,0.002,0.003 });
    glm::dvec4 position = calcPosition(pg_param);
    glm::dmat4x4 orientation = calcOrientation(pg_param);

    std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationAzMis", 50, 200, position, orientation, 100, 1000, 1, 0, std::vector<double>{ 0,0,0,0,0,0 }, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    
    //std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationAzMis", 0, 50, 200, 10, 0.0, 7.5, 10000, 100, 1000, 1, 2, 0, std::vector<double>{ 1,2,3,0.001,0.002,0.003 }, std::vector<double>{ 0,0,0,0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    plG->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pg_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plG }, 20);
    ASSERT_TRUE(true);
}

// constant incidence angle mode, azimuthal angle and misalignment
TEST(opticalElements, planeGratingIncAzMis) {
    double incidenceAngle = degree(1.3962634015954636);
    double exitAngle = degree(1.4088395764879007);
    GeometricUserParams pg_param = GeometricUserParams(incidenceAngle, exitAngle, 7.5, 10000, std::vector<double>{ 1,2,3,0.001,0.002,0.003 });
    glm::dvec4 position = calcPosition(pg_param);
    glm::dmat4x4 orientation = calcOrientation(pg_param);
    
    std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingIncAzMis", 50, 200, position, orientation, 100, 1000, 1, 0,  std::vector<double>{ 0,0,0,0,0,0 }, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    //std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingIncAzMis", 1, 50, 200, 0.0, 10, 7.5, 10000, 100, 1000, 1, 2, 0, std::vector<double>{ 1,2,3,0.001,0.002,0.003 }, std::vector<double>{ 0,0,0,0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    plG->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pg_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plG }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, planeGratingDevMisVLS) {
    double incidenceAngle = degree(1.4773068838645145);
    double exitAngle = degree(1.4897528445258457);
    GeometricUserParams pg_param = GeometricUserParams(incidenceAngle, exitAngle, 7.5, 10000, std::vector<double>{ 1,2,3,0.001,0.002,0.003 });
    glm::dvec4 position = calcPosition(pg_param);
    glm::dmat4x4 orientation = calcOrientation(pg_param);
    
    std::shared_ptr<RAYX::PlaneGrating> plG = std::make_shared<RAYX::PlaneGrating>("PlaneGratingDeviationMis", 50, 200, position, orientation, 100, 1000, 1, 0, std::vector<double>{ 1,2,3,4,5,6 }, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    plG->setOutMatrix(glmToVector16(glm::transpose(calcE2B(pg_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ plG }, 20);
    ASSERT_TRUE(true);
}

// RZPs 

TEST(opticalElements, RZPDefaultParams) {
    // alpha and beta calculated from user parameters
    GeometricUserParams rzp_param = GeometricUserParams( degree(0.017453292519943295), degree(0.017453292519941554), 0, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(rzp_param);
    glm::dmat4x4 orientation = calcOrientation(rzp_param);
    
    //std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault", 0, 1, 0, 0, 0, 50, 200, 170, 1, 0, 10000, 100, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, 0, 0, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault", 0, 0, 50, 200, position, orientation, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ rzp }, 20);
    ASSERT_TRUE(true);
}

TEST(opticalElements, RZPDefaultParams200) {
    GeometricUserParams rzp_param = GeometricUserParams( degree( 0.017453292519943295), degree(0.017453292519941554), 0, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(rzp_param);
    glm::dmat4x4 orientation = calcOrientation(rzp_param);
    
    //std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault200", 0, 1, 0, 0, 0, 50, 200, 170, 1, 0, 10000, 100, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, 0, 0, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault200", 0, 0, 50, 200, position, orientation, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ rzp }, 200);
    ASSERT_TRUE(true);
}


TEST(opticalElements, RZPAzimuthal200) {
    GeometricUserParams rzp_param = GeometricUserParams( degree( 0.017453292519943295), degree(0.017453292519941554), 10, 10000, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(rzp_param);
    glm::dmat4x4 orientation = calcOrientation(rzp_param);

    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault200", 0, 0, 50, 200, position, orientation, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    // std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateAzim200", 0, 1, 0, 0, 0, 50, 200, 170, 1, 10, 10000, 100, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, 0, 0, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, false); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    testOpticalElement({ rzp }, 200);
    ASSERT_TRUE(true);
}


TEST(opticalElements, RZPMis) {
    GeometricUserParams rzp_param = GeometricUserParams( degree( 0.017453292519943295), degree(0.017453292519941554), 0, 10000, std::vector<double>{ 1,2,3,0.001,0.002,0.003 });
    glm::dvec4 position = calcPosition(rzp_param);
    glm::dmat4x4 orientation = calcOrientation(rzp_param);

    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePlateDefault200", 0, 0, 50, 200, position, orientation, 100, -1, -1, 1, 1, 100, 500, 100, 500, 0, 0, 0, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi // {1,2,3,0.001,0.002,0.003}
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    testOpticalElement({ rzp }, 200);
    ASSERT_TRUE(true);
}

TEST(opticalElements, ImagePlane) {
    std::shared_ptr<RAYX::PlaneMirror> plM = std::make_shared<RAYX::PlaneMirror>("PlaneMirror_ImagePlane", 50, 200, 10, 0, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, false); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::ImagePlane> i = std::make_shared<RAYX::ImagePlane>("ImagePlane", 1000, plM, false);
    testOpticalElement({ plM, i }, 200);
    ASSERT_TRUE(true);
}


TEST(globalCoordinates, FourMirrors_9Rays) {
    std::shared_ptr<RAYX::PlaneMirror> p1 = std::make_shared<RAYX::PlaneMirror>("globalCoordinates_9rays", 50, 200, 10, 7, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, true); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p2 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror2", 50, 200, 15, 4, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p1, true); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p3 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror3", 50, 200, 7, 10, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p2, true); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p4 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror4", 50, 200, 22, 17, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p3, true); // {1,2,3,0.01,0.02,0.03}
    p4->setOutMatrix({ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 });
    testOpticalElement({ p1, p2, p3, p4 }, 9);
    ASSERT_TRUE(true);
}

TEST(globalCoordinates, FourMirrors_20Rays) {
    std::shared_ptr<RAYX::PlaneMirror> p1 = std::make_shared<RAYX::PlaneMirror>("globalCoordinates_20rays", 50, 200, 10, 7, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, nullptr, false); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p2 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror2", 50, 200, 15, 4, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p1, false); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p3 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror3", 50, 200, 7, 10, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p2, false); // {1,2,3,0.01,0.02,0.03}
    std::shared_ptr<RAYX::PlaneMirror> p4 = std::make_shared<RAYX::PlaneMirror>("PlaneMirror4", 50, 200, 22, 17, 10000, std::vector<double>{ 0,0,0, 0,0,0 }, zeros7, p3, false); // {1,2,3,0.01,0.02,0.03}
    // to stay in element coordinates and make a comparison with old RAY possible
    p4->setOutMatrix({ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 });
    testOpticalElement({ p1, p2, p3, p4 }, 20);
    ASSERT_TRUE(true);
}


TEST(opticalElements, slit1) {
    std::shared_ptr<RAYX::MatrixSource> m = std::make_shared<RAYX::MatrixSource>(0, "matrix source", 200, 0, 0.065, 0.04, 0, 0.001, 0.001, 100, 0, 1, 0, 0, std::vector<double>{ 0,0,0,0 });
    std::shared_ptr<RAYX::Slit> s = std::make_shared<RAYX::Slit>("slit", 0, 1, 20, 2, 0, 10000, 20, 1, m->getPhotonEnergy(), std::vector<double>{ 0,0,0, 0,0,0 }, nullptr, true);
    std::shared_ptr<RAYX::ImagePlane> ip = std::make_shared<RAYX::ImagePlane>("Image plane", 1000, s, true);
    std::list<double> outputRays = runTracer(m->getRays(), {s,ip});
    int counter = 0;
    for (std::list<double>::iterator i = outputRays.begin(); i != outputRays.end();) {
        // only if weight == 1
        if(counter & RAY_DOUBLE_COUNT == 0) {
            std::list<double>::iterator j = i;
            j++;
            j++;
            if(*(j) == 1)
                EXPECT_TRUE(abs(*i) <= 6);
        }else if (counter % RAY_DOUBLE_COUNT == 1) { // y loc
            std::list<double>::iterator j = i;
            j++;
            j++;
            j++;
            if(*j == 1) {
                EXPECT_TRUE(abs(*i) >= 0.5);
                EXPECT_TRUE(abs(*i) <= 1.3);
            }
        }
        counter++;
        i++;
    }
}


// PETES SETUP
// spec1-first_rzp4mm
TEST(PeteRZP, spec1_first_rzp) {
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.017453292519943295), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 position = calcPosition(rzp_param);
    glm::dmat4x4 orientation = calcOrientation(rzp_param);

    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_rzp", 20000 , 1, 0.005,0.005,0, 0.02,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, position, orientation, p->getPhotonEnergy(), -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 1, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    std::list<double> outputRays = runTracer(p->getRays(), {rzp});
    std::string filename = "testFile_spec1_first_rzp";
    writeToFile(outputRays, filename);
}

TEST(PeteRZP, spec1_first_ip) {
    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_rzp4",20000 , 1, 0.005,0.005,0, 0.02,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.017453292519943295), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 rzp_position = calcPosition(rzp_param);
    glm::dmat4x4 rzp_orientation = calcOrientation(rzp_param);
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, rzp_position, rzp_orientation, p->getPhotonEnergy(), -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 1, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    
    GeometricUserParams ip_param = GeometricUserParams(0, 0, 0, 385.0, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 ip_position = calcPosition(ip_param, rzp_param, rzp_position, rzp_orientation);
    glm::dmat4x4 ip_orientation = calcOrientation(ip_param, rzp_param, rzp_position, rzp_orientation);
    std::shared_ptr<RAYX::ImagePlane> ip1 = std::make_shared<RAYX::ImagePlane>("ImagePlane1", ip_position, ip_orientation);
    
    std::vector<RAYX::Ray> input = p->getRays();
    std::list<double> outputRays = runTracer(input, {rzp, ip1});
    std::string filename = "testFile_spec1_first_rzp_ip";
    writeToFile(outputRays, filename);
}

TEST(PeteRZP, spec1_first_plus_rzp) {
    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_plus_rzp",20000 , 1, 0.005,0.005,0, 0.02,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.082903139469730644), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 rzp_position = calcPosition(rzp_param);
    glm::dmat4x4 rzp_orientation = calcOrientation(rzp_param);
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, rzp_position, rzp_orientation, p->getPhotonEnergy(), 1, 1, 2.2, 4.75, 90, 400, 90, 400, 0, 0, 1, -24.35, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    
    //std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 1, 0, 1, 1, 4, 60, 170, 2.2, 0, 90, p->getPhotonEnergy(), p->getPhotonEnergy(), 1, 1, 2.2, 4.75, 90, 400, 90, 400, 0, 0, 1, 0, -24.35, 4.75, std::vector<double>{ 0,0,0, 0,0,0 },  std::vector<double>{0,0,0,0, 0,0,0}, nullptr, true);  // dx,dy,dz, dpsi,dphi,dchi //
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    std::list<double> outputRays = runTracer(p->getRays(), {rzp});
    std::string filename = "testFile_spec1_first_plus_rzp";
    writeToFile(outputRays, filename);
}

TEST(PeteRZP, spec1_first_plus_ip) {
    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_plus_rzp_ip",20000 , 1, 0.005,0.005,0, 0.02,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.082903139469730644), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 rzp_position = calcPosition(rzp_param);
    glm::dmat4x4 rzp_orientation = calcOrientation(rzp_param);
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, rzp_position, rzp_orientation, p->getPhotonEnergy(), 1, 1, 2.2, 4.75, 90, 400, 90, 400, 0, 0, 1, -24.35, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    
    GeometricUserParams ip_param = GeometricUserParams(0, 0, 0, 400.0, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 ip_position = calcPosition(ip_param, rzp_param, rzp_position, rzp_orientation);
    glm::dmat4x4 ip_orientation = calcOrientation(ip_param, rzp_param, rzp_position, rzp_orientation);
    std::shared_ptr<RAYX::ImagePlane> ip1 = std::make_shared<RAYX::ImagePlane>("ImagePlane1", ip_position, ip_orientation);

    std::vector<RAYX::Ray> input = p->getRays();
    std::list<double> outputRays = runTracer(input, {rzp, ip1});
    std::string filename = "testFile_spec1_first_plus_rzp_ip";
    writeToFile(outputRays, filename);
}

TEST(PeteRZP, spec1_first_minus_rzp2) {
    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_minus_rzp2",20000 , 1, 0.005,0.005,0, 0.001,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.017453292519943295), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 rzp_position = calcPosition(rzp_param);
    glm::dmat4x4 rzp_orientation = calcOrientation(rzp_param);
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, rzp_position, rzp_orientation, p->getPhotonEnergy(), -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 1, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    rzp->setOutMatrix(glmToVector16(glm::transpose(calcE2B(rzp_param)))); // to make comparison with old ray files possible, use the beam coordinate system
    
    //std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 1, 0, 1, 1, 2, 60, 170, 2.2, 0, 90, p->getPhotonEnergy(), p->getPhotonEnergy(), -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 1, 0, 0, 1, std::vector<double>{ 0,0,0, 0,0,0 },  std::vector<double>{0,0,0,0, 0,0,0}, nullptr, true);  // dx,dy,dz, dpsi,dphi,dchi //
    std::list<double> outputRays = runTracer(p->getRays(), {rzp});
    std::string filename = "testFile_spec1_first_minus_rzp2";
    writeToFile(outputRays, filename);
}

TEST(PeteRZP, spec1_first_minus_ip2) {
    std::shared_ptr<RAYX::PointSource> p = std::make_shared<RAYX::PointSource>(0, "spec1_first_minus_rzp_ip2",20000 , 1, 0.005,0.005,0, 0.001,0.06, 1,1,0,0, 640, 120, 1, 0, 0, std::vector<double>{0,0,0,0});
    
    GeometricUserParams rzp_param = GeometricUserParams(degree(0.038397243543875255), degree(0.017453292519943295), 0, 90, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 rzp_position = calcPosition(rzp_param);
    glm::dmat4x4 rzp_orientation = calcOrientation(rzp_param);
    std::shared_ptr<RAYX::ReflectionZonePlate> rzp = std::make_shared<RAYX::ReflectionZonePlate>("ReflectionZonePete", 0, 0, 4, 60, rzp_position, rzp_orientation, p->getPhotonEnergy(), -1, -1, 2.2, 1, 90, 400, 90, 400, 0, 0, 1, 0, zeros7); // dx,dy,dz, dpsi,dphi,dchi //
    
    GeometricUserParams ip_param = GeometricUserParams(0, 0, 0, 400.0, std::vector<double>{ 0,0,0, 0,0,0 });
    glm::dvec4 ip_position = calcPosition(ip_param, rzp_param, rzp_position, rzp_orientation);
    glm::dmat4x4 ip_orientation = calcOrientation(ip_param, rzp_param, rzp_position, rzp_orientation);
    std::shared_ptr<RAYX::ImagePlane> ip1 = std::make_shared<RAYX::ImagePlane>("ImagePlane1", ip_position, ip_orientation);
    
    std::vector<RAYX::Ray> input = p->getRays();
    std::list<double> outputRays = runTracer(input, {rzp, ip1});
    std::string filename = "testFile_spec1_first_minus_rzp_ip2";
    writeToFile(outputRays, filename);
}
*/