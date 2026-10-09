#pragma once

#define POLY_OK            0
#define POLY_ERR_NULL     -1
#define POLY_ERR_SIZE     -2
#define POLY_ERR_BUFFER   -3

namespace Poly {
    int Add(const double* a, int na, const double* b, int nb,
        double* out, int outCap, int* outLen);
    int Multiply(const double* a, int na, const double* b, int nb,
        double* out, int outCap, int* outLen);
    int Evaluate(const double* a, int n, double x, double* result);
    int Derivative(const double* a, int n,
        double* out, int outCap, int* outLen);
    int Integral(const double* a, int n, double c,
        double* out, int outCap, int* outLen);
}