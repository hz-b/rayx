#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Core.h"
#include "Model/Beamline/Beamline.h"
#include "Tracer/RayList.h"

namespace RAYX {

class RAYX_API Tracer {
  public:
	Tracer() {}
	virtual ~Tracer() {}
    virtual RayList trace(const Beamline&) = 0;
};

}  // namespace RAYX
