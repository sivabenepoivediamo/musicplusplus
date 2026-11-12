#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/vectors.h"

namespace {

using musicpp::position_vector;

TEST_CASE("position_vector_compound_assignment_scalars", "[vectors][position]") {
    position_vector p({1, 2, 3}, 12, 12);
    p += 10;
    ASSERT_EQ(p[0], 11);
    ASSERT_EQ(p[1], 12);
    p -= 1;
    ASSERT_EQ(p[0], 10);
    p *= 2;
    ASSERT_EQ(p[0], 20);
}

TEST_CASE("position_vector_element_matches_bracket", "[vectors][position]") {
    position_vector p({10, 20, 30}, 12, 12);
    ASSERT_EQ(p.element(0), p[0]);
    ASSERT_EQ(p.element(3), p[3]);
    ASSERT_EQ(p.element(-1), p[-1]);
}

TEST_CASE("position_vector_scalar_aliases", "[vectors][position]") {
    position_vector p({2, 4}, 12, 12);
    ASSERT_POSITION_VECTOR_EQ(p.scalarSum(3), musicpp_test::ints({5, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(p.scalarProduct(2), musicpp_test::ints({4, 8}), 12);
}

TEST_CASE("position_vector_repeat_non_positive", "[vectors][position]") {
    position_vector p({1, 2}, 12, 12);
    ASSERT_EQ(p.repeat(0).size(), static_cast<size_t>(0));
    ASSERT_EQ(p.repeat(-1).size(), static_cast<size_t>(0));
    ASSERT_POSITION_VECTOR_EQ(p.repeat(2), musicpp_test::ints({1, 2, 1, 2}), 12);
}

TEST_CASE("position_vector_resize_slice", "[vectors][position]") {
    position_vector p({10, 20, 30, 40}, 12, 12);
    ASSERT_POSITION_VECTOR_EQ(p.resize(0, 2), musicpp_test::ints({10, 20, 30}), 12);
    ASSERT_POSITION_VECTOR_EQ(p.resize(3, 1), musicpp_test::ints({40, 30, 20}), 12);
}

TEST_CASE("position_vector_adapt_to_lcm", "[vectors][position]") {
    ASSERT_TRUE(position_vector::adapt_to_lcm({}).empty());

    position_vector a({0, 2}, 4, 4, true, false);
    position_vector b({0, 3}, 6, 6, true, false);
    auto adapted = position_vector::adapt_to_lcm({a, b});
    ASSERT_EQ(adapted.size(), static_cast<size_t>(2));
    ASSERT_EQ(adapted[0].mod(), 12);
    ASSERT_EQ(adapted[1].mod(), 12);
    ASSERT_VECTOR_EQ(adapted[0].data(), musicpp_test::ints({0, 6}));
    ASSERT_VECTOR_EQ(adapted[1].data(), musicpp_test::ints({0, 6}));

    auto same = position_vector::adapt_to_lcm({a, a});
    ASSERT_EQ(same.size(), static_cast<size_t>(2));
    ASSERT_EQ(same[0].mod(), 4);
}

TEST_CASE("position_vector_set_mod_updates_range", "[vectors][position]") {
    position_vector p({0, 6}, 12, 12);
    p.set_mod(24);
    ASSERT_EQ(p.mod(), 24);
}

TEST_CASE("position_vector_rotate_empty_is_unchanged", "[vectors][position]") {
    position_vector empty(std::vector<int>{}, 12, 12, true, false);
    ASSERT_POSITION_VECTOR_EQ(empty.rotate(5), empty.data(), 12);
}

} // namespace
