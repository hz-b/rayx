#include "TerminalApp.h"
#include "Debug.h"


int main(int argc, char** argv)
{
#ifdef RAY_PLATFORM_WINDOWS 
#ifdef RAY_DEBUG_MODE
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif

	TerminalApp app = TerminalApp(argc, argv);
	app.run(/*app.getProvidedFilePath()*/);

	return 0;
}