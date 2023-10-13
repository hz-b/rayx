#include "Application.h"

int main(int argc, char** argv) {
    Application app(1920, 1080, "RayX-UI", argc, argv);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}