#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/rhythm_gen.h"

#include <vector>

namespace {

using namespace musicpp;

TEST_CASE("euclidean_interval_vector_even_division", "[rhythm][util]") {
    std::vector<int> steps = euclidean(12, 4);
    ASSERT_VECTOR_EQ(steps, musicpp_test::ints({3, 3, 3, 3}));
}

TEST_CASE("phrase_length_formula", "[rhythm][util]") {
    ASSERT_EQ(phraseLength(10, 2, 12, 2, 1), 32);
    ASSERT_EQ(phraseLength(0, 0, 8, 0, 2), 0);
}

TEST_CASE("tihai_generator_rounds_length", "[rhythm][util]") {
    auto p = tihaiGenerator(16, 3);
    ASSERT_EQ(p.first, 4);
    ASSERT_EQ(p.second, 2);
}

TEST_CASE("tihai_edge_steps_and_repetitions", "[rhythm][util]") {
    auto a = tihai(2, 5, false);
    ASSERT_EQ(a.size(), static_cast<size_t>(2));
    ASSERT_TRUE(isAllOnes(a));

    auto z = tihai(8, 0, false);
    ASSERT_EQ(z.size(), static_cast<size_t>(8));
    ASSERT_TRUE(isAllZeros(z));

    auto one_rep = tihai(6, 1, false);
    ASSERT_TRUE(isAllOnes(one_rep));
}

TEST_CASE("is_all_zeros_and_ones", "[rhythm][util]") {
    ASSERT_TRUE(isAllZeros({0, 0, 0}));
    ASSERT_TRUE(!isAllZeros({0, 1}));
    ASSERT_TRUE(isAllOnes({1, 1}));
    ASSERT_TRUE(!isAllOnes({1, 0}));
}

TEST_CASE("append_ones_and_cut", "[rhythm][util]") {
    std::vector<int> v = {1};
    appendOnes(v, 4);
    ASSERT_VECTOR_EQ(v, musicpp_test::ints({1, 1, 1, 1}));

    ASSERT_VECTOR_EQ(cut({1, 2, 3, 4}, 2), musicpp_test::ints({1, 2}));
    ASSERT_VECTOR_EQ(cut({9}, 10), musicpp_test::ints({9}));
}

TEST_CASE("tihai_reader_matches_generator_bols_dams", "[rhythm][util]") {
    auto gen = tihaiGenerator(16, 3);
    std::vector<int> pat = tihaiReader(gen.first, gen.second, 3, 16);
    ASSERT_EQ(pat.size(), static_cast<size_t>(16));
}

TEST_CASE("clough_douthett_positions", "[rhythm][util]") {
    ASSERT_VECTOR_EQ(CloughDouthett(12, 4), musicpp_test::ints({0, 3, 6, 9}));
    ASSERT_VECTOR_EQ(CloughDouthett(8, 3), musicpp_test::ints({0, 2, 5}));
}

TEST_CASE("deep_rhythm_sorted_positions", "[rhythm][util]") {
    ASSERT_VECTOR_EQ(deepRhythm(8, 4, 3), musicpp_test::ints({0, 1, 3, 6}));
}

TEST_CASE("clough_douthett_vector_offset", "[rhythm][util]") {
    position_vector pv = CloughDouthettVector(12, 4, 5);
    ASSERT_POSITION_VECTOR_EQ(pv, musicpp_test::ints({5, 8, 11, 14}), 12);
}

TEST_CASE("deep_rhythm_position_vector_offset", "[rhythm][util]") {
    position_vector pv = deepRhythm(8, 4, 3, 10);
    ASSERT_POSITION_VECTOR_EQ(pv, musicpp_test::ints({10, 11, 13, 16}), 8);
}

TEST_CASE("euclidean_interval_vector_odd_split", "[rhythm][util]") {
    interval_vector iv = euclidean(12, 5, 2);
    ASSERT_EQ(iv.size(), static_cast<size_t>(5));
    ASSERT_EQ(iv.mod(), 12);
    ASSERT_EQ(iv.offset(), 2);
    int sum = 0;
    for (int x : iv.data()) {
        sum += x;
    }
    ASSERT_EQ(sum, 12);
}

TEST_CASE("tihai_onset_vector_wrap", "[rhythm][util]") {
    onset_vector bv = tihai(8, 3, false, 1);
    ASSERT_EQ(bv.mod(), 8);
    ASSERT_EQ(bv.size(), static_cast<size_t>(8));
    ASSERT_EQ(bv.offset(), 1);
}

} // namespace
