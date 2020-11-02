#include <RayCore.h>

class TerminalApp : public RAY::Application
{
public:
	TerminalApp()
	{

	}

	~TerminalApp()
	{

	}

};

RAY::Application* RAY::CreateApplication()
{
	return new TerminalApp();
}