#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/distances.h"

#include <cmath>
#include <stdexcept>

namespace {

using namespace musicpp;

TEST_CASE("normalize_throws_on_zero_sum", "[distances]") {
    std::vector<int> z = {0, 0, 0};
    REQUIRE_THROWS_AS(normalize(z), std::invalid_argument);
}

TEST_CASE("normalize_probability_sum_one", "[distances]") {
    const std::vector<int> v = {2, 2, 4};
    auto out = normalize(v);
    ASSERT_EQ(out.size(), static_cast<size_t>(3));
    ASSERT_NEAR(out[0], 0.25, 1e-12);
    ASSERT_NEAR(out[1], 0.25, 1e-12);
    ASSERT_NEAR(out[2], 0.5, 1e-12);
    double sum = 0.0;
    for (double x : out) {
        sum += x;
    }
    ASSERT_NEAR(sum, 1.0, 1e-12);
}

TEST_CASE("compute_cdf_monotone", "[distances]") {
    std::vector<double> pdf = {0.25, 0.25, 0.5};
    auto cdf = computeCDF(pdf);
    ASSERT_EQ(cdf.size(), pdf.size());
    ASSERT_NEAR(cdf[0], 0.25, 1e-9);
    ASSERT_NEAR(cdf[1], 0.5, 1e-9);
    ASSERT_NEAR(cdf[2], 1.0, 1e-9);
}

TEST_CASE("euclidean_distance_int_vectors", "[distances]") {
    ASSERT_NEAR(euclideanDistance({0, 0}, {3, 4}), 5.0, 1e-9);
    ASSERT_NEAR(euclideanDistance({1, 2, 3}, {1, 2}), 0.0, 1e-9);
}

TEST_CASE("manhattan_hamming_difference_edit_int_vectors", "[distances]") {
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {1, 0, 5};
    ASSERT_EQ(manhattanDistance(a, b), 4);
    ASSERT_EQ(hammingDistance(a, b), 2);
    ASSERT_EQ(difference(a, b), 0);

    std::vector<int> x = {1, 2, 3};
    std::vector<int> y = {1, 2, 3, 9};
    ASSERT_EQ(editDistance(x, y), 1);
}

TEST_CASE("variation_distance_sorted_and_empty", "[distances]") {
    ASSERT_NEAR(
        variationDistance(std::vector<int>{}, std::vector<int>{}),
        0.0,
        1e-9);
    REQUIRE_THROWS_AS(
        variationDistance(std::vector<int>{}, std::vector<int>{1}),
        std::invalid_argument);
    REQUIRE_THROWS_AS(
        variationDistance(std::vector<int>{1}, std::vector<int>{}),
        std::invalid_argument);

    ASSERT_NEAR(
        variationDistance(std::vector<int>{0, 4, 7}, std::vector<int>{0, 4, 7}),
        0.0,
        1e-9);
    double d = variationDistance(std::vector<int>{7, 0, 4}, std::vector<int>{0, 4, 7});
    ASSERT_NEAR(d, 0.0, 1e-9);

    double d2 = variationDistance(std::vector<int>{0, 12}, std::vector<int>{0});
    ASSERT_NEAR(d2, 12.0, 1e-9);
}

TEST_CASE("generalized_neo_riemann_bounds", "[distances]") {
    std::vector<int> in = {0, 4, 7};
    auto o = generalizedNeoRiemann(in, -1, 99);
    ASSERT_VECTOR_EQ(o, in);
    o = generalizedNeoRiemann(in, 3, 5);
    ASSERT_VECTOR_EQ(o, in);
    o = generalizedNeoRiemann(in, 1, 3);
    ASSERT_VECTOR_EQ(o, std::vector<int>({0, 7, 7}));
}

TEST_CASE("transformation_steps_identical_and_resize", "[distances]") {
    auto s = transformationSteps({1, 2}, {1, 2});
    ASSERT_TRUE(s.empty());

    auto add = transformationSteps({0}, {0, 5});
    ASSERT_EQ(add.size(), static_cast<size_t>(1));
    ASSERT_EQ(add[0].first, 1);

    auto rem = transformationSteps({0, 1, 2}, {0, 1});
    ASSERT_TRUE(!rem.empty());
    ASSERT_EQ(rem.back().first, 2);
}

TEST_CASE("weighted_transformation_distance_int", "[distances]") {
    std::vector<int> a = {0, 0};
    std::vector<int> b = {3, 5};
    ASSERT_EQ(weightedTransformationDistance(a, b), 8);
}

TEST_CASE("distance_overloads_interval_vectors", "[distances]") {
    interval_vector iv1({0, 4, 7}, 0, 12);
    interval_vector iv2({1, 5, 8}, 0, 12);
    ASSERT_EQ(hammingDistance(iv1, iv2), 3);
    ASSERT_EQ(manhattanDistance(iv1, iv2), 3);
    ASSERT_NEAR(euclideanDistance(iv1, iv2), std::sqrt(3.0), 1e-9);
    ASSERT_EQ(difference(iv1, iv2), -3);
    ASSERT_EQ(editDistance(iv1, iv2), 3);
    ASSERT_EQ(weightedTransformationDistance(iv1, iv2), 3);
}

TEST_CASE("variation_distance_position_vector_overload", "[distances]") {
    position_vector a({7, 0, 4}, 12, 12);
    position_vector b({0, 4, 7}, 12, 12);
    ASSERT_NEAR(variationDistance(a, b), 0.0, 1e-9);
}

TEST_CASE("compute_cdf_empty", "[distances]") {
    std::vector<double> empty;
    auto cdf = computeCDF(empty);
    ASSERT_TRUE(cdf.empty());
}

} // namespace
