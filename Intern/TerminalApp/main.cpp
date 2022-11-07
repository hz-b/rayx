#include "TerminalApp.h"

int main(int argc, char** argv) {
    RAYX_PROFILE_BEGIN_SESSION("RAY-X", "results.json");

    TerminalApp app = TerminalApp(argc, argv);
    app.run();

    RAYX_PROFILE_END_SESSION();
    return 0;
}