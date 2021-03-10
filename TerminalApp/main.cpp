#include <iostream>

#include "TerminalApp.h"

int main(/*int argc, char** argv*/)
{
	auto app = TerminalApp();
	app.Run();
    std::cout << "TerminalApp finished!" << std::endl;
}