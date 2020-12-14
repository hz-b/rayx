#pragma once
#include <iostream>

extern RAY::Application* RAY::CreateApplication();

int main(/*int argc, char** argv*/)
{
	auto app = RAY::CreateApplication();
	app->Run();
	delete app;
}
