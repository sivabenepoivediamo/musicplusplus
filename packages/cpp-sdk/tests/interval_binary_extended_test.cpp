#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/vectors.h"

namespace {

using musicpp::binary_vector;
using musicpp::interval_vector;

TEST_CASE("interval_vector_compound_assignment_scalars", "[vectors][interval]") {
    interval_vector iv({1, 2}, 0, 12);
    iv += 10;
    ASSERT_EQ(iv[0], 11);
    ASSERT_EQ(iv[1], 12);
    iv -= 1;
    ASSERT_EQ(iv[0], 10);
    iv *= 2;
    ASSERT_EQ(iv[0], 20);
}

TEST_CASE("interval_vector_element_matches_bracket", "[vectors][interval]") {
    interval_vector iv({10, 20, 30}, 0, 12);
    ASSERT_EQ(iv.element(0), iv[0]);
    ASSERT_EQ(iv.element(4), iv[4]);
    ASSERT_EQ(iv.element(-1), iv[-1]);
}

TEST_CASE("interval_vector_repeat_non_positive", "[vectors][interval]") {
    interval_vector iv({1, 2}, 3, 12);
    ASSERT_EQ(iv.repeat(0).size(), static_cast<size_t>(0));
    ASSERT_EQ(iv.repeat(-1).size(), static_cast<size_t>(0));
    ASSERT_INTERVAL_VECTOR_EQ(iv.repeat(2), musicpp_test::ints({1, 2, 1, 2}), 3, 12);
}

TEST_CASE("interval_vector_adapt_to_lcm", "[vectors][interval]") {
    ASSERT_TRUE(interval_vector::adapt_to_lcm({}).empty());

    interval_vector a({0, 2}, 0, 4);
    interval_vector b({0, 3}, 0, 6);
    auto adapted = interval_vector::adapt_to_lcm({a, b});
    ASSERT_EQ(adapted.size(), static_cast<size_t>(2));
    ASSERT_EQ(adapted[0].mod(), 12);
    ASSERT_EQ(adapted[1].mod(), 12);
    ASSERT_VECTOR_EQ(adapted[0].data(), musicpp_test::ints({0, 6}));
    ASSERT_VECTOR_EQ(adapted[1].data(), musicpp_test::ints({0, 6}));

    auto same = interval_vector::adapt_to_lcm({a, a});
    ASSERT_EQ(same.size(), static_cast<size_t>(2));
    ASSERT_EQ(same[0].mod(), 4);
}

TEST_CASE("interval_vector_negate_flips_offset", "[vectors][interval]") {
    interval_vector iv({1, 2}, 5, 12);
    auto n = iv.negate();
    ASSERT_INTERVAL_VECTOR_EQ(n, musicpp_test::ints({-1, -2}), -5, 12);
}

TEST_CASE("interval_vector_rotate_empty", "[vectors][interval]") {
    interval_vector empty(std::vector<int>{}, 7, 12);
    ASSERT_INTERVAL_VECTOR_EQ(empty.rotate(3), empty.data(), 7, 12);
}

TEST_CASE("binary_vector_repeat_non_positive", "[vectors][binary]") {
    binary_vector b({1, 0}, 0, 4);
    ASSERT_EQ(b.repeat(0).size(), static_cast<size_t>(0));
    ASSERT_EQ(b.repeat(-2).size(), static_cast<size_t>(0));
    ASSERT_BINARY_VECTOR_EQ(b.repeat(2), musicpp_test::ints({1, 0, 1, 0}), 0, 4);
}

TEST_CASE("binary_vector_adapt_to_lcm_spacing", "[vectors][binary]") {
    binary_vector a({1, 0, 0, 0}, 0, 4);
    binary_vector b({1, 0, 0, 0, 0, 0}, 0, 6);
    auto adapted = binary_vector::adapt_to_lcm({a, b});
    ASSERT_EQ(adapted.size(), static_cast<size_t>(2));
    ASSERT_EQ(adapted[0].mod(), 12);
    ASSERT_EQ(adapted[1].mod(), 12);
    ASSERT_VECTOR_EQ(adapted[0].data(), musicpp_test::ints({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
    ASSERT_VECTOR_EQ(adapted[1].data(), musicpp_test::ints({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}

} // namespace
