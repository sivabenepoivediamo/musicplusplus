#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/note_names.h"

namespace {

using namespace musicpp;

TEST_CASE("midi_note_name_examples", "[note_names]") {
    NoteNamingSystem system;
    NoteResult harmonic_major = system.midiNumbersToNoteNames(
        {1, 3, 5, 6, 8, 9, 12},
        NoteMapperOptions(true, true, 12));
    ASSERT_STRING_VECTOR_EQ(harmonic_major.noteNames, std::vector<std::string>({"C♯", "D♯", "E♯", "F♯", "G♯", "A", "B♯"}));

    NoteResult altered_diatonic_flats = system.midiNumbersToNoteNames(
        {0, 1, 3, 4, 6, 8, 10},
        NoteMapperOptions(false, true, 12));
    ASSERT_STRING_VECTOR_EQ(altered_diatonic_flats.noteNames, std::vector<std::string>({"C", "D♭", "E♭", "F♭", "G♭", "A♭", "B♭"}));

    NoteResult chromatic_flats = system.midiNumbersToNoteNames(
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        NoteMapperOptions(false, false, 12));
    ASSERT_STRING_VECTOR_EQ(
        chromatic_flats.noteNames,
        std::vector<std::string>({"C", "D♭", "D", "E♭", "E", "F", "G♭", "G", "A♭", "A", "B♭", "B"}));

    TEST_CASE_LOG("harmonic_major_midi");
    TEST_INPUT("midi", musicpp_test::ints({1, 3, 5, 6, 8, 9, 12}));
    TEST_OUTPUT("note_names", harmonic_major.noteNames);

    TEST_CASE_LOG("altered_diatonic_flats_midi");
    TEST_INPUT("midi", musicpp_test::ints({0, 1, 3, 4, 6, 8, 10}));
    TEST_OUTPUT("note_names", altered_diatonic_flats.noteNames);

    TEST_CASE_LOG("chromatic_flats_midi");
    TEST_INPUT("midi", musicpp_test::ints({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}));
    TEST_OUTPUT("note_names", chromatic_flats.noteNames);
}

TEST_CASE("position_vector_note_name_examples", "[note_names]") {
    NoteNamingSystem system;
    NoteMapperOptions sharps_diatonic(true, true, 12);
    NoteMapperOptions flats_diatonic(false, true, 12);
    NoteMapperOptions sharps_non_diatonic(true, false, 12);

    position_vector c_major({0, 2, 4, 5, 7, 9, 11}, 12);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(c_major, sharps_diatonic).noteNames,
        std::vector<std::string>({"C", "D", "E", "F", "G", "A", "B"}));

    position_vector f_major({5, 7, 9, 10, 0, 2, 4}, 12);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(f_major, flats_diatonic).noteNames,
        std::vector<std::string>({"F", "G", "A", "B♭", "C", "D", "E"}));

    position_vector g7_chord({7, 11, 2, 5}, 12);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(g7_chord, sharps_non_diatonic).noteNames,
        std::vector<std::string>({"G", "B", "D", "F"}));

    position_vector microtonal({0, 5, 10, 13, 18, 23, 28}, 31);
    microtonal = microtonal.relative_mode(2);
    NoteResult microtonal_result = system.positionVectorToNoteNames(microtonal, NoteMapperOptions(false, false, 31));
    ASSERT_STRING_VECTOR_EQ(
        microtonal_result.noteNames,
        std::vector<std::string>({"E", "F", "G", "A", "B", "C", "D"}));
    ASSERT_STRING_VECTOR_EQ(
        microtonal_result.centsInfo,
        std::vector<std::string>({
            "E -13 cents",
            "F +3 cents",
            "G -3 cents",
            "A -10 cents",
            "B -16 cents",
            "D -6 cents",
        }));

    position_vector transposed = c_major + 5;
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(transposed, sharps_diatonic).noteNames,
        std::vector<std::string>({"F", "G", "A", "B♭", "C", "D", "E"}));

    position_vector rotated = c_major.rotate(2);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(rotated, flats_diatonic).noteNames,
        std::vector<std::string>({"A", "B", "C", "D", "E", "F", "G"}));

    position_vector c_major_chord({0, 4, 7}, 12);
    position_vector inverted = c_major_chord.inversion(0);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(inverted, sharps_non_diatonic).noteNames,
        std::vector<std::string>({"F", "G♯", "C"}));

    position_vector pentatonic({0, 2, 4, 7, 9}, 12);
    ASSERT_STRING_VECTOR_EQ(
        system.positionVectorToNoteNames(pentatonic.complement(), sharps_non_diatonic).noteNames,
        std::vector<std::string>({"C♯", "D♯", "F", "F♯", "G♯", "A♯", "B"}));

    TEST_CASE_LOG("c_major_positions");
    TEST_INPUT("positions", c_major);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(c_major, sharps_diatonic).noteNames);

    TEST_CASE_LOG("f_major_positions");
    TEST_INPUT("positions", f_major);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(f_major, flats_diatonic).noteNames);

    TEST_CASE_LOG("g7_chord_positions");
    TEST_INPUT("positions", g7_chord);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(g7_chord, sharps_non_diatonic).noteNames);

    TEST_CASE_LOG("microtonal_positions");
    TEST_INPUT("positions", microtonal);
    TEST_OUTPUT("note_names", microtonal_result.noteNames);
    TEST_OUTPUT("cents", microtonal_result.centsInfo);

    TEST_CASE_LOG("transposed_c_major");
    TEST_INPUT("positions", transposed);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(transposed, sharps_diatonic).noteNames);

    TEST_CASE_LOG("rotated_c_major");
    TEST_INPUT("positions", rotated);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(rotated, flats_diatonic).noteNames);

    TEST_CASE_LOG("inverted_c_major_chord");
    TEST_INPUT("positions", inverted);
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(inverted, sharps_non_diatonic).noteNames);

    TEST_CASE_LOG("pentatonic_complement");
    TEST_INPUT("positions", pentatonic.complement());
    TEST_OUTPUT("note_names", system.positionVectorToNoteNames(pentatonic.complement(), sharps_non_diatonic).noteNames);
}

TEST_CASE("midi_numbers_empty_returns_empty_note_result", "[note_names]") {
    NoteNamingSystem system;
    NoteResult r = system.midiNumbersToNoteNames({}, NoteMapperOptions(true, false, 12));
    ASSERT_TRUE(r.noteNames.empty());
    ASSERT_TRUE(r.centsInfo.empty());
}

} // namespace
