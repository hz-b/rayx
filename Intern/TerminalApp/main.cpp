#include "TerminalApp.h"

int main(int argc, char** argv)
{
	TerminalApp app = TerminalApp(argc, argv);
	app.run(/*app.getProvidedFilePath()*/);
}