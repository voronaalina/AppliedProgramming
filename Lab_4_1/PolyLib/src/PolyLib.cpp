#include "PolyLib.h"
#include <algorithm>

namespace Poly {

    // Перевірка вхідного полінома
    static int check(const double* p, int n) {
        if (!p) return POLY_ERR_NULL;
        if (n <= 0) return POLY_ERR_SIZE;
        return POLY_OK;
    }

    // Перевірка вихідного буфера
    static int checkOut(const double* out, const int* outLen, int cap, int need) {
        if (!out || !outLen) return POLY_ERR_NULL;
        if (cap < need) return POLY_ERR_BUFFER;
        return POLY_OK;
    }

    int Add(const double* a, int na, const double* b, int nb,
        double* out, int outCap, int* outLen) {
        int r = check(a, na); if (r) return r;
        r = check(b, nb);     if (r) return r;
        int n = std::max(na, nb);
        r = checkOut(out, outLen, outCap, n); if (r) return r;

        for (int i = 0; i < n; ++i)
            out[i] = (i < na ? a[i] : 0.0) + (i < nb ? b[i] : 0.0);
        *outLen = n;
        return POLY_OK;
    }

    int Multiply(const double* a, int na, const double* b, int nb,
        double* out, int outCap, int* outLen) {
        int r = check(a, na); if (r) return r;
        r = check(b, nb);     if (r) return r;
        int n = na + nb - 1;
        r = checkOut(out, outLen, outCap, n); if (r) return r;

        std::fill(out, out + n, 0.0);
        for (int i = 0; i < na; ++i)
            for (int j = 0; j < nb; ++j)
                out[i + j] += a[i] * b[j];
        *outLen = n;
        return POLY_OK;
    }

    int Evaluate(const double* a, int n, double x, double* result) {
        int r = check(a, n); if (r) return r;
        if (!result) return POLY_ERR_NULL;

        double s = 0.0;
        for (int i = n - 1; i >= 0; --i)   // схема Горнера
            s = s * x + a[i];
        *result = s;
        return POLY_OK;
    }

    int Derivative(const double* a, int n,
        double* out, int outCap, int* outLen) {
        int r = check(a, n); if (r) return r;
        int m = (n == 1) ? 1 : n - 1;
        r = checkOut(out, outLen, outCap, m); if (r) return r;

        if (n == 1) {
            out[0] = 0.0;                  
        }
        else {
            for (int i = 1; i < n; ++i)
                out[i - 1] = a[i] * i;
        }
        *outLen = m;
        return POLY_OK;
    }

    int Integral(const double* a, int n, double c,
        double* out, int outCap, int* outLen) {
        int r = check(a, n); if (r) return r;
        r = checkOut(out, outLen, outCap, n + 1); if (r) return r;

        out[0] = c;                        
        for (int i = 0; i < n; ++i)
            out[i + 1] = a[i] / (i + 1);
        *outLen = n + 1;
        return POLY_OK;
    }

}