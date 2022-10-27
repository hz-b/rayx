#include "TerminalApp.h"

/**
 * @brief Wrapper of the main RAY-X Application for any global execution (e.g Profiling)
 *
 */
struct RAYXWrapper {
    RAYXWrapper() { RAYX_PROFILE_BEGIN_SESSION("RAY-X", "results.json"); }
    ~RAYXWrapper() { RAYX_PROFILE_END_SESSION(); }
};

RAYXWrapper rayxwrrapper;

int main(int argc, char** argv) {
    TerminalApp app = TerminalApp(argc, argv);
    app.run();
    return 0;
}