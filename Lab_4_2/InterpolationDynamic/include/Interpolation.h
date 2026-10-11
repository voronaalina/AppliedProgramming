#pragma once

#if defined(INTERP_STATIC)
#define INTERP_API extern "C"
#elif defined(INTERP_EXPORTS)
#define INTERP_API extern "C" __declspec(dllexport)
#else
#define INTERP_API extern "C" __declspec(dllimport)
#endif

#define INTERP_OK                0
#define INTERP_ERR_NULL         -1
#define INTERP_ERR_SIZE         -2
#define INTERP_ERR_DUPLICATE_X  -3
#define INTERP_ERR_NOT_SORTED   -4
#define INTERP_ERR_OUT_OF_RANGE -5
#define INTERP_ERR_NOT_FINITE   -6
#define INTERP_ERR_BAD_METHOD   -7
#define INTERP_ERR_OVERFLOW     -8

#define INTERP_MAX_N 10000

#define INTERP_METHOD_LINEAR   0
#define INTERP_METHOD_LAGRANGE 1

INTERP_API int linearInterpolation(const double* x, const double* y, int n,
    double xq, double* result);

INTERP_API int lagrange(const double* x, const double* y, int n,
    double xq, double* result);

INTERP_API int interpolate(const double* x, const double* y, int n,
    double xq, int method, double* result);

INTERP_API int interpolationError(const double* x, const double* y, int n,
    double xq, int method, double exactValue,
    double* result);