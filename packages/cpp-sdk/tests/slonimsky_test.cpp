#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/slonimsky.h"

namespace {

TEST_CASE("slonimsky_examples", "[slonimsky]") {
    using namespace musicpp::slonimsky;

    const Seq x = {0, 6, 12};

    ASSERT_EQ(minInterval(x), 6);
    ASSERT_VECTOR_EQ(interpolation(x, 2), std::vector<int>({0, 2, 6, 8, 12}));
    ASSERT_VECTOR_EQ(interpolation(x, {2, 4}), std::vector<int>({0, 2, 4, 6, 8, 10, 12}));
    ASSERT_VECTOR_EQ(interpolation(x, {1, 2, 4}), std::vector<int>({0, 1, 2, 4, 6, 7, 8, 10, 12}));

    ASSERT_VECTOR_EQ(symmetricInterpolation(x, 2), std::vector<int>({0, 2, 6, 10, 6, 8, 12}));
    ASSERT_VECTOR_EQ(symmetricInterpolation(x, {2, 4}), std::vector<int>({0, 2, 4, 6, 8, 10, 6, 8, 10, 12}));

    ASSERT_VECTOR_EQ(
        asymmetricInterpolation(x, {2, 4}, {1, 3}),
        std::vector<int>({0, 2, 4, 6, 9, 11, 6, 8, 10, 12}));

    ASSERT_VECTOR_EQ(ultrapolation(x, 2), std::vector<int>({0, 8, 6, 14, 12}));
    ASSERT_VECTOR_EQ(ultrapolation(x, {2, 4}), std::vector<int>({0, 10, 8, 6, 16, 14, 12}));

    ASSERT_VECTOR_EQ(infrapolation(x, 2), std::vector<int>({0, -2, 6, 4, 12}));
    ASSERT_VECTOR_EQ(infrapolation(x, {2, 4}), std::vector<int>({0, -2, -4, 6, 4, 2, 12}));
    ASSERT_VECTOR_EQ(infrapolation(x, {1, 3, 5}), std::vector<int>({0, -1, -3, -5, 6, 5, 3, 1, 12}));
    ASSERT_EQ(infrapolationPermutation(3), std::vector<int>({2, 0, 1}));

    ASSERT_VECTOR_EQ(infraInterpolation(x, 2, 3), std::vector<int>({0, -2, 3, 6, 4, 9, 12}));
    ASSERT_VECTOR_EQ(interInfrapolation(x, 3, 2), std::vector<int>({0, 3, -2, 6, 9, 4, 12}));
    ASSERT_VECTOR_EQ(infraUltrapolation(x, 2, 3), std::vector<int>({0, -2, 9, 6, 4, 15, 12}));
    ASSERT_VECTOR_EQ(interUltrapolation(x, 2, 3), std::vector<int>({0, 2, 3, 6, 8, 9, 12}));
    ASSERT_VECTOR_EQ(ultraInterpolation(x, 2, 3), std::vector<int>({0, 8, 3, 6, 14, 9, 12}));
    ASSERT_VECTOR_EQ(infraInterUltrapolation(x, 1, 1, 1), std::vector<int>({0, -1, 1, 7, 6, 5, 7, 13, 12}));
    ASSERT_VECTOR_EQ(interInfraInterpolation(x, 1, 2, 3), std::vector<int>({0, 5, -2, 3, 6, 11, 4, 9, 12}));
    ASSERT_VECTOR_EQ(ultraInfraInterpolation(x, 1, 2, 3), std::vector<int>({0, 7, -2, 3, 6, 13, 4, 9, 12}));

    TEST_CASE_LOG("slonimsky_sequences");
    TEST_INPUT("source", x);
    TEST_OUTPUT("interpolation_2", interpolation(x, 2));
    TEST_OUTPUT("interpolation_2_4", interpolation(x, {2, 4}));
    TEST_OUTPUT("symmetric_2_4", symmetricInterpolation(x, {2, 4}));
    TEST_OUTPUT("ultrapolation_2_4", ultrapolation(x, {2, 4}));
    TEST_OUTPUT("infrapolation_2_4", infrapolation(x, {2, 4}));
    TEST_OUTPUT("ultra_infra_1_2_3", ultraInfraInterpolation(x, 1, 2, 3));
}

} // namespace
