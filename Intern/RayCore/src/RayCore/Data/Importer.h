#pragma once

#include <memory>

#include "Beamline/Beamline.h"
#include "Core.h"

namespace RAYX
{

	class RAYX_API Importer
	{
	public:
		Importer(std::shared_ptr<Beamline> Bl);
		~Importer();

		void importBeamline();

	private:
		std::shared_ptr<Beamline> m_Beamline;


	};

} // namespace RAYX