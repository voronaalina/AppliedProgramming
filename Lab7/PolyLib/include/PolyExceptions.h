#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

// Базовий клас усіх користувацьких винятків бібліотеки.
class PolynomialException : public std::runtime_error
{
public:
    explicit PolynomialException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

// Коефіцієнт є NaN або Inf.
class InvalidCoefficientException : public PolynomialException
{
private:
    std::size_t index_;
    double value_;

public:
    InvalidCoefficientException(std::size_t index, double value)
        : PolynomialException("Coefficient is not finite at index " + std::to_string(index)),
        index_(index),
        value_(value)
    {
    }

    std::size_t index() const noexcept { return index_; }
    double value() const noexcept { return value_; }
};

// Результат операції не вміщується в скінченне число double.
class PolynomialOverflowException : public PolynomialException
{
private:
    std::string operation_;

public:
    explicit PolynomialOverflowException(const std::string& operation)
        : PolynomialException("Overflow in operation: " + operation),
        operation_(operation)
    {
    }

    const std::string& operation() const noexcept { return operation_; }
};

// Недопустиме значення параметра операції .
class InvalidParameterException : public PolynomialException
{
private:
    std::string parameter_;
    double value_;

public:
    InvalidParameterException(const std::string& parameter, double value)
        : PolynomialException("Invalid parameter: " + parameter),
        parameter_(parameter),
        value_(value)
    {
    }

    const std::string& parameter() const noexcept { return parameter_; }
    double value() const noexcept { return value_; }
};