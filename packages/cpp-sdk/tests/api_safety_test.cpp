#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/distances.h"
#include "../src/matrix_distance.h"
#include "../src/melody.h"
#include "../src/rhythm_gen.h"
#include "../src/slonimsky.h"
#include "../src/vectors.h"

#include <stdexcept>

namespace {

using musicpp::align;
using musicpp::Analysis;
using musicpp::binary_vector;
using musicpp::euclidean;
using musicpp::hierarchy;
using musicpp::positions_to_binary;
using musicpp::interval_vector;
using musicpp::ModalMatrixDistance;
using musicpp::ModalRototranslationMatrixDistance;
using musicpp::ModalSelectionMatrixDistance;
using musicpp::normalize;
using musicpp::position_vector;
using musicpp::RototranslationMatrixDistance;
using musicpp::TranspositionMatrixDistance;
using musicpp::tripleSelect;
using musicpp::vector_set;
using musicpp::VectorModification;

TEST_CASE("normalize_empty_vector_throws", "[distances][safety]") {
    std::vector<int> empty;
    REQUIRE_THROWS_AS(normalize(empty), std::invalid_argument);
}

TEST_CASE("binary_vector_constructor_rejects_non_binary", "[binary_vector][safety]") {
    REQUIRE_THROWS_AS((void)binary_vector(std::vector<int>{1, 2}, 0, 4), std::invalid_argument);
}

TEST_CASE("binary_vector_validate_after_mutating_data", "[binary_vector][safety]") {
    binary_vector bv({1, 0, 1, 0}, 0, 4);
    bv.data()[1] = 7;
    REQUIRE_THROWS_AS(bv.validateBinaryData(), std::invalid_argument);
}

TEST_CASE("binary_vector_scalar_multiply_non_positive_throws", "[binary_vector][safety]") {
    binary_vector bv({1, 0, 0, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv * 0, std::invalid_argument);
    REQUIRE_THROWS_AS(bv * -1, std::invalid_argument);
}

TEST_CASE("binary_vector_scalar_divide_invalid_throws", "[binary_vector][safety]") {
    binary_vector bv({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv / 0, std::invalid_argument);
    REQUIRE_THROWS_AS(bv / 5, std::invalid_argument);
    REQUIRE_THROWS_AS(bv / 3, std::invalid_argument);
}

TEST_CASE("binary_vector_deprecated_divide_and_stretch_non_positive", "[binary_vector][safety]") {
    binary_vector bv({1, 0, 0, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv.divide(0), std::invalid_argument);
    REQUIRE_THROWS_AS(bv.stretch(0), std::invalid_argument);
}

TEST_CASE("binary_vector_static_euclidean_invalid_parameters", "[binary_vector][safety]") {
    REQUIRE_THROWS_AS(binary_vector::euclidean(0, 8), std::invalid_argument);
    REQUIRE_THROWS_AS(binary_vector::euclidean(3, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(binary_vector::euclidean(5, 4), std::invalid_argument);
}

TEST_CASE("vector_set_multiply_binary_non_positive_scalar", "[vectors][safety]") {
    vector_set s = vector_set::from_binary({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(s.multiply_binary(0), std::invalid_argument);
}

TEST_CASE("vector_set_divide_binary_propagates_binary_rules", "[vectors][safety]") {
    vector_set s = vector_set::from_binary({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(s.divide_binary(3), std::invalid_argument);
}

TEST_CASE("position_vector_componentwise_division_empty_divisor", "[position_vector][safety]") {
    position_vector p({4, 8}, 12, 12);
    std::vector<int> empty;
    REQUIRE_THROWS_AS(p.componentwiseDivision(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(p.componentwiseModulo(empty), std::invalid_argument);
}

TEST_CASE("interval_vector_componentwise_division_empty_divisor", "[interval_vector][safety]") {
    interval_vector iv({2, 2, 2}, 0, 12);
    std::vector<int> empty;
    REQUIRE_THROWS_AS(iv.componentwiseDivision(empty), std::invalid_argument);
    REQUIRE_THROWS_AS(iv.componentwiseModulo(empty), std::invalid_argument);
}

TEST_CASE("triple_select_invalid_modification_type_throws", "[melody][safety]") {
    std::vector<int> chord = {0, 4, 7};
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> chrom = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    Analysis a = hierarchy(0, chord, scale, chrom, 12);
    const auto badType = static_cast<VectorModification::Type>(99);
    std::vector<VectorModification> mods = {{badType, 0}};
    REQUIRE_THROWS_AS(tripleSelect(a, mods, chord, scale, chrom, 12), std::runtime_error);
}

TEST_CASE("rhythm_euclidean_non_positive_args_throws", "[rhythm_gen][safety]") {
    REQUIRE_THROWS_AS(euclidean(12, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(euclidean(0, 4), std::invalid_argument);
    REQUIRE_THROWS_AS(euclidean(-1, 3), std::invalid_argument);
}

TEST_CASE("positions_to_binary_non_positive_range_returns_empty_pattern", "[vectors][safety]") {
    position_vector pv({0, 4, 7}, 12, 12);
    pv.set_effective_range(0);
    binary_vector b = positions_to_binary(pv);
    ASSERT_TRUE(b.data().empty());
    ASSERT_EQ(b.mod(), pv.mod());
}

TEST_CASE("align_empty_or_bad_range_throws", "[matrix_distance][safety]") {
    position_vector a({0, 4, 7}, 12, 12);
    position_vector b({2, 5, 9}, 12, 12);
    position_vector empty;
    REQUIRE_THROWS_AS(align(empty, b), std::invalid_argument);
    REQUIRE_THROWS_AS(align(a, empty), std::invalid_argument);
    position_vector z({0, 4, 7}, 12, 12);
    z.set_effective_range(0);
    REQUIRE_THROWS_AS(align(z, b), std::invalid_argument);
}

TEST_CASE("slonimsky_min_interval_too_short_throws", "[slonimsky][safety]") {
    using musicpp::slonimsky::minInterval;
    REQUIRE_THROWS_AS(minInterval({}), std::invalid_argument);
    REQUIRE_THROWS_AS(minInterval({0}), std::invalid_argument);
}

TEST_CASE("slonimsky_intervals_short_sequences", "[slonimsky][safety]") {
    using musicpp::slonimsky::intervals;
    ASSERT_TRUE(intervals({}).empty());
    ASSERT_TRUE(intervals({5}).empty());
}

TEST_CASE("matrix_distance_empty_at_throws_out_of_range", "[matrix_distance][safety]") {
    ModalMatrixDistance<position_vector> modal;
    TranspositionMatrixDistance trans;
    RototranslationMatrixDistance roto;
    ModalSelectionMatrixDistance<position_vector> sel;
    ModalRototranslationMatrixDistance mrt;
    REQUIRE_THROWS_AS((void)modal.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)trans.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)roto.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)sel.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)mrt.at(0), std::out_of_range);
}

} // namespace
