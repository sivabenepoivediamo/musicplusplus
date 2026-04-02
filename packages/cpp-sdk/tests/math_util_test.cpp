#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/math_util.h"

#include <stdexcept>

namespace {

using musicpp::division_result;
using musicpp::euclidean_division;
using musicpp::gcd;
using musicpp::lcm;

TEST_CASE("euclidean_division_zero_divisor_throws", "[math_util][safety]") {
    REQUIRE_THROWS_AS(euclidean_division(1, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(euclidean_division(0, 0), std::invalid_argument);
}

TEST_CASE("euclidean_division_positive_divisor", "[math_util]") {
    division_result r = euclidean_division(17, 5);
    ASSERT_EQ(r.quotient, 3);
    ASSERT_EQ(r.remainder, 2);

    r = euclidean_division(15, 5);
    ASSERT_EQ(r.quotient, 3);
    ASSERT_EQ(r.remainder, 0);
}

TEST_CASE("euclidean_division_negative_dividend", "[math_util]") {
    division_result r = euclidean_division(-17, 5);
    ASSERT_EQ(r.remainder, 3);
    ASSERT_EQ(r.quotient * 5 + r.remainder, -17);
    ASSERT_TRUE(r.remainder >= 0 && r.remainder < 5);

    r = euclidean_division(-1, 3);
    ASSERT_EQ(r.remainder, 2);
    ASSERT_EQ(r.quotient, -1);
}

TEST_CASE("euclidean_division_negative_divisor_identity", "[math_util]") {
    division_result r = euclidean_division(17, -5);
    ASSERT_EQ(r.quotient * (-5) + r.remainder, 17);
    ASSERT_TRUE(r.remainder >= 0);
    ASSERT_TRUE(r.remainder < 5);

    division_result r2 = euclidean_division(-7, -3);
    ASSERT_EQ(r2.quotient * (-3) + r2.remainder, -7);
    ASSERT_TRUE(r2.remainder >= 0);
    ASSERT_TRUE(r2.remainder < 3);
}

TEST_CASE("gcd_cases", "[math_util]") {
    ASSERT_EQ(gcd(48, 18), 6);
    ASSERT_EQ(gcd(18, 48), 6);
    ASSERT_EQ(gcd(7, 13), 1);
    ASSERT_EQ(gcd(0, 5), 5);
    ASSERT_EQ(gcd(5, 0), 5);
    ASSERT_EQ(gcd(-12, 18), 6);
    ASSERT_EQ(gcd(1, 1), 1);
}

TEST_CASE("lcm_cases", "[math_util]") {
    ASSERT_EQ(lcm({}), 1);
    ASSERT_EQ(lcm({7}), 7);
    ASSERT_EQ(lcm({-7}), 7);
    ASSERT_EQ(lcm({4, 6}), 12);
    ASSERT_EQ(lcm({3, 4, 5}), 60);
    ASSERT_EQ(lcm({8, 12, 15}), 120);
    ASSERT_EQ(lcm({0, 5}), 0);
    ASSERT_EQ(lcm({5, 0}), 0);
}

} // namespace
