#include "TerminalApp.h"

#include "Tracer/Ray.h"
#include <type_traits>
#include <iostream>

int main(int argc, char** argv) {
    TerminalApp app = TerminalApp(argc, argv);
    app.run();
    return 0;
}