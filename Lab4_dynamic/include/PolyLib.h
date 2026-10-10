#pragma once
#ifdef POLYLIB_EXPORTS
#define POLY_API extern "C" __declspec(dllexport)
#else
#define POLY_API extern "C" __declspec(dllimport)
#endif

#define POLY_OK            0
#define POLY_ERR_NULL     -1
#define POLY_ERR_SIZE     -2
#define POLY_ERR_BUFFER   -3

POLY_API int poly_add(const double* a, int na, const double* b, int nb, double* out, int outCap, int* outLen);
POLY_API int poly_multiply(const double* a, int na, const double* b, int nb, double* out, int outCap, int* outLen);
POLY_API int poly_evaluate(const double* a, int n, double x, double* result);
POLY_API int poly_derivative(const double* a, int n, double* out, int outCap, int* outLen);
POLY_API int poly_integral(const double* a, int n, double c, double* out, int outCap, int* outLen);