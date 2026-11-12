#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/chord_names.h"

#include <stdexcept>

namespace {

using musicpp::analyzeChord;
using musicpp::buildChordName;
using musicpp::intervalToString;
using musicpp::noteToString;

TEST_CASE("note_to_string_octave_wrap", "[chord_names]") {
    ASSERT_EQ(noteToString(60), "C");
    ASSERT_EQ(noteToString(61), "C#");
    ASSERT_EQ(noteToString(72), "C");
}

TEST_CASE("note_to_string_negative_midi_pitch_class", "[chord_names]") {
    ASSERT_EQ(noteToString(-1), "B");
    ASSERT_EQ(noteToString(-13), "B");
    ASSERT_EQ(noteToString(-12), "C");
}

TEST_CASE("interval_to_string_known_and_fallback", "[chord_names]") {
    ASSERT_EQ(intervalToString(3), "m3");
    ASSERT_EQ(intervalToString(4), "M3");
    ASSERT_EQ(intervalToString(999), "999");
}

TEST_CASE("analyze_chord_invalid_root_throws", "[chord_names][safety]") {
    REQUIRE_THROWS_AS(analyzeChord({}, 0), std::out_of_range);
    std::vector<int> one = {60};
    REQUIRE_THROWS_AS(analyzeChord(one, -1), std::out_of_range);
    REQUIRE_THROWS_AS(analyzeChord(one, 1), std::out_of_range);
}

TEST_CASE("analyze_chord_nonzero_root_index", "[chord_names]") {
    std::vector<int> inv = {64, 60, 67};
    auto a = analyzeChord(inv, 1);
    ASSERT_EQ(a.root, 60);
    ASSERT_TRUE(a.hasMajorThird);
    ASSERT_TRUE(a.hasPerfectFifth);
}

TEST_CASE("build_chord_name_minor_seventh", "[chord_names]") {
    std::vector<int> dmin7 = {62, 65, 69, 72};
    auto name = buildChordName(analyzeChord(dmin7, 0));
    ASSERT_EQ(name, "Dmin7");
}

TEST_CASE("build_chord_name_major_and_diminished_triads", "[chord_names]") {
    ASSERT_EQ(buildChordName(analyzeChord({60, 64, 67}, 0)), "C");
    ASSERT_EQ(buildChordName(analyzeChord({60, 63, 66}, 0)), "Cdim");
}

TEST_CASE("build_chord_name_sus4", "[chord_names]") {
    ASSERT_EQ(buildChordName(analyzeChord({60, 65, 67}, 0)), "Csus4");
}

TEST_CASE("build_chord_name_sus2_and_aug7", "[chord_names]") {
    ASSERT_EQ(buildChordName(analyzeChord({60, 62, 67}, 0)), "Csus2");
    ASSERT_EQ(buildChordName(analyzeChord({60, 64, 68, 70}, 0)), "Caug7");
}

TEST_CASE("interval_to_string_perfect_fifth", "[chord_names]") {
    ASSERT_EQ(intervalToString(7), "5");
}

TEST_CASE("build_chord_name_seventh_sus_and_half_diminished", "[chord_names]") {
    ASSERT_EQ(buildChordName(analyzeChord({60, 65, 67, 71}, 0)), "Cmaj7sus4");
    ASSERT_EQ(buildChordName(analyzeChord({60, 62, 67, 70}, 0)), "C7sus2");
    ASSERT_EQ(buildChordName(analyzeChord({60, 63, 66, 70}, 0)), "Cmin7b5");
}

} // namespace
