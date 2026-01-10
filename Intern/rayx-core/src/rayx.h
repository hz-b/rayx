#pragma once

////////////////////////////////////////////////////////////
// includes
////////////////////////////////////////////////////////////

#include "Core.h"

#include "Design/Design.h"
#include "Trace/Trace.h"
#include "Math/Math.h"
#include "IO/IO.h"
#include "Misc/Misc.h"

////////////////////////////////////////////////////////////
// namespace aggregation
////////////////////////////////////////////////////////////

namespace rayx {

// re-export modules
using namespace design;
using namespace tracer;
using namespace math;
using namespace io;
using namespace misc;

// re-export literals
namespace literals {
    using namespace design::literals;
    using namespace tracer::literals;
    using namespace math::literals;
    using namespace io::literals;
    using namespace misc::literals;
}

} // namespace rayx
