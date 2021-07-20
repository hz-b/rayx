#pragma once

#include "Core.h"
#include "Tracer/TracerInterface.h"

namespace RAYX
{

	class RAY_API Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run() = 0;
		/*void run(const std::string& file);*/

	protected:
		TracerInterface m_tracerInterface;

	};

} // namespace RAYX