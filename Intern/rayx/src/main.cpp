#include "TerminalApp.h"

int main(int argc, char** argv) {
    // RAYX_PROFILE_FUNCTION_STDOUT(); This somehow records the computers uptime
    TerminalApp app = TerminalApp(argc, argv);
    app.run();

    return 0;
}