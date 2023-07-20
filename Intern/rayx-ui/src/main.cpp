#include "Application.h"

int main() {
    Application app(1920, 1080, "RayX-UI");

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}