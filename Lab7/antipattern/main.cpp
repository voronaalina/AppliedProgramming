#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

// Антипатерн 1: виняток типу int
void throwInt() { throw 42; }

// Антипатерн 2: виняток типу std::string
void throwString() { throw std::string("Invalid polynomial"); }

// Рекомендований підхід
void throwStandard() { throw std::invalid_argument("Invalid polynomial"); }

int main()
{
    std::cout << "=== 1. int: catch (const std::exception&) ===\n";
    try { throwInt(); }
    catch (const std::exception& e) { std::cout << "std::exception: " << e.what() << '\n'; }
    catch (int v) { std::cout << "Caught int by its own handler: " << v << '\n'; }

    std::cout << "\n=== 2. std::string: catch (const std::exception&) ===\n";
    try { throwString(); }
    catch (const std::exception& e) { std::cout << "std::exception: " << e.what() << '\n'; }
    catch (const std::string& s) { std::cout << "Caught string by its own handler: " << s << '\n'; }

    std::cout << "\n=== 3. catch (...) catches everything, but gives no information ===\n";
    try { throwInt(); }
    catch (...) { std::cout << "Caught unknown exception (type and data are not available)\n"; }

    std::cout << "\n=== 4. standard exception through the base class ===\n";
    try { throwStandard(); }
    catch (const std::exception& e) { std::cout << "Caught std::exception: " << e.what() << '\n'; }

    return 0;
}