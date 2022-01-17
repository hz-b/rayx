#pragma once

#include <vector>
#include <array>

#include "Core.h"

namespace RAYX {
/** An abstract class used as a baseline for all surfaces.
 *
 *  # This is a headline
 *  TODO(Jannis): Here will be a more detailed description,
 *  which will continue here.
 *
 */
class RAYX_API Surface {
  public:
    virtual std::array<double, 4*4> getParams() const = 0;

    Surface();
    virtual ~Surface();

  private:
};
}  // namespace RAYX
