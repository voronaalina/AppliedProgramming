#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>


// Інваріанти:
//   1. коефіцієнтів хоча б один;
//   2. коефіцієнтів не більше kMaxCoefficients;
//   3. кожен коефіцієнт скінченний (не NaN і не Inf).
class Polynomial
{
private:
    std::vector<double> coeffs_;

    static std::vector<double> validated(std::vector<double> coefficients);

public:
    static constexpr std::size_t kMaxCoefficients = 1000;

    explicit Polynomial(std::vector<double> coefficients);
    Polynomial(std::initializer_list<double> coefficients);

    std::size_t size() const noexcept;
    std::size_t degree() const noexcept;
    const std::vector<double>& coefficients() const noexcept;
    double coefficient(std::size_t index) const;        

    // Гарантія: strong (усі перевірки до присвоєння).
    void setCoefficient(std::size_t index, double value);

    // Гарантія: strong (результат готується окремо, потім swap).
    Polynomial& multiplyAssign(const Polynomial& other);
    Polynomial& operator*=(const Polynomial& other);

    // Гарантія: no-throw.
    void swap(Polynomial& other) noexcept;
 
    double evaluate(double x) const;
    Polynomial derivative() const;
    Polynomial integral(double constant = 0.0) const;
    Polynomial add(const Polynomial& other) const;
    Polynomial multiply(const Polynomial& other) const;
};