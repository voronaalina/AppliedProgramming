#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "PolyExceptions.h"
#include "Polynomial.h"

namespace
{
    constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
    constexpr double kInf = std::numeric_limits<double>::infinity();

    // Допоміжна перевірка: коефіцієнти полінома дорівнюють очікуваним.
    void ExpectCoefficients(const Polynomial& p, const std::vector<double>& expected)
    {
        ASSERT_EQ(p.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i)
            EXPECT_DOUBLE_EQ(p.coefficient(i), expected[i]) << "index " << i;
    }
}

// 1. Основні операції 

TEST(PolynomialBasic, MultipliesPolynomials)
{
    Polynomial a{ 1.0, 2.0, 3.0 };
    Polynomial b{ 0.0, 1.0 };
    ExpectCoefficients(a.multiply(b), { 0.0, 1.0, 2.0, 3.0 });
}

TEST(PolynomialBasic, EvaluatesByHorner)
{
    Polynomial p{ 1.0, 2.0, 3.0 };            // 1 + 2x + 3x^2
    EXPECT_DOUBLE_EQ(p.evaluate(2.0), 17.0);
}

TEST(PolynomialBasic, DerivativeOfPolynomial)
{
    Polynomial p{ 1.0, 2.0, 3.0 };
    ExpectCoefficients(p.derivative(), { 2.0, 6.0 });
}

TEST(PolynomialBasic, IntegralWithConstant)
{
    Polynomial p{ 1.0, 2.0, 3.0 };
    ExpectCoefficients(p.integral(5.0), { 5.0, 1.0, 1.0, 1.0 });
}

TEST(PolynomialBasic, MultiplyAssignByItself)
{
    Polynomial a{ 1.0, 1.0 };                 // (1 + x)^2
    a *= a;
    ExpectCoefficients(a, { 1.0, 2.0, 1.0 });
}

// 2. Числова точність 

TEST(PolynomialPrecision, EvaluateWithTolerance)
{
    Polynomial p{ 0.1, 0.2 };                 // 0.1 + 0.2 * 0.3
    EXPECT_NEAR(p.evaluate(0.3), 0.16, 1e-12);
}

TEST(PolynomialPrecision, IntegralDividesByDegree)
{
    Polynomial p{ 1.0, 1.0, 1.0 };
    Polynomial q = p.integral();
    EXPECT_NEAR(q.coefficient(3), 1.0 / 3.0, 1e-12);
}

//  3. Сценарії помилок 

// Сценарій 1: порожній набір коефіцієнтів
TEST(PolynomialErrors, RejectsEmptyCoefficients)
{
    EXPECT_THROW(Polynomial(std::vector<double>{}), std::invalid_argument);
}

// Сценарій 2: NaN або Inf у коефіцієнті
TEST(PolynomialErrors, RejectsNonFiniteCoefficient)
{
    EXPECT_THROW((Polynomial{ 1.0, kNaN }), InvalidCoefficientException);
    EXPECT_THROW((Polynomial{ kInf, 1.0 }), InvalidCoefficientException);

    // Діагностичні дані винятку
    try
    {
        Polynomial p{ 1.0, 2.0, kInf };
        FAIL() << "Exception expected";
    }
    catch (const InvalidCoefficientException& e)
    {
        EXPECT_EQ(e.index(), 2u);
    }
}

// Сценарій 3: недопустимий індекс коефіцієнта
TEST(PolynomialErrors, CoefficientIndexOutOfRange)
{
    Polynomial p{ 1.0, 2.0 };
    EXPECT_THROW(p.coefficient(2), std::out_of_range);
    EXPECT_THROW(p.setCoefficient(2, 1.0), std::out_of_range);
}

// Сценарій 4: переповнення при множенні
TEST(PolynomialErrors, MultiplyOverflow)
{
    Polynomial a{ 1e200, 1.0 };
    Polynomial b{ 1e200, 1.0 };
    EXPECT_THROW(a.multiply(b), PolynomialOverflowException);
}

// Сценарій 5: недопустимий параметр операції
TEST(PolynomialErrors, EvaluateRejectsNonFiniteX)
{
    Polynomial p{ 1.0, 2.0 };
    EXPECT_THROW(p.evaluate(kNaN), InvalidParameterException);
    EXPECT_THROW(p.evaluate(kInf), InvalidParameterException);
}

// Сценарій 6: забагато коефіцієнтів
TEST(PolynomialErrors, TooManyCoefficients)
{
    EXPECT_THROW(Polynomial(std::vector<double>(Polynomial::kMaxCoefficients + 1, 1.0)),
        std::length_error);
}

// 4. Ієрархія винятків 

TEST(PolynomialHierarchy, CustomExceptionsDeriveFromBase)
{
    EXPECT_THROW((Polynomial{ kNaN }), PolynomialException);
    EXPECT_THROW((Polynomial{ kNaN }), std::runtime_error);
    EXPECT_THROW(Polynomial{ 1.0 }.evaluate(kNaN), PolynomialException);
}

// 5. Граничні значення

TEST(PolynomialBoundary, MaximumAllowedSizeIsAccepted)
{
    EXPECT_NO_THROW(Polynomial(std::vector<double>(Polynomial::kMaxCoefficients, 1.0)));
}

TEST(PolynomialBoundary, LargestSafeProductDoesNotOverflow)
{
    Polynomial a{ 1e154 };
    Polynomial b{ 1e154 };
    EXPECT_NO_THROW(a.multiply(b));         
}

//  6. Інваріанти 

TEST(PolynomialInvariants, ResultsOfOperationsAreFiniteAndNonEmpty)
{
    Polynomial p{ 1.0, -2.0, 0.5 };
    for (const Polynomial& r : { p.derivative(), p.integral(3.0), p.add(p), p.multiply(p) })
    {
        EXPECT_GE(r.size(), 1u);
        for (double c : r.coefficients())
            EXPECT_TRUE(std::isfinite(c));
    }
}

TEST(PolynomialInvariants, SwapIsNoThrow)
{
    static_assert(noexcept(std::declval<Polynomial&>().swap(std::declval<Polynomial&>())));
    static_assert(noexcept(std::declval<const Polynomial&>().size()));
    SUCCEED();
}

//  7. Сильна гарантія 

TEST(PolynomialStrongGuarantee, SetCoefficientPreservesStateOnBadIndex)
{
    Polynomial p{ 1.0, 2.0, 3.0 };
    EXPECT_THROW(p.setCoefficient(10, 5.0), std::out_of_range);
    ExpectCoefficients(p, { 1.0, 2.0, 3.0 });
}

TEST(PolynomialStrongGuarantee, MultiplyAssignPreservesStateOnOverflow)
{
    Polynomial a{ 1e200, 1.0 };
    const Polynomial b{ 1e200, 1.0 };
    EXPECT_THROW(a *= b, PolynomialOverflowException);

    ASSERT_EQ(a.size(), 2u);
    EXPECT_DOUBLE_EQ(a.coefficient(0), 1e200);
    EXPECT_DOUBLE_EQ(a.coefficient(1), 1.0);
}

TEST(PolynomialStrongGuarantee, MultiplyAssignPreservesStateOnSizeLimit)
{
    Polynomial a(std::vector<double>(600, 2.0));
    const Polynomial b(std::vector<double>(600, 3.0));
    EXPECT_THROW(a.multiplyAssign(b), std::length_error);

    EXPECT_EQ(a.coefficients(), std::vector<double>(600, 2.0));
}