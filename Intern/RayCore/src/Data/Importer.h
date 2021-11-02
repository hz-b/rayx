#pragma once

#include <memory>

#include "Model/Beamline/Beamline.h"
#include "Core.h"

namespace RAYX
{

	class RAYX_API Importer
	{
	public:
		Importer();
		~Importer();

		static Beamline importBeamline(const char* filename);

	private:

	};

} // namespace RAYX