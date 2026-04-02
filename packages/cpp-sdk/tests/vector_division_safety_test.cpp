#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/vectors.h"

#include <stdexcept>

namespace {

using musicpp::interval_vector;
using musicpp::position_vector;

TEST_CASE("position_vector_scalar_div_mod_zero_throws", "[vectors][safety]") {
    position_vector p({4, 8, 12});
    REQUIRE_THROWS_AS(p / 0, std::invalid_argument);
    REQUIRE_THROWS_AS(p % 0, std::invalid_argument);
}

TEST_CASE("interval_vector_scalar_div_mod_zero_throws", "[vectors][safety]") {
    interval_vector iv({2, 3, 5});
    REQUIRE_THROWS_AS(iv / 0, std::invalid_argument);
    REQUIRE_THROWS_AS(iv % 0, std::invalid_argument);
}

TEST_CASE("position_vector_componentwise_div_mod_zero_throws", "[vectors][safety]") {
    position_vector a({10, 20});
    position_vector b({2, 0});
    REQUIRE_THROWS_AS(a / b, std::invalid_argument);
    REQUIRE_THROWS_AS(a % b, std::invalid_argument);
    std::vector<int> z = {1, 0};
    REQUIRE_THROWS_AS(a / z, std::invalid_argument);
}

TEST_CASE("interval_vector_componentwise_div_mod_zero_throws", "[vectors][safety]") {
    interval_vector a({4, 6});
    interval_vector b({2, 0});
    REQUIRE_THROWS_AS(a / b, std::invalid_argument);
    REQUIRE_THROWS_AS(a % b, std::invalid_argument);
}

} // namespace
