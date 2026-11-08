#include <iostream>
#include <limits>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>

#include "Polynomial.h"
#include "PolyExceptions.h"
#include "ScopeLogger.h"

namespace
{
    constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
    constexpr double kInf = std::numeric_limits<double>::infinity();

    void print(const std::string& name, const Polynomial& p)
    {
        std::cout << name << " = ";
        bool first = true;
        for (std::size_t i = 0; i < p.size(); ++i)
        {
            const double c = p.coefficient(i);
            if (c == 0) continue;
            if (!first) std::cout << " + ";
            first = false;
            std::cout << c;
            if (i == 1) std::cout << "*x";
            if (i > 1) std::cout << "*x^" << i;
        }
        if (first) std::cout << 0;
        std::cout << '\n';
    }

    // Обробники: від найбільш спеціалізованих до загальних                                              
    template <class F>
    void scenario(const std::string& title, F action)
    {
        std::cout << "--- " << title << '\n';
        try
        {
            action();
        }
        catch (const InvalidCoefficientException& e)
        {
            std::cout << "InvalidCoefficientException: " << e.what()
                << " (index=" << e.index() << ", value=" << e.value() << ")\n";
        }
        catch (const InvalidParameterException& e)
        {
            std::cout << "InvalidParameterException: " << e.what()
                << " (parameter=" << e.parameter() << ", value=" << e.value() << ")\n";
        }
        catch (const PolynomialOverflowException& e)
        {
            std::cout << "PolynomialOverflowException: " << e.what()
                << " (operation=" << e.operation() << ")\n";
        }
        catch (const PolynomialException& e)
        {
            std::cout << "PolynomialException: " << e.what() << '\n';
        }
        catch (const std::out_of_range& e)
        {
            std::cout << "std::out_of_range: " << e.what() << '\n';
        }
        catch (const std::invalid_argument& e)
        {
            std::cout << "std::invalid_argument: " << e.what() << '\n';
        }
        catch (const std::length_error& e)
        {
            std::cout << "std::length_error: " << e.what() << '\n';
        }
        catch (const std::exception& e)
        {
            std::cout << "std::exception: " << e.what() << '\n';
        }
    }

    // ланцюжок з трьох функцій + RAII 
    void level3()
    {
        ScopeLogger logger("level3");
        std::cout << "Creating polynomial with NaN coefficient...\n";
        Polynomial p{ 1.0, kNaN };
    }

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
                << " (index=" << e.index() << ")\n";
            throw;  
        }
    }

    void level1()
    {
        ScopeLogger logger("level1");
        level2();
    }
}

int main()
{
    std::cout << "=== Part 1: error scenarios ===\n";

    scenario("1. Empty coefficient list", [] {
        Polynomial p(std::vector<double>{});
        });
    scenario("2. NaN / Inf coefficient", [] {
        Polynomial p{ 1.0, 2.0, kInf };
        });
    scenario("3. Coefficient index out of range", [] {
        Polynomial p{ 1.0, 2.0 };
        p.coefficient(5);
        });
    scenario("4. Overflow in multiplication", [] {
        Polynomial a{ 1e200, 1.0 };
        Polynomial b{ 1e200, 1.0 };
        a.multiply(b);
        });
    scenario("5. Invalid operation parameter (x = NaN)", [] {
        Polynomial p{ 1.0, 2.0, 3.0 };
        p.evaluate(kNaN);
        });
    scenario("6. Too many coefficients", [] {
        Polynomial a(std::vector<double>(600, 1.0));
        Polynomial b(std::vector<double>(600, 1.0));
        a.multiply(b);
        });

    std::cout << "\n=== Part 2: strong guarantee for multiplyAssign ===\n";
    {
        Polynomial a{ 1e200, 1.0 };
        const Polynomial b{ 1e200, 1.0 };
        print("a before", a);
        scenario("a *= b (overflow)", [&] { a *= b; });
        print("a after ", a);

        Polynomial c{ 1.0, 2.0, 3.0 };
        const Polynomial x{ 0.0, 1.0 };
        c *= x;
        print("c * x   ", c);
    }

    std::cout << "\n=== Part 3: stack unwinding ===\n";
    try
    {
        level1();
    }
    catch (const PolynomialException& e)
    {
        std::cout << "[MAIN] PolynomialException: " << e.what() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cout << "[MAIN] std::exception: " << e.what() << '\n';
    }
    std::cout << "Program continues normally.\n";
    return 0;
}