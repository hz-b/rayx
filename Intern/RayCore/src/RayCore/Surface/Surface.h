#pragma once

#include "Core.h"
#include <vector>

namespace RAY
{
    class RAY_API Surface {
    public:
        virtual std::vector<double> getParams() const = 0;

        Surface();
        virtual ~Surface();
    private:

    };
} // namespace RAY
