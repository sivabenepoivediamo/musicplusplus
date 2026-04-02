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
using musicpp::onset_vector;
using musicpp::euclidean;
using musicpp::hierarchy;
using musicpp::positions_to_onset;
using musicpp::interval_vector;
using musicpp::ModalMatrixDistance;
using musicpp::ModalRelativeModeMatrixDistance;
using musicpp::ModalSelectionMatrixDistance;
using musicpp::normalize;
using musicpp::position_vector;
using musicpp::RelativeModeMatrixDistance;
using musicpp::TranspositionMatrixDistance;
using musicpp::tripleSelect;
using musicpp::vector_set;
using musicpp::VectorModification;

TEST_CASE("normalize_empty_vector_throws", "[distances][safety]") {
    std::vector<int> empty;
    REQUIRE_THROWS_AS(normalize(empty), std::invalid_argument);
}

TEST_CASE("onset_vector_constructor_rejects_invalid_pattern", "[onset_vector][safety]") {
    REQUIRE_THROWS_AS((void)onset_vector(std::vector<int>{1, 2}, 0, 4), std::invalid_argument);
}

TEST_CASE("onset_vector_validate_after_mutating_data", "[onset_vector][safety]") {
    onset_vector bv({1, 0, 1, 0}, 0, 4);
    bv.data()[1] = 7;
    REQUIRE_THROWS_AS(bv.validate_onset_pattern(), std::invalid_argument);
}

TEST_CASE("onset_vector_scalar_multiply_non_positive_throws", "[onset_vector][safety]") {
    onset_vector bv({1, 0, 0, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv * 0, std::invalid_argument);
    REQUIRE_THROWS_AS(bv * -1, std::invalid_argument);
}

TEST_CASE("onset_vector_scalar_divide_invalid_throws", "[onset_vector][safety]") {
    onset_vector bv({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv / 0, std::invalid_argument);
    REQUIRE_THROWS_AS(bv / 5, std::invalid_argument);
    REQUIRE_THROWS_AS(bv / 3, std::invalid_argument);
}

TEST_CASE("onset_vector_deprecated_divide_and_stretch_non_positive", "[onset_vector][safety]") {
    onset_vector bv({1, 0, 0, 0}, 0, 4);
    REQUIRE_THROWS_AS(bv.divide(0), std::invalid_argument);
    REQUIRE_THROWS_AS(bv.stretch(0), std::invalid_argument);
}

TEST_CASE("onset_vector_static_euclidean_invalid_parameters", "[onset_vector][safety]") {
    REQUIRE_THROWS_AS(onset_vector::euclidean(0, 8), std::invalid_argument);
    REQUIRE_THROWS_AS(onset_vector::euclidean(3, 0), std::invalid_argument);
    REQUIRE_THROWS_AS(onset_vector::euclidean(5, 4), std::invalid_argument);
}

TEST_CASE("vector_set_multiply_onset_non_positive_scalar", "[vectors][safety]") {
    vector_set s = vector_set::from_onset({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(s.multiply_onset(0), std::invalid_argument);
}

TEST_CASE("vector_set_divide_onset_propagates_onset_rules", "[vectors][safety]") {
    vector_set s = vector_set::from_onset({1, 0, 1, 0}, 0, 4);
    REQUIRE_THROWS_AS(s.divide_onset(3), std::invalid_argument);
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

TEST_CASE("positions_to_onset_non_positive_range_returns_empty_pattern", "[vectors][safety]") {
    position_vector pv({0, 4, 7}, 12, 12);
    pv.set_effective_range(0);
    onset_vector b = positions_to_onset(pv);
    ASSERT_TRUE(b.data().empty());
    ASSERT_EQ(b.mod(), pv.mod());
}

TEST_CASE("align_empty_or_bad_range_throws", "[matrix_distance][safety]") {
    position_vector a({0, 4, 7}, 12, 12);
    position_vector b({2, 5, 9}, 12, 12);
    position_vector empty(std::vector<int>{}, 12, 12, true, false);
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
    RelativeModeMatrixDistance roto;
    ModalSelectionMatrixDistance<position_vector> sel;
    ModalRelativeModeMatrixDistance mrt;
    REQUIRE_THROWS_AS((void)modal.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)trans.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)roto.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)sel.at(0), std::out_of_range);
    REQUIRE_THROWS_AS((void)mrt.at(0), std::out_of_range);
}

} // namespace
