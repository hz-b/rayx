// This is currently just to establish the base structure of the project.

#include <RAY-Core.h>
#include <Random.h>

#include <iostream>

int main() {
    RAYX::randomSeed();
    std::cout << RAYX::randomDouble() << std::endl;
}