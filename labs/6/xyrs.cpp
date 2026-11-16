#include "calculations.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

int main() {
    double x, y;

    try {
        std::cout << "Enter X and Y: ";
        if (!(std::cin >> x >> y)) {
            throw std::invalid_argument("X and Y must be numeric finite values.");
        }

        const double r = calculateR(x, y);
        const double s = calculateS(x, y);
        const double c = calculateMax(x, y);

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "R = " << r << '\n';
        std::cout << "S = " << s << '\n';
        std::cout << "max(R, S) = " << c << '\n';
    } catch (const std::invalid_argument& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}
