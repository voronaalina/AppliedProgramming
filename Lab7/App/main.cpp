#include <iostream>
#include <limits>
#include "Polynomial.h"
#include "PolyExceptions.h"

int main()
{
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();

    Polynomial a{ 1.0, 2.0, 3.0 };
    Polynomial b{ 0.0, 1.0 };

    // «вичайна робота
    Polynomial m = a.multiply(b);
    std::cout << "a*b coefficients:";
    for (double c : m.coefficients()) std::cout << ' ' << c;
    std::cout << "\na(2) = " << a.evaluate(2.0) << '\n';

    // —ценар≥й: NaN у коеф≥ц≥Їнт≥
    try { Polynomial p{ 1.0, nan }; }
    catch (const InvalidCoefficientException& e)
    {
        std::cout << e.what() << " (index " << e.index() << ")\n";
    }

    // —ценар≥й: порожн≥й наб≥р
    try { Polynomial p(std::vector<double>{}); }
    catch (const std::invalid_argument& e)
    {
        std::cout << "invalid_argument: " << e.what() << '\n';
    }

    // —ценар≥й: переповненн€, стан не зм≥нюЇтьс€
    Polynomial x{ 1e200, 1.0 };
    try { x *= Polynomial{ 1e200, 1.0 }; }
    catch (const PolynomialOverflowException& e)
    {
        std::cout << e.what() << '\n';
    }
    std::cout << "x after failed *=: size " << x.size()
        << ", x[0] = " << x.coefficient(0) << '\n';

    return 0;
}