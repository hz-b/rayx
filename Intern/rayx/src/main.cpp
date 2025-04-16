#include "TerminalApp.h"

int main(int argc, char** argv) {
    RAYX_PROFILE_FUNCTION_STDOUT();
    TerminalApp app = TerminalApp(argc, argv);
    app.run();

    return 0;
}
