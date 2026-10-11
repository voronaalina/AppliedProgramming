#include <iostream>
#include <limits>
#include <vector>
#include "Interpolation.h"

using namespace std;

// Друкує результат одного тесту: код, який повернула функція, і код, який очікується за README
void check(const char* name, int got, int expected) {
    cout << (got == expected ? "[OK]   " : "[DIFF] ")
        << name << ": got " << got << ", expected " << expected << "\n";
}

int main() {
    double x[] = { 0, 1, 2, 3 };
    double y[] = { 0, 1, 4, 9 };        // y = x^2
    double r = 0;
 
    cout << "=== Normal use ===\n";

    int rc = linearInterpolation(x, y, 4, 1.5, &r);
    cout << "linear(1.5)   = " << r << " (expected 2.5), code " << rc << "\n";

    rc = lagrange(x, y, 4, 1.5, &r);
    cout << "lagrange(1.5) = " << r << " (expected 2.25), code " << rc << "\n";

    rc = interpolate(x, y, 4, 1.5, INTERP_METHOD_LINEAR, &r);
    cout << "interpolate, linear   = " << r << " (expected 2.5), code " << rc << "\n";

    rc = interpolate(x, y, 4, 1.5, INTERP_METHOD_LAGRANGE, &r);
    cout << "interpolate, lagrange = " << r << " (expected 2.25), code " << rc << "\n";

    rc = interpolationError(x, y, 4, 1.5, INTERP_METHOD_LINEAR, 2.25, &r);
    cout << "error, linear   = " << r << " (expected 0.25), code " << rc << "\n";

    // Граничні випадки 
    cout << "\n=== Edge cases ===\n";

    // nullptr
    check("x is nullptr", linearInterpolation(nullptr, y, 4, 1.5, &r), INTERP_ERR_NULL);
    check("result is nullptr", lagrange(x, y, 4, 1.5, nullptr), INTERP_ERR_NULL);

    // Розмір n
    check("n = 1", lagrange(x, y, 1, 1.5, &r), INTERP_ERR_SIZE);

    vector<double> big(10001);               // справжній масив, тому безпечно
    for (int i = 0; i < 10001; ++i) big[i] = i;
    check("n = 10001 (too big)", lagrange(big.data(), big.data(), 10001, 1.5, &r),
        INTERP_ERR_SIZE);
    check("n = 10000 (max)", lagrange(big.data(), big.data(), 10000, 1.5, &r),
        INTERP_OK);

    // Однакові x та порядок x
    double dupX[] = { 0, 1, 1, 2 };
    double dupY[] = { 0, 1, 2, 4 };
    check("lagrange, duplicate x", lagrange(dupX, dupY, 4, 0.5, &r), INTERP_ERR_DUPLICATE_X);

    double unsX[] = { 0, 2, 1, 3 };
    double unsY[] = { 0, 4, 1, 9 };
    check("linear, x not sorted", linearInterpolation(unsX, unsY, 4, 1.5, &r),
        INTERP_ERR_NOT_SORTED);

    // Точка xq поза діапазоном
    check("linear, xq = 3.5 (out of range)", linearInterpolation(x, y, 4, 3.5, &r),
        INTERP_ERR_OUT_OF_RANGE);
    check("lagrange, xq = 5 (extrapolation)", lagrange(x, y, 4, 5.0, &r), INTERP_OK);

    // NaN та inf
    constexpr double nanValue = numeric_limits<double>::quiet_NaN();
    constexpr double infValue = numeric_limits<double>::infinity();

    double nanY[] = { 0, nanValue, 4, 9 };
    check("NaN in y", lagrange(x, nanY, 4, 1.5, &r), INTERP_ERR_NOT_FINITE);
    check("xq = inf", lagrange(x, y, 4, infValue, &r), INTERP_ERR_NOT_FINITE);

    // Неправильний method
    check("method = 5", interpolate(x, y, 4, 1.5, 5, &r), INTERP_ERR_BAD_METHOD);

    // Переповнення результату
    double ox[] = { 0, 1, 2 };
    double oy[] = { 0, 1, 4 };
    check("overflow, xq = 1e200", lagrange(ox, oy, 3, 1e200, &r), INTERP_ERR_OVERFLOW);

    return 0;
}