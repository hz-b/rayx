#pragma once 
#include "RayCore.h"

class TerminalApp : public RAY::Application
{
public:
	TerminalApp();
	TerminalApp(int argc, char** argv);
	~TerminalApp();

	void run() override;

	const std::string& getProvidedFilePath() const { return providedFile; };

private:
	char** m_argv;
	int m_argc;
	std::string providedFile;
};