#include "pch.h"
#include "CppUnitTest.h"
#include "../calculations.h"

#include <cmath>
#include <limits>
#include <stdexcept>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

constexpr double tolerance = 1e-6;

namespace xyrs_tests {
TEST_CLASS(CalculationsTests) {
public:
    TEST_METHOD(CalculateR_RIsMaximum_ReturnsExpectedValue) {
        Assert::AreEqual(4.8284271247461903, calculateR(4.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateR_ZeroDiscriminant_ReturnsMinusTwo) {
        Assert::AreEqual(-2.0, calculateR(4.0, 1.0), tolerance);
    }

    TEST_METHOD(CalculateR_SecondControlCase_ReturnsExpectedValue) {
        Assert::AreEqual(2.7320508075688772, calculateR(2.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateS_NegativeMinimum_ReturnsExpectedValue) {
        Assert::AreEqual(4.3299964468946833, calculateS(4.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateS_PositiveMinimum_ReturnsExpectedValue) {
        Assert::AreEqual(1.2151810025858787, calculateS(4.0, 1.0), tolerance);
    }

    TEST_METHOD(CalculateS_SecondControlCase_ReturnsExpectedValue) {
        Assert::AreEqual(2.9437021494336825, calculateS(2.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateMax_RGreaterThanS_ReturnsR) {
        Assert::AreEqual(calculateR(4.0, -1.0), calculateMax(4.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateMax_SGreaterThanR_ReturnsS) {
        Assert::AreEqual(calculateS(4.0, 1.0), calculateMax(4.0, 1.0), tolerance);
    }

    TEST_METHOD(CalculateMax_SecondControlCase_ReturnsS) {
        Assert::AreEqual(2.9437021494336825, calculateMax(2.0, -1.0), tolerance);
    }

    TEST_METHOD(CalculateR_ZeroY_ThrowsInvalidArgument) {
        Assert::ExpectException<std::invalid_argument>([] {
            calculateR(1.0, 0.0);
        });
    }

    TEST_METHOD(CalculateR_NegativeDiscriminant_ThrowsInvalidArgument) {
        Assert::ExpectException<std::invalid_argument>([] {
            calculateR(1.0, 1.0);
        });
    }

    TEST_METHOD(CalculateS_TangentPole_ThrowsInvalidArgument) {
        const double pole = -std::acos(-1.0) / 2.0;
        Assert::ExpectException<std::invalid_argument>([pole] {
            calculateS(pole, 1.0);
        });
    }

    TEST_METHOD(CalculateS_NearTangentPole_ThrowsInvalidArgument) {
        const double nearPole = -std::acos(-1.0) / 2.0 + 1e-13;
        Assert::ExpectException<std::invalid_argument>([nearPole] {
            calculateS(nearPole, 1.0);
        });
    }

    TEST_METHOD(CalculateR_NaNX_ThrowsInvalidArgument) {
        const double nan = std::numeric_limits<double>::quiet_NaN();
        Assert::ExpectException<std::invalid_argument>([nan] {
            calculateR(nan, 1.0);
        });
    }

    TEST_METHOD(CalculateS_NaNY_ThrowsInvalidArgument) {
        const double nan = std::numeric_limits<double>::quiet_NaN();
        Assert::ExpectException<std::invalid_argument>([nan] {
            calculateS(1.0, nan);
        });
    }

    TEST_METHOD(CalculateR_InfiniteX_ThrowsInvalidArgument) {
        const double infinity = std::numeric_limits<double>::infinity();
        Assert::ExpectException<std::invalid_argument>([infinity] {
            calculateR(infinity, 1.0);
        });
    }

    TEST_METHOD(CalculateS_InfiniteY_ThrowsInvalidArgument) {
        const double infinity = std::numeric_limits<double>::infinity();
        Assert::ExpectException<std::invalid_argument>([infinity] {
            calculateS(1.0, infinity);
        });
    }
};
}
