#pragma once

#include "Core.h"
#include "Tracer/TracerInterface.h"

namespace RAY
{

	class RAY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		TracerInterface m_tracerInterface;
	};

	// To be defined in CLIENT
	Application *CreateApplication();

} // namespace RAY