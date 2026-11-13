#include "Polynomial.h"
#include "PolyExceptions.h"
#include "PolyLib.h"

#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
    // Перетворює коди помилок функцій з ЛР4 на винятки.
    void throwIfError(int code, const char* operation)
    {
        switch (code)
        {
        case POLY_OK:
            return;
        case POLY_ERR_NULL:
            throw std::invalid_argument(std::string(operation) + ": null pointer");
        case POLY_ERR_SIZE:
            throw std::invalid_argument(std::string(operation) + ": invalid size");
        case POLY_ERR_BUFFER:
            throw std::length_error(std::string(operation) + ": output buffer too small");
        default:
            throw std::logic_error(std::string(operation) + ": unknown error code");
        }
    }

    // NaN або Inf у результаті означає переповнення.
    void requireFinite(const std::vector<double>& values, const char* operation)
    {
        for (double v : values)
            if (!std::isfinite(v))
                throw PolynomialOverflowException(operation);
    }

    void requireSizeLimit(std::size_t size, const char* operation)
    {
        if (size > Polynomial::kMaxCoefficients)
            throw std::length_error(std::string(operation) + ": too many coefficients");
    }
}

std::vector<double> Polynomial::validated(std::vector<double> coefficients)
{
    if (coefficients.empty())
        throw std::invalid_argument("Polynomial must have at least one coefficient");
    requireSizeLimit(coefficients.size(), "Polynomial");
    for (std::size_t i = 0; i < coefficients.size(); ++i)
        if (!std::isfinite(coefficients[i]))
            throw InvalidCoefficientException(i, coefficients[i]);
    return coefficients;
}

Polynomial::Polynomial(std::vector<double> coefficients)
    : coeffs_(validated(std::move(coefficients)))
{
}

Polynomial::Polynomial(std::initializer_list<double> coefficients)
    : coeffs_(validated(std::vector<double>(coefficients)))
{
}

std::size_t Polynomial::size() const noexcept { return coeffs_.size(); }

std::size_t Polynomial::degree() const noexcept { return coeffs_.size() - 1; }

const std::vector<double>& Polynomial::coefficients() const noexcept { return coeffs_; }

double Polynomial::coefficient(std::size_t index) const
{
    if (index >= coeffs_.size())
        throw std::out_of_range("Coefficient index out of range");
    return coeffs_[index];
}

// Гарантія: strong
void Polynomial::setCoefficient(std::size_t index, double value)
{
    // Усі перевірки до зміни стану.
    if (index >= coeffs_.size())
        throw std::out_of_range("Coefficient index out of range");
    if (!std::isfinite(value))
        throw InvalidCoefficientException(index, value);

    coeffs_[index] = value;
}

// Гарантія: no-throw (обмін внутрішніх векторів не виділяє пам'ять)
void Polynomial::swap(Polynomial& other) noexcept
{
    coeffs_.swap(other.coeffs_);
}

// Гарантія: strong.
Polynomial& Polynomial::multiplyAssign(const Polynomial& other)
{
    //const Polynomial copy = *this;
    //coeffs_.assign(coeffs_.size(), 0.0);          // ПОМИЛКА
    //Polynomial result = copy.multiply(other);

    Polynomial result = multiply(other);  // при помилці

    swap(result);
    return *this;
}

// Гарантія: strong (делегує multiplyAssign).
Polynomial& Polynomial::operator*=(const Polynomial& other)
{
    return multiplyAssign(other);
}

double Polynomial::evaluate(double x) const
{
    if (!std::isfinite(x))
        throw InvalidParameterException("x", x);

    double result = 0.0;
    throwIfError(Poly::Evaluate(coeffs_.data(), static_cast<int>(coeffs_.size()), x, &result),
        "evaluate");
    if (!std::isfinite(result))
        throw PolynomialOverflowException("evaluate");
    return result;
}

Polynomial Polynomial::derivative() const
{
    const std::size_t n = coeffs_.size();
    std::vector<double> out(n == 1 ? 1 : n - 1);
    int len = 0;
    throwIfError(Poly::Derivative(coeffs_.data(), static_cast<int>(n),
        out.data(), static_cast<int>(out.size()), &len),
        "derivative");
    out.resize(static_cast<std::size_t>(len));
    requireFinite(out, "derivative");
    return Polynomial(std::move(out));
}

Polynomial Polynomial::integral(double constant) const
{
    if (!std::isfinite(constant))
        throw InvalidParameterException("constant", constant);

    const std::size_t n = coeffs_.size();
    requireSizeLimit(n + 1, "integral");

    std::vector<double> out(n + 1);
    int len = 0;
    throwIfError(Poly::Integral(coeffs_.data(), static_cast<int>(n), constant,
        out.data(), static_cast<int>(out.size()), &len),
        "integral");
    out.resize(static_cast<std::size_t>(len));
    requireFinite(out, "integral");
    return Polynomial(std::move(out));
}

Polynomial Polynomial::add(const Polynomial& other) const
{
    const std::size_t na = coeffs_.size();
    const std::size_t nb = other.coeffs_.size();
    std::vector<double> out(na > nb ? na : nb);
    int len = 0;
    throwIfError(Poly::Add(coeffs_.data(), static_cast<int>(na),
        other.coeffs_.data(), static_cast<int>(nb),
        out.data(), static_cast<int>(out.size()), &len),
        "add");
    out.resize(static_cast<std::size_t>(len));
    requireFinite(out, "add");
    return Polynomial(std::move(out));
}

Polynomial Polynomial::multiply(const Polynomial& other) const
{
    const std::size_t na = coeffs_.size();
    const std::size_t nb = other.coeffs_.size();
    requireSizeLimit(na + nb - 1, "multiply");

    std::vector<double> out(na + nb - 1);
    int len = 0;
    throwIfError(Poly::Multiply(coeffs_.data(), static_cast<int>(na),
        other.coeffs_.data(), static_cast<int>(nb),
        out.data(), static_cast<int>(out.size()), &len),
        "multiply");
    out.resize(static_cast<std::size_t>(len));
    requireFinite(out, "multiply");
    return Polynomial(std::move(out));
}