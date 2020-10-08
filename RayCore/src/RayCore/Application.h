#pragma once

#include "Core.h"

namespace RAY {

	class RAY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}