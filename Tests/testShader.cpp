#include "setupTests.h"
#include "Tracer/CpuTracer.h"

// TODO(rudi): shader tests

namespace RAYX {
	namespace CPP_TRACER {
		double r8_exp(double);
		double r8_log(double);
		double squaresDoubleRNG(uint64_t&);
	}
}

using namespace RAYX::CPP_TRACER;

TEST_F(TestSuite, testUniformRandom) {
	uint64_t ctr = 13;
	double old = 0;

	for (int i = 0; i < 100; i++) {
		double d = squaresDoubleRNG(ctr);
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
		CHECK_EQ(r8_exp(x), exp(x));
		CHECK_EQ(r8_exp(-x), exp(-x));
	}
}

// interestingly r8_log and log behave differently on input 0, namely 1 vs -inf.
TEST_F(TestSuite, LogTest) {
	std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0000001};
	for (auto x : args) {
		CHECK_EQ(r8_log(x), log(x));
	}
}
