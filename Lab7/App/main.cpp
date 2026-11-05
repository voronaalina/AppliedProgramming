#include <iostream>
#include <limits>
#include "Polynomial.h"
#include "PolyExceptions.h"
#include "ScopeLogger.h"

// Рівень 3: тут виникає виняток
void level3()
{
    ScopeLogger logger("level3");
    std::cout << "Creating polynomial with NaN coefficient...\n";
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    Polynomial p{ 1.0, nan };
    std::cout << "This line is never reached\n";
}

// Рівень 2: перехоплює, виводить діагностику і кидає далі
void level2()
{
    ScopeLogger logger("level2");
    try
    {
        level3();
    }
    catch (const InvalidCoefficientException& e)
    {
        std::cout << "[LOG] level2 caught: " << e.what()
            << " (index " << e.index() << ")\n";
        throw;
    }
}

// Рівень 1: просто викликає наступний
void level1()
{
    ScopeLogger logger("level1");
    level2();
    std::cout << "This line is never reached\n";
}

int main()
{
    try
    {
        level1();
    }
    catch (const PolynomialException& e)
    {
        std::cout << "[MAIN] caught: " << e.what() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cout << "[MAIN] std::exception: " << e.what() << '\n';
    }
    std::cout << "Program continues normally.\n";
    return 0;
}