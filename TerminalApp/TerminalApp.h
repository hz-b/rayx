#pragma once 
#include "RayCore.h"

class TerminalApp : public RAY::Application
{
public:
	TerminalApp();
	TerminalApp(int argc, char** argv);
	~TerminalApp();

	const std::string& getProvidedFilePath() const { return providedFile; };

private:
	int m_argc;
	char** m_argv;

	std::string providedFile;
};