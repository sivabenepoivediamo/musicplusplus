#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/chord.h"
#include "../src/chordNames.h"

namespace {

TEST_CASE("chord_helper_examples", "[chord]") {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    PositionVector triad_degrees({0, 2, 4}, 12);
    IntervalVector grouping({2}, 0, 12);

    auto positions_from_degrees = chord(c_major_scale, triad_degrees, 0, 0, 5, 0);
    auto positions_from_grouping = chord(c_major_scale, grouping, 0, 0, 5, 0);
    auto intervals_from_grouping = chord(major_scale_intervals, grouping, 0, 0, 5, 0);
    auto intervals_from_degrees = chord(major_scale_intervals, triad_degrees, 0, 0, 5, 0);

    ASSERT_POSITION_VECTOR_EQ(positions_from_degrees, musicpp_test::ints({0, 4, 7, 12, 16}), 12);
    ASSERT_POSITION_VECTOR_EQ(positions_from_grouping, musicpp_test::ints({0, 4, 7, 11, 14}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(intervals_from_grouping, musicpp_test::ints({4, 3, 4, 3, 3}), 0, 12);
    ASSERT_INTERVAL_VECTOR_EQ(intervals_from_degrees, musicpp_test::ints({4, 3, 5, 4, 3}), 0, 12);

    TEST_CASE_LOG("chord_helper");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("intervals", major_scale_intervals);
    TEST_INPUT("triad_degrees", triad_degrees);
    TEST_INPUT("grouping", grouping);
    TEST_OUTPUT("positions_from_degrees", positions_from_degrees);
    TEST_OUTPUT("positions_from_grouping", positions_from_grouping);
    TEST_OUTPUT("intervals_from_grouping", intervals_from_grouping);
    TEST_OUTPUT("intervals_from_degrees", intervals_from_degrees);
}

TEST_CASE("chord_class_examples", "[chord]") {
    PositionVector c_major_scale({0, 2, 4, 5, 7, 9, 11}, 12);
    IntervalVector major_scale_intervals({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    PositionVector triad_degrees({0, 2, 4}, 12);
    IntervalVector grouping({2}, 0, 12);

    ChordParams params;
    params.withShift(0).withPosition(1).withPreVoices(3);

    Chord chord1(c_major_scale, triad_degrees, params);
    Chord chord2(c_major_scale, grouping, params);
    Chord chord3(major_scale_intervals, grouping, params);
    Chord chord4(major_scale_intervals, triad_degrees, params);

    ASSERT_POSITION_VECTOR_EQ(chord1.toPositions(), musicpp_test::ints({4, 7, 12}), 12);
    ASSERT_POSITION_VECTOR_EQ(chord2.toPositions(), musicpp_test::ints({4, 7, 12}), 12);
    ASSERT_INTERVAL_VECTOR_EQ(chord3.toIntervals(), musicpp_test::ints({3, 4, 4}), 4, 12);
    ASSERT_INTERVAL_VECTOR_EQ(chord4.toIntervals(), musicpp_test::ints({3, 5, 4}), 4, 12);

    chord1.setRotationOrRototrans(2);
    ASSERT_POSITION_VECTOR_EQ(chord1.toPositions(), musicpp_test::ints({12, 16, 19}), 12);

    ChordParams inverted_params;
    inverted_params.withShift(0).withPosition(0).withPreVoices(3).withInvert(true).withAxis(6);
    Chord inverted_chord(c_major_scale, triad_degrees, inverted_params);
    ASSERT_POSITION_VECTOR_EQ(inverted_chord.toPositions(), musicpp_test::ints({-7, -4, 0}), 12);

    TEST_CASE_LOG("chord_class");
    TEST_INPUT("scale", c_major_scale);
    TEST_INPUT("intervals", major_scale_intervals);
    TEST_INPUT("triad_degrees", triad_degrees);
    TEST_INPUT("grouping", grouping);
    TEST_OUTPUT("positions", chord1.toPositions());
    TEST_OUTPUT("grouping_positions", chord2.toPositions());
    TEST_OUTPUT("grouping_intervals", chord3.toIntervals());
    TEST_OUTPUT("triad_degree_intervals", chord4.toIntervals());
    TEST_OUTPUT("inverted_positions", inverted_chord.toPositions());
}

TEST_CASE("chord_name_examples", "[chord]") {
    std::vector<int> c_major = {60, 64, 67};
    std::vector<int> c_minor = {60, 63, 67};
    std::vector<int> c_dom7 = {60, 64, 67, 70};
    std::vector<int> diminished7 = {60, 63, 66, 69};
    std::vector<int> d_minor7 = {62, 65, 69, 72};

    auto c_major_name = buildChordName(analyzeChord(c_major, 0));
    auto c_minor_name = buildChordName(analyzeChord(c_minor, 0));
    auto c_dom7_name = buildChordName(analyzeChord(c_dom7, 0));
    auto diminished7_name = buildChordName(analyzeChord(diminished7, 0));
    auto d_minor7_name = buildChordName(analyzeChord(d_minor7, 0));

    ASSERT_EQ(c_major_name, "C");
    ASSERT_EQ(c_minor_name, "Cmin");
    ASSERT_EQ(c_dom7_name, "C7");
    ASSERT_EQ(diminished7_name, "Cdim7");
    ASSERT_EQ(d_minor7_name, "Dmin7");

    TEST_CASE_LOG("chord_names");
    TEST_INPUT("c_major", c_major);
    TEST_INPUT("c_minor", c_minor);
    TEST_INPUT("c_dom7", c_dom7);
    TEST_INPUT("diminished7", diminished7);
    TEST_INPUT("d_minor7", d_minor7);
    TEST_OUTPUT("c_major", c_major_name);
    TEST_OUTPUT("c_minor", c_minor_name);
    TEST_OUTPUT("c_dom7", c_dom7_name);
    TEST_OUTPUT("diminished7", diminished7_name);
    TEST_OUTPUT("d_minor7", d_minor7_name);
}

} // namespace
