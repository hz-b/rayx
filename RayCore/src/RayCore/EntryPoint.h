#pragma once
#include <iostream>

#ifdef RAY_PLATFORM_WINDOWS

extern RAY::Application* RAY::CreateApplication();

int main(/*int argc, char** argv*/)
{
	std::cout << "Hello World!" << std::endl;
	auto app = RAY::CreateApplication();
	app->Run();
	delete app;
}

#endif