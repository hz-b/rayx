#include "setupTests.h"
#include "Tracer/CpuTracer.h"

// TODO(rudi): shader tests

namespace RAYX {
	namespace CPP_TRACER {
		double r8_exp(double);
	}
}

using namespace RAYX::CPP_TRACER;

TEST_F(TestSuite, ExpTest) {
	std::vector<double> args = {10.0, 5.0, 2.0, 1.0, 0.5, 0.0001, 0.0};
	for (auto x : args) {
		CHECK_EQ(r8_exp(x), exp(x));
		CHECK_EQ(r8_exp(-x), exp(-x));
	}
}
