#include "test_support.h"

#include <catch2/catch_test_macros.hpp>

#include "../src/melody.h"
#include "../src/rhythm_gen.h"

namespace {

using namespace musicpp;

std::vector<int> apply_ornament(
    int note,
    const std::vector<int>& chord,
    const std::vector<int>& scale,
    const std::vector<int>& chromatic,
    int mod,
    const std::vector<int>& ornaments,
    const std::vector<VectorModification::Type>& types) {
    Analysis analysis = hierarchy(note, chord, scale, chromatic, mod);
    auto expanded_types = ornamentTypesLoop(types, static_cast<int>(ornaments.size()));
    auto modifications = parseModifications(ornaments, expanded_types);
    return tripleSelect(analysis, modifications, chord, scale, chromatic, mod).results;
}

TEST_CASE("rhythm_generation_examples", "[rhythm]") {
    interval_vector ev = euclidean(16, 3, 0);
    ASSERT_INTERVAL_VECTOR_EQ(ev, musicpp_test::ints({5, 5, 6}), 0, 16);

    vector_set euc = vector_set::from_intervals(ev.data(), 16);
    ASSERT_POSITION_VECTOR_EQ(euc.positions(), musicpp_test::ints({0, 5, 10}), 16);

    vector_set euc2 = euc.mode(-1);
    ASSERT_POSITION_VECTOR_EQ(euc2.positions(), musicpp_test::ints({0, 6, 11}), 16);
    ASSERT_POSITION_VECTOR_EQ(euc2.transpose(1).positions(), musicpp_test::ints({1, 7, 12}), 16);
    ASSERT_POSITION_VECTOR_EQ(euc2.relative_mode(2).positions(), musicpp_test::ints({11, 16, 22}), 16);

    ASSERT_POSITION_VECTOR_EQ(CloughDouthettVector(16, 3, 0), musicpp_test::ints({0, 5, 10}), 16);
    ASSERT_POSITION_VECTOR_EQ(deepRhythm(16, 3, 5, 0), musicpp_test::ints({0, 5, 10}), 16);
    ASSERT_ONSET_VECTOR_EQ(
        tihai(16, 3, false, 0),
        musicpp_test::ints({1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}),
        0,
        16);

    TEST_CASE_LOG("euclidean_rhythm");
    TEST_INPUT("args", musicpp_test::ints({16, 3, 0}));
    TEST_OUTPUT("intervals", ev);
    TEST_OUTPUT("positions", euc.positions());
    TEST_OUTPUT("mode_minus_1", euc2.positions());
    TEST_OUTPUT("mode_minus_1_transpose_1", euc2.transpose(1).positions());
    TEST_OUTPUT("mode_minus_1_relative_mode_2", euc2.relative_mode(2).positions());
    TEST_OUTPUT("clough_douthett", CloughDouthettVector(16, 3, 0));
    TEST_OUTPUT("deep_rhythm", deepRhythm(16, 3, 5, 0));

    TEST_CASE_LOG("tihai");
    TEST_INPUT("args", std::string("length=16 repeats=3 rest=false offset=0"));
    TEST_OUTPUT("pattern", tihai(16, 3, false, 0));
}

TEST_CASE("melody_examples", "[rhythm]") {
    const std::vector<int> chord = {0, 4, 7};
    const std::vector<int> scale = {0, 2, 4, 5, 7, 9, 11};
    const std::vector<int> chromatic = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    const int mod = 12;

    auto loop_10 = ornamentLoop({1, 0}, 7);
    auto diminution_result = diminution(0, 4, false, true);
    auto run_result = run(4, 6, true);
    auto run2_result = run2(3, -3);
    auto scale_ornament = apply_ornament(60, chord, scale, chromatic, mod, ornamentLoop({1, 0}, 7), {VectorModification::Type::SCALE});
    auto downward_ornament = apply_ornament(60, chord, scale, chromatic, mod, ornamentLoop({-1, 0}, 5), {VectorModification::Type::SCALE});
    auto mixed_ornament = apply_ornament(60, chord, scale, chromatic, mod, ornamentLoop({1, 0, -1, 0}, 5), {VectorModification::Type::SCALE, VectorModification::Type::CHROMATIC});
    auto chord_run = apply_ornament(60, chord, scale, chromatic, mod, run(3, 5, true), {VectorModification::Type::CHORD});
    auto chord_run2 = apply_ornament(60, chord, scale, chromatic, mod, run2(0, 3), {VectorModification::Type::CHORD});
    auto phrase = apply_ornament(
        60,
        chord,
        scale,
        chromatic,
        mod,
        {0, 1, 2, 0, 0, 1, 2, 0, 2, 3, 4, 2, 3, 4},
        {VectorModification::Type::SCALE});

    ASSERT_VECTOR_EQ(loop_10, std::vector<int>({0, 1, 0, 1, 0, 1, 0}));
    ASSERT_VECTOR_EQ(diminution_result, std::vector<int>({0, 1, 2, 0}));
    ASSERT_VECTOR_EQ(run_result, std::vector<int>({-1, 0, 1, 2, 3, 4}));
    ASSERT_VECTOR_EQ(run2_result, std::vector<int>({3, 2, 1, 0, -1, -2, -3}));

    ASSERT_VECTOR_EQ(scale_ornament, std::vector<int>({60, 62, 60, 62, 60, 62, 60}));

    ASSERT_VECTOR_EQ(downward_ornament, std::vector<int>({60, 59, 60, 59, 60}));

    ASSERT_VECTOR_EQ(mixed_ornament, std::vector<int>({60, 62, 60, 59, 60}));

    ASSERT_VECTOR_EQ(chord_run, std::vector<int>({55, 60, 64, 67, 72}));

    ASSERT_VECTOR_EQ(chord_run2, std::vector<int>({60, 64, 67, 72}));

    ASSERT_VECTOR_EQ(phrase, std::vector<int>({60, 62, 64, 60, 60, 62, 64, 60, 64, 65, 67, 64, 65, 67}));

    TEST_CASE_LOG("melodic_generators");
    TEST_INPUT("note", 60);
    TEST_INPUT("chord", chord);
    TEST_INPUT("scale", scale);
    TEST_INPUT("chromatic", chromatic);
    TEST_INPUT("mod", mod);
    TEST_OUTPUT("ornament_loop", loop_10);
    TEST_OUTPUT("diminution", diminution_result);
    TEST_OUTPUT("run", run_result);
    TEST_OUTPUT("run2", run2_result);

    TEST_CASE_LOG("melodic_realizations");
    TEST_INPUT("note", 60);
    TEST_INPUT("chord", chord);
    TEST_INPUT("scale", scale);
    TEST_OUTPUT("scale_ornament", scale_ornament);
    TEST_OUTPUT("downward_ornament", downward_ornament);
    TEST_OUTPUT("mixed_ornament", mixed_ornament);
    TEST_OUTPUT("chord_run", chord_run);
    TEST_OUTPUT("chord_run2", chord_run2);
    TEST_OUTPUT("phrase", phrase);
}

} // namespace
