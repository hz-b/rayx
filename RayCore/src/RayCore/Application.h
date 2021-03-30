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

		void run();
		void run(const std::string& file);

	private:
		TracerInterface m_tracerInterface;
	};

} // namespace RAY