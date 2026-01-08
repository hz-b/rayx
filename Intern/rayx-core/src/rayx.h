#pragma once

// include core headers
#include "Core.h"

// include design headers
#include "Design/Design.h"

// expose rayx::design and rayx::design::literals in rayx namespace
namespace rayx {
using namespace design;
namespace literals {
    using namespace design::literals;
}
}

// include tracer headers
#include "Trace/Trace.h"

// expose rayx::tracer and rayx::tracer::literals in rayx namespace
namespace rayx {
using namespace tracer;
namespace literals {
    using namespace tracer::literals;
}
}
