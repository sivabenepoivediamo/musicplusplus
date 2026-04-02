#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/automations.h"

namespace {

std::vector<std::vector<int>> to_raw_sequence(const std::vector<PositionVector>& sequence) {
    std::vector<std::vector<int>> result;
    result.reserve(sequence.size());
    for (const auto& vector : sequence) {
        result.push_back(vector.getData());
    }
    return result;
}

void assert_position_sequence_eq(
    const std::vector<PositionVector>& actual,
    const std::vector<std::vector<int>>& expected) {
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        ASSERT_POSITION_VECTOR_EQ(actual[i], expected[i], 12);
    }
}

TEST_CASE("automation_row_examples", "[automation]") {
    PositionVector scale({0, 2, 4, 5, 7, 9, 11});
    // Same stacked-thirds pattern as matrix_test / selection_test ({2,2,3}, offset 0), but a
    // non-zero offset here is deliberate: chord() does setOffset(degree + intervals.getOffset())
    // before select() on the scale (chord.h), and modalMatrix().rotate() keeps that offset on
    // each mode row — so the starting cumulative index shifts which rototranslation row is
    // closest to target {67,71,74} (this fixture expects translation_index 0, distance 2, …).
    IntervalVector criterion({2, 2, 3}, 35, 12);
    PositionVector reference({60, 64, 67});
    PositionVector target({67, 71, 74});
    std::vector<int> notes = {63};

    auto degree_row = degreeAutomation(scale, criterion, 3, target, 0);
    ASSERT_EQ(degree_row.getModeIndex(), 6);
    ASSERT_EQ(degree_row.getTranslationIndex(), 0);
    ASSERT_NEAR(degree_row.getDistance(), 2.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(degree_row.getVector(), musicpp_test::ints({65, 71, 74}), 12);

    auto voice_row = voiceLeadingAutomation(reference, target, 0);
    ASSERT_EQ(voice_row.getTranslation(), -2);
    ASSERT_EQ(voice_row.getCenter(), -2);
    ASSERT_NEAR(voice_row.getDistance(), 3.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(voice_row.getVector(), musicpp_test::ints({59, 62, 67}), 12);

    auto modal_row = modalInterchangeAutomation(scale, notes, 0);
    ASSERT_EQ(modal_row.getIndex(), 1);
    ASSERT_NEAR(modal_row.getDistance(), 2.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(modal_row.getVector(), musicpp_test::ints({0, 2, 3, 5, 7, 9, 10}), 12);

    auto modulation_row = modulationAutomation(scale, notes, 0);
    ASSERT_EQ(modulation_row.getTransposition(), 10);
    ASSERT_NEAR(modulation_row.getDistance(), 2.0, 1e-6);
    ASSERT_POSITION_VECTOR_EQ(modulation_row.getVector(), musicpp_test::ints({0, 2, 3, 5, 7, 9, 10}), 12);

    TEST_CASE_LOG("degree_automation");
    TEST_INPUT("scale", scale);
    TEST_INPUT("criterion", criterion);
    TEST_INPUT("degree", 3);
    TEST_INPUT("reference", target);
    TEST_OUTPUT("mode_index", degree_row.getModeIndex());
    TEST_OUTPUT("translation_index", degree_row.getTranslationIndex());
    TEST_OUTPUT("distance", degree_row.getDistance());
    TEST_OUTPUT("vector", degree_row.getVector());

    TEST_CASE_LOG("voice_leading_automation");
    TEST_INPUT("reference", reference);
    TEST_INPUT("target", target);
    TEST_OUTPUT("translation", voice_row.getTranslation());
    TEST_OUTPUT("center", voice_row.getCenter());
    TEST_OUTPUT("distance", voice_row.getDistance());
    TEST_OUTPUT("vector", voice_row.getVector());

    TEST_CASE_LOG("modal_interchange_automation");
    TEST_INPUT("scale", scale);
    TEST_INPUT("notes", notes);
    TEST_OUTPUT("mode_index", modal_row.getIndex());
    TEST_OUTPUT("distance", modal_row.getDistance());
    TEST_OUTPUT("vector", modal_row.getVector());

    TEST_CASE_LOG("modulation_automation");
    TEST_INPUT("scale", scale);
    TEST_INPUT("notes", notes);
    TEST_OUTPUT("transposition", modulation_row.getTransposition());
    TEST_OUTPUT("distance", modulation_row.getDistance());
    TEST_OUTPUT("vector", modulation_row.getVector());
}

TEST_CASE("automation_sequence_examples", "[automation]") {
    PositionVector scale({0, 2, 4, 5, 7, 9, 11});
    IntervalVector crit({2, 2, 2, 2});
    PositionVector I = chord(scale, crit, 0, 0, 4);
    PositionVector II = chord(scale, crit, 1, 0, 4);
    PositionVector V = chord(scale, crit, 4, 0, 4);
    PositionVector VI = chord(scale, crit, 5, 0, 4);
    PositionVector IV = chord(scale, crit, 3, 0, 4);
    PositionVector VII = chord(scale, crit, 6, 0, 4);
    PositionVector IV6 = chord(scale, crit, 1, 0, 4, 1);

    std::vector<PositionVector> chords{
        I, II, V, I, VI, IV, II, VII, I, V, VI, II, V, I
    };
    std::vector<int> complexities{0};
    std::vector<int> octave_rule{0, 5, 20, 7, 20, 10, 15, 0, 5, 15, 7, 20, 0, 15, 7};
    std::vector<int> degrees = {0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0};

    auto forward_voice_leading = forwardVoiceLeading(chords, complexities);
    auto backward_voice_leading = voiceLeadingAutomationSequentialBackward(chords, complexities);
    auto reference_voice_leading = voiceLeadingAutomationReference(chords, IV6, complexities);
    auto forward_degree = forwardDegreeAutomation(scale, crit, degrees, I, octave_rule);
    auto backward_degree = degreeAutomationSequentialBackward(scale, crit, degrees, I, complexities);
    auto reference_degree = degreeAutomationReference(scale, crit, degrees, I, complexities);

    assert_position_sequence_eq(
        forward_voice_leading,
        {
            {0, 4, 7, 11}, {0, 2, 5, 9}, {-1, 2, 5, 7}, {-1, 0, 4, 7},
            {-3, 0, 4, 7}, {-3, 0, 4, 5}, {-3, 0, 2, 5}, {-3, -1, 2, 5},
            {-5, -1, 0, 4}, {-5, -1, 2, 5}, {-5, -3, 0, 4}, {-7, -3, 0, 2},
            {-7, -5, -1, 2}, {-8, -5, -1, 0},
        });

    assert_position_sequence_eq(
        backward_voice_leading,
        {
            {7, 11, 12, 16}, {9, 12, 14, 17}, {7, 11, 14, 17}, {7, 11, 12, 16},
            {7, 9, 12, 16}, {5, 9, 12, 16}, {5, 9, 12, 14}, {5, 9, 11, 14},
            {4, 7, 11, 12}, {5, 7, 11, 14}, {4, 7, 9, 12}, {2, 5, 9, 12},
            {2, 5, 7, 11}, {0, 4, 7, 11},
        });

    assert_position_sequence_eq(
        reference_voice_leading,
        {
            {4, 7, 11, 12}, {5, 9, 12, 14}, {5, 7, 11, 14}, {4, 7, 11, 12},
            {7, 9, 12, 16}, {5, 9, 12, 16}, {5, 9, 12, 14}, {5, 9, 11, 14},
            {4, 7, 11, 12}, {5, 7, 11, 14}, {7, 9, 12, 16}, {5, 9, 12, 14},
            {5, 7, 11, 14}, {4, 7, 11, 12},
        });

    assert_position_sequence_eq(
        forward_degree,
        {
            {0, 4, 7, 11}, {2, 5, 9, 12}, {4, 7, 11, 14}, {4, 5, 9, 12},
            {5, 7, 11, 14}, {4, 7, 9, 12}, {5, 9, 11, 14}, {4, 7, 11, 12},
            {5, 9, 11, 14}, {4, 7, 9, 12}, {5, 7, 11, 14}, {4, 5, 9, 12},
            {2, 4, 7, 11}, {0, 2, 5, 9}, {-1, 0, 4, 7},
        });

    assert_position_sequence_eq(
        backward_degree,
        {
            {4, 7, 11, 12}, {5, 9, 12, 14}, {4, 7, 11, 14}, {5, 9, 12, 16},
            {5, 7, 11, 14}, {4, 7, 9, 12}, {2, 5, 9, 11}, {0, 4, 7, 11},
            {-1, 2, 5, 9}, {-3, 0, 4, 7}, {-1, 2, 5, 7}, {0, 4, 5, 9},
            {-1, 2, 4, 7}, {0, 2, 5, 9}, {0, 4, 7, 11},
        });

    assert_position_sequence_eq(
        reference_degree,
        {
            {0, 4, 7, 11}, {0, 2, 5, 9}, {2, 4, 7, 11}, {0, 4, 5, 9},
            {2, 5, 7, 11}, {0, 4, 7, 9}, {2, 5, 9, 11}, {0, 4, 7, 11},
            {2, 5, 9, 11}, {0, 4, 7, 9}, {2, 5, 7, 11}, {0, 4, 5, 9},
            {2, 4, 7, 11}, {0, 2, 5, 9}, {0, 4, 7, 11},
        });

    TEST_CASE_LOG("voice_leading_sequences");
    TEST_INPUT("scale", scale);
    TEST_INPUT("criterion", crit);
    TEST_INPUT("chords", to_raw_sequence(chords));
    TEST_INPUT("complexities", complexities);
    TEST_INPUT("reference_chord", IV6);
    TEST_OUTPUT("forward", to_raw_sequence(forward_voice_leading));
    TEST_OUTPUT("backward", to_raw_sequence(backward_voice_leading));
    TEST_OUTPUT("reference", to_raw_sequence(reference_voice_leading));

    TEST_CASE_LOG("degree_sequences");
    TEST_INPUT("scale", scale);
    TEST_INPUT("criterion", crit);
    TEST_INPUT("degrees", degrees);
    TEST_INPUT("seed", I);
    TEST_INPUT("complexities", complexities);
    TEST_INPUT("octave_rule", octave_rule);
    TEST_OUTPUT("forward", to_raw_sequence(forward_degree));
    TEST_OUTPUT("backward", to_raw_sequence(backward_degree));
    TEST_OUTPUT("reference", to_raw_sequence(reference_degree));
}

} // namespace
