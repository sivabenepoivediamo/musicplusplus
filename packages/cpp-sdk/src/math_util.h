#ifndef MUSICPP_MATH_UTIL_H
#define MUSICPP_MATH_UTIL_H

#include "utility.h"

#include <stdexcept>

namespace musicpp {

struct division_result {
    int quotient;
    int remainder;
};

inline division_result euclidean_division(int dividend, int divisor) {
    if (divisor == 0) {
        throw std::invalid_argument("euclidean_division: divisor is zero");
    }
    int quotient = dividend / divisor;
    int remainder = dividend - quotient * divisor;
    if (remainder < 0) {
        return {quotient - 1, remainder + divisor};
    }
    return {quotient, remainder};
}

inline int gcd(int a, int b) {
    if (b == 0) {
        return std::abs(a);
    }
    return gcd(b, a % b);
}

inline int lcm(const std::vector<int>& values) {
    if (values.empty()) {
        return 1;
    }
    if (values.size() == 1) {
        return std::abs(values[0]);
    }
    int result = std::abs(values[0]);
    for (size_t i = 1; i < values.size(); ++i) {
        int g = gcd(result, values[i]);
        result = (result * std::abs(values[i])) / g;
    }
    return result;
}

} // namespace musicpp

#endif
