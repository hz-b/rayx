#include "TerminalApp.h"



TerminalApp::TerminalApp()
{

}

TerminalApp::~TerminalApp()
{

}

RAY::Application* RAY::CreateApplication()
{
	return new TerminalApp();
}