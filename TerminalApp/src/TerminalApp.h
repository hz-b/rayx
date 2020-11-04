#pragma once 
#include "RayCore.h"
#include <stdio.h>

using namespace std;

class TerminalApp : public RAY::Application
{
public:
	TerminalApp();
	~TerminalApp();

	void Run();

private:
	int executeCommand(string[]& argv);
	string argumentParser(string[]& argv);

	int loadCMD(string[]& argv);
	int runCMD(string[]& argv);
	int exitCMD(string[]& argv);

};