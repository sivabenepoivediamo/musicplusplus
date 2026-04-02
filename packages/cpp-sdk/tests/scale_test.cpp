#include "test_support.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>

#include "../src/automations.h"
#include "../src/scale.h"
#include "../src/scale_dictionary.h"

using musicpp::Scale;
using musicpp::ScaleDatabase;
using musicpp::ScaleParams;
using musicpp::autoScale;
using musicpp::getRootNote;
using musicpp::interval_vector;
using musicpp::position_vector;

namespace {

template <typename Results>
std::vector<std::string> scale_labels(const Results& results) {
    std::vector<std::string> labels;
    labels.reserve(results.size());
    for (const auto& result : results) {
        labels.push_back(result.sheetName + "|" + result.scaleName);
    }
    std::sort(labels.begin(), labels.end());
    return labels;
}

TEST_CASE("scale_class_examples", "[scale]") {
    interval_vector interval_gen({2, 2, 1, 2, 2, 2, 1}, 0, 12);
    Scale scale1(interval_gen, 0, 0, 0, false, 0, false, 0);
    ASSERT_INTERVAL_VECTOR_EQ(scale1.getIntervals(), musicpp_test::ints({2, 2, 1, 2, 2, 2, 1}), 0, 12);
    ASSERT_POSITION_VECTOR_EQ(scale1.toPositions(), musicpp_test::ints({0, 2, 4, 5, 7, 9, 11}), 12);
    ASSERT_EQ(scale1.getRoot(), 0);
    ASSERT_EQ(scale1.getMode(), 0);
    ASSERT_TRUE(!scale1.getInvert());
    ASSERT_TRUE(!scale1.getMirror());

    position_vector pos_gen({0, 2, 4, 5, 7, 9, 11}, 12);
    Scale scale2(pos_gen, 3, 1, 0, false, 0, false, 0);
    ASSERT_INTERVAL_VECTOR_EQ(scale2.getIntervals(), musicpp_test::ints({2, 1, 2, 2, 2, 1, 2}), 3, 12);
    ASSERT_POSITION_VECTOR_EQ(scale2.toPositions(), musicpp_test::ints({3, 5, 6, 8, 10, 12, 13}), 12);
    ASSERT_EQ(scale2.getRoot(), 3);
    ASSERT_EQ(scale2.getMode(), 1);
    ASSERT_TRUE(scale2.getIsFromPositions());

    scale1.setRoot(5);
    ASSERT_POSITION_VECTOR_EQ(scale1.toPositions(), musicpp_test::ints({5, 7, 9, 10, 12, 14, 16}), 12);

    scale1.setMode(2);
    ASSERT_INTERVAL_VECTOR_EQ(scale1.getIntervals(), musicpp_test::ints({1, 2, 2, 2, 1, 2, 2}), 5, 12);
    ASSERT_POSITION_VECTOR_EQ(scale1.toPositions(), musicpp_test::ints({5, 6, 8, 10, 12, 13, 15}), 12);

    scale1.setInvert(true);
    scale1.setInversionAxis(4);
    ASSERT_INTERVAL_VECTOR_EQ(scale1.getIntervals(), musicpp_test::ints({2, 2, 2, 1, 2, 2, 1}), 5, 12);
    ASSERT_POSITION_VECTOR_EQ(scale1.toPositions(), musicpp_test::ints({5, 7, 9, 11, 12, 14, 16}), 12);
    ASSERT_EQ(scale1.getInversionAxis(), 4);
    ASSERT_TRUE(scale1.getInvert());

    Scale scale3(pos_gen, 0, 0, 0, false, 0, false, 0);
    ASSERT_INTERVAL_VECTOR_EQ(scale3.toIntervals(), musicpp_test::ints({2, 2, 1, 2, 2, 2, 1}), 0, 12);
    ASSERT_POSITION_VECTOR_EQ(scale3.toPositions(), musicpp_test::ints({0, 2, 4, 5, 7, 9, 11}), 12);

    TEST_CASE_LOG("scale_class_transformations");
    TEST_INPUT("interval_gen", interval_gen);
    TEST_INPUT("pos_gen", pos_gen);
    TEST_OUTPUT("scale1.positions", scale1.toPositions());
    TEST_OUTPUT("scale1.intervals", scale1.getIntervals());
    TEST_OUTPUT("scale2.positions", scale2.toPositions());
    TEST_OUTPUT("scale2.intervals", scale2.getIntervals());
}

TEST_CASE("autoscale_examples", "[scale]") {
    position_vector scale1({0, 2, 4, 5, 7, 9, 11});
    std::vector<int> notes1 = {61, 70};
    position_vector result1 = autoScale(scale1, notes1);
    ASSERT_POSITION_VECTOR_EQ(result1, musicpp_test::ints({1, 2, 4, 5, 7, 9, 10}), 12);

    position_vector scale2({0, 2, 4, 7, 9});
    std::vector<int> notes2 = {61, 65};
    ASSERT_POSITION_VECTOR_EQ(autoScale(scale2, notes2), musicpp_test::ints({1, 2, 5, 7, 9}), 12);

    position_vector scale3({7, 9, 11, 12, 14, 16, 18});
    std::vector<int> notes3 = {63, 70};
    position_vector result3 = autoScale(scale3, notes3);
    ASSERT_POSITION_VECTOR_EQ(result3, musicpp_test::ints({7, 10, 11, 12, 14, 15, 18}), 12);

    position_vector scale4({0, 2, 4, 6, 8, 10});
    std::vector<int> notes4 = {61, 63};
    ASSERT_POSITION_VECTOR_EQ(autoScale(scale4, notes4), musicpp_test::ints({1, 3, 4, 6, 8, 10}), 12);

    position_vector scale5({0, 2, 4, 5, 7, 9, 11});
    std::vector<int> notes5 = {63, 66};
    ASSERT_POSITION_VECTOR_EQ(autoScale(scale5, notes5), musicpp_test::ints({0, 2, 3, 5, 6, 9, 11}), 12);

    position_vector scale6({1, 3, 5, 6, 8, 10, 12});
    std::vector<int> notes6 = {67};
    ASSERT_POSITION_VECTOR_EQ(autoScale(scale6, notes6), musicpp_test::ints({1, 3, 5, 7, 8, 10, 12}), 12);

    position_vector scale7({0, 2, 4, 5, 7, 9, 11});
    std::vector<int> notes7 = {70};
    position_vector result7 = autoScale(scale7, notes7);
    ASSERT_POSITION_VECTOR_EQ(result7, musicpp_test::ints({0, 2, 4, 5, 7, 9, 10}), 12);

    TEST_CASE_LOG("autoscale_case_1");
    TEST_INPUT("scale", scale1);
    TEST_INPUT("notes", notes1);
    TEST_OUTPUT("result", result1);

    TEST_CASE_LOG("autoscale_case_2");
    TEST_INPUT("scale", scale2);
    TEST_INPUT("notes", notes2);
    TEST_OUTPUT("result", autoScale(scale2, notes2));

    TEST_CASE_LOG("autoscale_case_3");
    TEST_INPUT("scale", scale3);
    TEST_INPUT("notes", notes3);
    TEST_OUTPUT("result", result3);

    TEST_CASE_LOG("autoscale_case_4");
    TEST_INPUT("scale", scale4);
    TEST_INPUT("notes", notes4);
    TEST_OUTPUT("result", autoScale(scale4, notes4));

    TEST_CASE_LOG("autoscale_case_5");
    TEST_INPUT("scale", scale5);
    TEST_INPUT("notes", notes5);
    TEST_OUTPUT("result", autoScale(scale5, notes5));

    TEST_CASE_LOG("autoscale_case_6");
    TEST_INPUT("scale", scale6);
    TEST_INPUT("notes", notes6);
    TEST_OUTPUT("result", autoScale(scale6, notes6));

    TEST_CASE_LOG("autoscale_case_7");
    TEST_INPUT("scale", scale7);
    TEST_INPUT("notes", notes7);
    TEST_OUTPUT("result", result7);
}

TEST_CASE("scale_dictionary_examples", "[scale]") {
    ScaleDatabase db;

    std::vector<int> major_c = {0, 2, 4, 5, 7, 9, 11};
    std::vector<int> mixolydian_g = {7, 9, 11, 12, 14, 16, 17};
    std::vector<int> major_pentatonic_c = {0, 2, 4, 7, 9};
    std::vector<int> whole_tone_c = {0, 2, 4, 6, 8, 10};
    std::vector<int> chromatic_c = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::vector<int> harmonic_major_c = {0, 2, 4, 5, 7, 8, 11};
    std::vector<int> no_match = {0, 1, 4, 8};

    auto major_results = db.findScale(major_c);
    auto mixolydian_results = db.findScale(mixolydian_g);
    auto pentatonic_results = db.findScale(major_pentatonic_c);
    auto whole_tone_results = db.findScale(whole_tone_c);
    auto chromatic_results = db.findScale(chromatic_c);
    auto harmonic_major_results = db.findScale(harmonic_major_c);
    auto no_match_results = db.findScale(no_match);

    ASSERT_EQ(major_results.size(), static_cast<size_t>(1));
    ASSERT_EQ(mixolydian_results.size(), static_cast<size_t>(2));
    ASSERT_EQ(pentatonic_results.size(), static_cast<size_t>(1));
    ASSERT_EQ(whole_tone_results.size(), static_cast<size_t>(1));
    ASSERT_EQ(chromatic_results.size(), static_cast<size_t>(1));
    ASSERT_EQ(harmonic_major_results.size(), static_cast<size_t>(1));
    ASSERT_EQ(no_match_results.size(), static_cast<size_t>(0));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(major_results),
        musicpp_test::strings({"Major and minor scales|Ionian (Major)"}));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(mixolydian_results),
        musicpp_test::strings({
            "Indian Scales|Raga Khamach (desc)",
            "Major and minor scales|Mixolydian",
        }));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(pentatonic_results),
        musicpp_test::strings({"Pentatonic Scales|Major Pentatonic"}));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(whole_tone_results),
        musicpp_test::strings({"Symmetrical scales|Whole-Tone"}));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(chromatic_results),
        musicpp_test::strings({"Symmetrical scales|Chromatic"}));
    ASSERT_STRING_VECTOR_EQ(
        scale_labels(harmonic_major_results),
        musicpp_test::strings({"Modal Scales|Harmonic Major"}));
    ASSERT_STRING_VECTOR_EQ(scale_labels(no_match_results), std::vector<std::string>{});
    ASSERT_EQ(getRootNote(major_c), "C");
    ASSERT_EQ(getRootNote(musicpp_test::ints({2, 4, 6, 7, 9, 11, 13})), "D");

    TEST_CASE_LOG("dictionary_major");
    TEST_INPUT("query", major_c);
    TEST_OUTPUT("matches", scale_labels(major_results));

    TEST_CASE_LOG("dictionary_mixolydian");
    TEST_INPUT("query", mixolydian_g);
    TEST_OUTPUT("matches", scale_labels(mixolydian_results));

    TEST_CASE_LOG("dictionary_pentatonic");
    TEST_INPUT("query", major_pentatonic_c);
    TEST_OUTPUT("matches", scale_labels(pentatonic_results));

    TEST_CASE_LOG("dictionary_whole_tone");
    TEST_INPUT("query", whole_tone_c);
    TEST_OUTPUT("matches", scale_labels(whole_tone_results));

    TEST_CASE_LOG("dictionary_chromatic");
    TEST_INPUT("query", chromatic_c);
    TEST_OUTPUT("matches", scale_labels(chromatic_results));

    TEST_CASE_LOG("dictionary_harmonic_major");
    TEST_INPUT("query", harmonic_major_c);
    TEST_OUTPUT("matches", scale_labels(harmonic_major_results));

    TEST_CASE_LOG("dictionary_no_match");
    TEST_INPUT("query", no_match);
    TEST_OUTPUT("matches", scale_labels(no_match_results));
}

TEST_CASE("scale_params_chaining_and_merge", "[scale]") {
    ScaleParams a;
    ScaleParams b;
    b.withRoot(7).withMode(3).withDegree(2).withInvert(true).withInversionAxis(4).withMirror(true).withMirrorAxis(1);
    a.merge(b);
    ASSERT_EQ(a.root, 7);
    ASSERT_EQ(a.mode, 3);
    ASSERT_EQ(a.degree, 2);
    ASSERT_TRUE(a.invert);
    ASSERT_EQ(a.inversionAxis, 4);
    ASSERT_TRUE(a.mirror);
    ASSERT_EQ(a.mirrorAxis, 1);

    ScaleParams c;
    c.withRoot(1).withMode(0);
    ASSERT_EQ(c.root, 1);
    ASSERT_EQ(c.mode, 0);
}

} // namespace
