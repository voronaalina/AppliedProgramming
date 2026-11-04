#include <iostream>
#include "PolyLib.h"

void printPoly(const double* p, int n) {
    bool first = true;
    for (int i = 0; i < n; ++i) {
        if (p[i] == 0) continue;               
        if (!first) std::cout << " + ";
        first = false;
        std::cout << p[i];
        if (i == 1) std::cout << "*x";
        if (i > 1)  std::cout << "*x^" << i;
    }
    if (first) std::cout << 0;                
    std::cout << "\n";
}

int main() {
    double a[] = { 1, 2, 3 };    // 1 + 2*x + 3*x^2
    double b[] = { 0, 1 };       // x
    double r[10];             
    int len = 0;
    double v = 0;

    std::cout << "a(x) = "; printPoly(a, 3);
    std::cout << "b(x) = "; printPoly(b, 2);
    std::cout << "\n";

    Poly::Add(a, 3, b, 2, r, 10, &len);
    std::cout << "a + b = "; printPoly(r, len);      

    Poly::Multiply(a, 3, b, 2, r, 10, &len);
    std::cout << "a * b = "; printPoly(r, len);      

    Poly::Evaluate(a, 3, 2.0, &v);
    std::cout << "a(2) = " << v << "\n";           

    Poly::Derivative(a, 3, r, 10, &len);
    std::cout << "a'(x) = "; printPoly(r, len);      

    Poly::Integral(a, 3, 5.0, r, 10, &len);
    std::cout << "integral a = "; printPoly(r, len);      

    return 0;
}