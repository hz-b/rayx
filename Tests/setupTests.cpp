#include "setupTests.h"

void check_eq(std::string filename, int line, std::string l, std::string r, std::vector<double> vl, std::vector<double> vr, double tolerance) {
	if (vl.size() != vr.size()) {
		RAYX::Warn(filename, line) << l << " != " << r << ": different lengths!";
		EXPECT_TRUE(false);
		return;
	}

	bool success = true;
	for (size_t i = 0; i < vl.size(); i++) {
		if (abs(vl[i] - vr[i]) > tolerance) {
			success = false;
			break;
		}
	}
	if (success) return;

	RAYX::Warn(filename, line) << l << " != " << r << ":";

	int counter = 0; // stores the number of elements in the stringstream
	std::stringstream s;
	for (size_t i = 0; i < vl.size(); i++) {
		if (counter != 0) { s << " "; }
		if (abs(vl[i]-vr[i]) <= tolerance) {
			s << vl[i] << "|" << vr[i];
		} else {
			s << "\x1B[36m" << vl[i] << "|" << vr[i] << "\x1B[31m";
		}

		counter++;
		if (counter == 4 && vl.size() == 16) { // 4x4 things should be written in 4 rows
			counter = 0;
			RAYX::Warn(filename, line) << s.str();
			s = std::stringstream();
		}
	}
	if (counter > 0) {
		RAYX::Warn(filename, line) << s.str();
	}
}
