#pragma once

#include "Debug.h"

namespace RAYX {

class RAYX_API CpuTracer : public Tracer {
	RayList trace(const Beamline&) {
		RAYX_ERR << "oh no!";
		return {};
	}
};

};
