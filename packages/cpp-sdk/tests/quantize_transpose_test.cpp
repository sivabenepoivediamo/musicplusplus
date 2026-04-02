#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/quantize_transpose.h"

namespace {

using musicpp::position_vector;
using musicpp::quantize;
using musicpp::transpose;

TEST_CASE("quantize_exact_and_between_neighbors", "[quantize_transpose]") {
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    ASSERT_EQ(quantize(4, scale, true), 4);
    ASSERT_EQ(quantize(4, scale, false), 4);
    ASSERT_EQ(quantize(3, scale, true), 2);
    ASSERT_EQ(quantize(3, scale, false), 4);
}

TEST_CASE("quantize_outside_range", "[quantize_transpose]") {
    std::vector<int> scale = {2, 4, 7};
    ASSERT_EQ(quantize(0, scale, true), 2);
    ASSERT_EQ(quantize(100, scale, false), 7);
}

TEST_CASE("quantize_empty_scale_returns_negative_one", "[quantize_transpose]") {
    std::vector<int> empty;
    ASSERT_EQ(quantize(5, empty, true), -1);
}

TEST_CASE("transpose_c_major_to_c_major_degrees", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    auto pr = transpose(inScale, outScale, 0, 0, {0, 4, 7}, outDeg, outNotes);
    ASSERT_POSITION_VECTOR_EQ(pr.first, musicpp_test::ints({0, 2, 4}), 7);
    ASSERT_POSITION_VECTOR_EQ(pr.second, musicpp_test::ints({0, 4, 7}), 12);
    ASSERT_POSITION_VECTOR_EQ(outDeg, pr.first.data(), 7);
    ASSERT_POSITION_VECTOR_EQ(outNotes, pr.second.data(), 12);
}

TEST_CASE("transpose_with_roots_and_octave", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    transpose(inScale, outScale, 0, 0, {12, 16}, outDeg, outNotes);
    ASSERT_POSITION_VECTOR_EQ(outNotes, musicpp_test::ints({12, 16}), 12);
}

TEST_CASE("transpose_negative_pitch_class_handling", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    transpose(inScale, outScale, 0, 0, {-1}, outDeg, outNotes);
    ASSERT_TRUE(!outNotes.data().empty());
}

TEST_CASE("transpose_empty_note_list", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    auto pr = transpose(inScale, outScale, 0, 0, {}, outDeg, outNotes);
    ASSERT_TRUE(pr.first.data().empty());
    ASSERT_TRUE(pr.second.data().empty());
    ASSERT_TRUE(outDeg.data().empty());
    ASSERT_TRUE(outNotes.data().empty());
}

TEST_CASE("transpose_seven_note_scale_to_pentatonic_wrap_degrees", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 7, 9}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    transpose(inScale, outScale, 0, 0, {0, 11}, outDeg, outNotes);
    ASSERT_POSITION_VECTOR_EQ(outDeg, musicpp_test::ints({0, 6}), 7);
    ASSERT_POSITION_VECTOR_EQ(outNotes, musicpp_test::ints({0, 2}), 12);
}

TEST_CASE("transpose_nonzero_out_root", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    transpose(inScale, outScale, 0, 12, {0}, outDeg, outNotes);
    ASSERT_POSITION_VECTOR_EQ(outNotes, musicpp_test::ints({12}), 12);
}

TEST_CASE("transpose_nonzero_in_root", "[quantize_transpose]") {
    position_vector inScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outScale({0, 2, 4, 5, 7, 9, 11}, 12, 12);
    position_vector outDeg;
    position_vector outNotes;
    transpose(inScale, outScale, 2, 0, {2}, outDeg, outNotes);
    ASSERT_POSITION_VECTOR_EQ(outNotes, musicpp_test::ints({0}), 12);
}

} // namespace
