#include <nfd.h>

#include <clocale>

#include "Application.h"

int main(int argc, char** argv) {
    NFD_Init();  // Initialize Native File Dialog

    // Set locale to C to avoid issues with scanf
    std::setlocale(LC_NUMERIC, "C");

    Application app(1920, 1080, "RAYX-UI", argc, argv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    NFD_Quit();
    return EXIT_SUCCESS;
}