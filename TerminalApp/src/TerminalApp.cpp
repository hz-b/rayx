#include "TerminalApp.h"



TerminalApp::TerminalApp()
{

}

TerminalApp::~TerminalApp()
{

}

void TerminalApp::Run()
{
	while (true);

	// do config stuff
	// create terminal
	// run terminal
		// wait for commands
		// on command do stuff with beamline, tracer, analyser
	// on exit close terminal
}

RAY::Application* RAY::CreateApplication()
{
	return new TerminalApp();
}