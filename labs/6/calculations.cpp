#include "calculations.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double epsilon = 1e-12;

void validateInput(double x, double y) {
    if (!std::isfinite(x) || !std::isfinite(y)) {
        throw std::invalid_argument("X and Y must be finite numbers.");
    }
}

void validateResult(double value) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument("The calculation result is not finite for the given X and Y.");
    }
}
}

double calculateR(double x, double y) {
    validateInput(x, y);

    if (std::abs(y) <= epsilon) {
        throw std::invalid_argument("Y must not be zero or too close to zero.");
    }

    const double discriminant = x * x - 4.0 * x * y;
    validateResult(discriminant);

    if (discriminant < -epsilon) {
        throw std::invalid_argument("The expression under the square root must not be negative.");
    }

    const double r = (-x - std::sqrt(std::max(0.0, discriminant))) / (2.0 * y);
    validateResult(r);
    return r;
}

double calculateS(double x, double y) {
    validateInput(x, y);

    const double angle = std::min(x, y);
    if (std::abs(std::cos(angle)) <= epsilon) {
        throw std::invalid_argument("tan(min(X, Y)) is undefined near this point.");
    }

    const double s = std::log(std::pow(2.0, x)) - std::tan(angle);
    validateResult(s);
    return s;
}

double calculateMax(double x, double y) {
    return std::max(calculateR(x, y), calculateS(x, y));
}
