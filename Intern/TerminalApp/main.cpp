#include "TerminalApp.h"
#include "Debug.h"


int main(int argc, char** argv)
{
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	TerminalApp app = TerminalApp(argc, argv);
	app.run(/*app.getProvidedFilePath()*/);

	return 0;
}