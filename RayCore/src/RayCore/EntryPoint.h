#pragma once
#include <iostream>

#ifdef RAY_PLATFORM_WINDOWS

extern RAY::Application* RAY::CreateApplication();

int main(/*int argc, char** argv*/)
{
	auto app = RAY::CreateApplication();
	app->Run();
	delete app;
}

#endif