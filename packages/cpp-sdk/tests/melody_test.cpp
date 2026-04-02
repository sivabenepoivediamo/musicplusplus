#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/melody.h"

#include <stdexcept>

namespace {

using musicpp::Analysis;
using musicpp::hierarchy;
using musicpp::join;
using musicpp::normalizeNotes;
using musicpp::ornamentLoop;
using musicpp::ornamentTypesLoop;
using musicpp::parseModifications;
using musicpp::run;
using musicpp::run2;
using musicpp::TripleSelectResult;
using musicpp::tripleSelect;
using musicpp::VectorModification;

TEST_CASE("diminution_length_edge_cases", "[melody]") {
    ASSERT_VECTOR_EQ(musicpp::diminution(5, 0, true, true), musicpp_test::ints({}));
    ASSERT_VECTOR_EQ(musicpp::diminution(5, 1, true, true), musicpp_test::ints({5}));
    auto d = musicpp::diminution(3, 4, true, true);
    ASSERT_EQ(d.size(), static_cast<size_t>(4));
    ASSERT_EQ(d.front(), 3);
    ASSERT_EQ(d.back(), 3);
}

TEST_CASE("diminution_right_neighbor_order", "[melody]") {
    auto d = musicpp::diminution(5, 4, true, false);
    ASSERT_EQ(d.size(), static_cast<size_t>(4));
    ASSERT_EQ(d.front(), 5);
    ASSERT_EQ(d.back(), 5);
    ASSERT_TRUE(d[1] > d[2]);
}

TEST_CASE("run_and_run2", "[melody]") {
    ASSERT_VECTOR_EQ(run(5, 4, true), musicpp_test::ints({2, 3, 4, 5}));
    ASSERT_VECTOR_EQ(run(5, 4, false), musicpp_test::ints({8, 7, 6, 5}));
    ASSERT_VECTOR_EQ(run2(1, 3), musicpp_test::ints({1, 2, 3}));
    ASSERT_VECTOR_EQ(run2(3, 1), musicpp_test::ints({3, 2, 1}));
}

TEST_CASE("normalize_notes_mod", "[melody]") {
    ASSERT_VECTOR_EQ(normalizeNotes({-1, 12, 25}, 12), musicpp_test::ints({11, 0, 1}));
}

TEST_CASE("normalize_notes_empty", "[melody]") {
    ASSERT_TRUE(normalizeNotes({}, 12).empty());
}

TEST_CASE("hierarchy_negative_note_matches_pitch_class", "[melody]") {
    std::vector<int> chord = {0, 4, 7};
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> chrom = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    Analysis a = hierarchy(-1, chord, scale, chrom, 12);
    ASSERT_EQ(a.chromatic.degree, 11);
}

TEST_CASE("hierarchy_and_triple_select", "[melody]") {
    std::vector<int> chord = {0, 4, 7};
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> chrom = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    Analysis a = hierarchy(0, chord, scale, chrom, 12);
    ASSERT_EQ(a.chord.degree, 0);
    ASSERT_EQ(a.scale.degree, 0);
    ASSERT_EQ(a.chromatic.degree, 0);

    std::vector<VectorModification> mods = {{VectorModification::Type::CHORD, 0}};
    TripleSelectResult ts = tripleSelect(a, mods, chord, scale, chrom, 12);
    ASSERT_EQ(ts.results.size(), static_cast<size_t>(1));
    ASSERT_EQ(ts.results[0], 0);
    ASSERT_TRUE(!ts.isOut[0]);
}

TEST_CASE("triple_select_chromatic_type", "[melody]") {
    std::vector<int> chord = {0, 4, 7};
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> chrom = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    Analysis a = hierarchy(0, chord, scale, chrom, 12);
    std::vector<VectorModification> mods = {{VectorModification::Type::CHROMATIC, 0}};
    TripleSelectResult ts = tripleSelect(a, mods, chord, scale, chrom, 12);
    ASSERT_TRUE(!ts.isOut[0]);
    ASSERT_EQ(ts.results[0], 0);
}

TEST_CASE("triple_select_invalid_chord_degree", "[melody]") {
    std::vector<int> chord = {0, 4, 7};
    std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> chrom = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    Analysis a = hierarchy(1, chord, scale, chrom, 12);
    std::vector<VectorModification> mods = {{VectorModification::Type::CHORD, 0}};
    TripleSelectResult ts = tripleSelect(a, mods, chord, scale, chrom, 12);
    ASSERT_TRUE(ts.isOut[0]);
    ASSERT_EQ(ts.results[0], 0);
}

TEST_CASE("parse_modifications_length_mismatch_throws", "[melody]") {
    REQUIRE_THROWS_AS(
        parseModifications({1}, {VectorModification::Type::CHORD, VectorModification::Type::SCALE}),
        std::runtime_error);
}

TEST_CASE("ornament_loop_and_types", "[melody]") {
    ASSERT_VECTOR_EQ(ornamentLoop({1, 2}, 0), musicpp_test::ints({}));
    ASSERT_VECTOR_EQ(ornamentLoop({9, 8}, 3), musicpp_test::ints({8, 9, 8}));
    auto t = ornamentTypesLoop({VectorModification::Type::CHORD, VectorModification::Type::SCALE}, 3);
    ASSERT_EQ(t.size(), static_cast<size_t>(3));
}

TEST_CASE("join_int_vector", "[melody]") {
    ASSERT_EQ(join({1, 2, 3}), std::string("1, 2, 3"));
    ASSERT_EQ(join({}), std::string(""));
}

TEST_CASE("parse_modifications_single_type_broadcast", "[melody]") {
    auto m = parseModifications({1, 2}, {VectorModification::Type::SCALE});
    ASSERT_EQ(m.size(), static_cast<size_t>(2));
    ASSERT_TRUE(m[0].type == VectorModification::Type::SCALE);
    ASSERT_EQ(m[1].delta, 2);
}

TEST_CASE("parse_modifications_paired_types", "[melody]") {
    auto m = parseModifications(
        {2, -1},
        {VectorModification::Type::SCALE, VectorModification::Type::CHROMATIC});
    ASSERT_EQ(m.size(), static_cast<size_t>(2));
    ASSERT_TRUE(m[0].type == VectorModification::Type::SCALE);
    ASSERT_TRUE(m[1].type == VectorModification::Type::CHROMATIC);
    ASSERT_EQ(m[0].delta, 2);
    ASSERT_EQ(m[1].delta, -1);
}

} // namespace
