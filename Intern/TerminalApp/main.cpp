#include "TerminalApp.h"

int main(int argc, char** argv) {
#ifdef RAYX_PLATFORM_WINDOWS
#ifdef RAY_DEBUG_MODE
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif
    RAYX_PROFILE_BEGIN_SESSION("Startup", "rayx_profile_startup.json");
    TerminalApp app = TerminalApp(argc, argv);
    RAYX_PROFILE_END_SESSION();

    RAYX_PROFILE_BEGIN_SESSION("Runtime", "rayx_profile_runtime.json");
    app.run();
    RAYX_PROFILE_END_SESSION();

    RAYX_PROFILE_BEGIN_SESSION("Shutdown", "rayx_profile_shutdown.json");
    return 0;
    RAYX_PROFILE_END_SESSION();
}