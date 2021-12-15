#pragma once

#include <memory>

#include "Core.h"
#include "Model/Beamline/Beamline.h"

namespace RAYX {

class RAYX_API Importer {
  public:
    Importer();
    ~Importer();

    static Beamline importBeamline(const char* filename);

  private:
};

}  // namespace RAYX