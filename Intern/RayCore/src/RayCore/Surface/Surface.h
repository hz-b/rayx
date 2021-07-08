#pragma once

#include "Core.h"

namespace RAY
{
    class RAY_API Surface {
    public:
        virtual std::vector<double> getParams() = 0;
    private:

    };
} // namespace RAY
