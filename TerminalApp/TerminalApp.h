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
	// int executeCommand(char** argv);
	// char** argumentParser(char* argv);

	// int loadCMD(char** argv);
	// int runCMD(char** argv);
	// int exitCMD(char** argv);
	int m_argc;
	char** m_argv;

	std::string providedFile;
};