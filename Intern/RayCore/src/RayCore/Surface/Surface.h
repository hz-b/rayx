#pragma once

#include "Core.h"
#include <vector>

namespace RAYX
{
    class RAYX_API Surface {
    public:
        virtual std::vector<double> getParams() const = 0;

        Surface();
        virtual ~Surface();
    private:

    };
} // namespace RAYX
