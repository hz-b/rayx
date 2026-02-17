#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include <array>
#include <filesystem>
#include <optional>
#include <rapidxml.hpp>
#include <vector>

#include "Design/Angle.h"
#include "Design/PhotonEnergy.h"
#include "Design/Coating.h"
#include "Design/Cutout.h"
#include "Design/Surface.h"
#include "Design/Material.h"
#include "Math/Constants.h"
#include "Design/Source.h"
#include "Design/SlopeError.h"

#include "Core.h"
#include "Beamline.h"

namespace rayx {

Beamline importBeamline(const std::filesystem::path& filepath);

}