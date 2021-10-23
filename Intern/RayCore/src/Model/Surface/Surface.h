#pragma once

#include "Core.h"
#include <vector>

namespace RAYX
{
    /** An abstract class used as a baseline for all surfaces.
     *
     *  # This is a headline
     *  TODO(Jannis): Here will be a more detailed description,
     *  which will continue here.
     *
     */
    class RAYX_API Surface {
    public:
        virtual std::vector<double> getParams() const = 0;

        Surface();
        virtual ~Surface();
    private:

    };
} // namespace RAYX