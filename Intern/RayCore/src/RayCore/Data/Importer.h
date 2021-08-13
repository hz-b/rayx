#pragma once

#include <memory>

#include "Beamline/Beamline.h"
#include "Core.h"

namespace RAYX
{

	class RAYX_API Importer
	{
	public:
		Importer();
		~Importer();

		static Beamline importBeamline();

	};

} // namespace RAYX