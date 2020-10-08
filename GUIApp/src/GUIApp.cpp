#include <iostream>
#include <RayCore.h>

class Sandbox : public RAY::Application // rename Sandbox -> GUIApp (also with TerminalApp)
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}

};

RAY::Application* RAY::CreateApplication()
{
	return new Sandbox();
}