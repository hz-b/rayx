#include "setupTests.h"
#include "Tracer/CpuTracer.h"

// TODO(rudi): shader tests


namespace RAYX {
	namespace CPP_TRACER {
		double r8_exp(double);
		double r8_log(double);
		double squaresDoubleRNG(uint64_t&);
		Ray refrac2D(Ray, glm::dvec4, double, double);
	}
}

using namespace RAYX;

TEST_F(TestSuite, testUniformRandom) {
	uint64_t ctr = 13;
	double old = 0;

	for (int i = 0; i < 100; i++) {
		double d = CPP_TRACER::squaresDoubleRNG(ctr);
		if (d == old) {
			RAYX_WARN << "repeating number in testUniformRandom! " << d;
		}
		if (d < 0.0 || d > 1.0) {
			RAYX_ERR << "random number out of range [0, 1]: " << d;
		}
		old = d;
	}
}

TEST_F(TestSuite, ExpTest) {
	std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
	for (auto x : args) {
		CHECK_EQ(CPP_TRACER::r8_exp(x), exp(x));
		CHECK_EQ(CPP_TRACER::r8_exp(-x), exp(-x));
	}
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, LogTest) {
	std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
	for (auto x : args) {
		CHECK_EQ(CPP_TRACER::r8_log(x), log(x));
	}
}

TEST_F(TestSuite, testRefrac2D) {
    glm::dvec4 normal = glm::dvec4(0, 1, 0, 0);
    glm::dvec3 direction = glm::dvec3(
        0.0001666666635802469, -0.017285764670739875, 0.99985057611723738);
    double weight = 1.0;
    double az = 0.00016514977645243345;
    double ax = 0.012830838024391771;
	Ray r = Ray::makeRayFrom(glm::dvec3(0, 1, 0), direction, glm::dvec4(az, ax, 0, 0), 0, weight, 0, 0, 0, 0);

	auto out = RAYX::CPP_TRACER::refrac2D(r, normal, az, ax);
	Ray correct;
	CHECK_EQ(out, correct);
}
