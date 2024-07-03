#include <clocale>

#include "Application.h"

int main(int argc, char** argv) {
    // Set locale to C to avoid issues with scanf
    std::setlocale(LC_NUMERIC, "C");

    Application app(1920, 1080, "RAYX-UI", argc, argv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}