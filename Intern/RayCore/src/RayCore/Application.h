#pragma once

#include "Beamline/Beamline.h"
#include "Core.h"
#include "Tracer/TracerInterface.h"

namespace RAYX
{
	/**
	 *	Abstract class to be used by applications built with the RayCore library.
	 *	It currently (13.09.2021) doesn't do anything, as we are still far from creating
	 *	user applications.
	 */
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