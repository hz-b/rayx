#pragma once

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Tracer/TracerInterface.h"

namespace RAYX
{
	class RAYX_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run() = 0;

	protected:
		TracerInterface m_TracerInterface;
		std::shared_ptr<Beamline> m_Beamline;

	};

} // namespace RAYX