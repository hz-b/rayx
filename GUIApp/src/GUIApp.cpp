#include <iostream>
#include <RayCore.h>

class GUIApp : public RAY::Application // rename Sandbox -> GUIApp (also with TerminalApp)
{
public:
	GUIApp()
	{

	}

	~GUIApp()
	{

	}

};

RAY::Application* RAY::CreateApplication()
{
	return new GUIApp();
}