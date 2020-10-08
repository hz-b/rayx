#include <RayCore.h>

class Sandbox : public RAY::Application
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