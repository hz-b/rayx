#include <iostream>

#include "TerminalApp.h"

int main(int argc, char** argv)
{
	auto app = TerminalApp(argc, argv);
	app.run(app.getProvidedFilePath());
	std::cout << "TerminalApp finished!" << std::endl;
}