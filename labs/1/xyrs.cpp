#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

int main() {
    double x, y;

    std::cout << "Enter X and Y: ";
    std::cin >> x >> y;

    const double r = (-x - std::sqrt(x * x - 4.0 * x * y)) / (2.0 * y);
    const double s = std::log(std::pow(2.0, x)) - std::tan(std::min(x, y));
    const double c = std::max(r, s);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "R = " << r << '\n';
    std::cout << "S = " << s << '\n';
    std::cout << "max(R, S) = " << c << '\n';

    std::cout << "\nPress Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}
