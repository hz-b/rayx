// This is currently just to establish the base structure of the project.

#include <iostream>
#include <RAY-Core.h>
#include <Random.h>

int main()
{
    RAYX::randomSeed();
    std::cout << RAYX::randomDouble() << std::endl;
}